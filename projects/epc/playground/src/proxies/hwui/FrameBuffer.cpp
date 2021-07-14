#include <proxies/hwui/controls/Point.h>
#include <proxies/hwui/FrameBuffer.h>
#include <string.h>
#include <proxies/hwui/panel-unit/RotaryEncoder.h>
#include <cmath>
#include <Application.h>
#include <Options.h>

#include <nltools/messaging/Message.h>
#include <nltools/messaging/WebSocketJsonAPI.h>
#include <proxies/hwui/HWUI.h>

FrameBuffer::StackScopeGuard::StackScopeGuard(FrameBuffer *fb)
    : m_fb(fb)
{
}

FrameBuffer::StackScopeGuard::StackScopeGuard(StackScopeGuard &&other) noexcept
    : m_fb(nullptr)
{
  std::swap(m_fb, other.m_fb);
}

FrameBuffer::StackScopeGuard::~StackScopeGuard() = default;

FrameBuffer::Clip::Clip(FrameBuffer *fb, const Rect &rect)
    : StackScopeGuard(fb)
{
  auto intersection = rect.getIntersection(m_fb->m_clips.top());
  fb->m_clips.push(intersection);
}

FrameBuffer::Clip::Clip(Clip &&other) noexcept
    : StackScopeGuard(std::move(other))
{
}

FrameBuffer::Clip::~Clip()
{
  if(m_fb)
    m_fb->m_clips.pop();
}

bool FrameBuffer::Clip::isEmpty() const
{
  return m_fb->m_clips.top().isEmpty();
}

FrameBuffer::Offset::Offset(FrameBuffer *fb, const Point &offset)
    : StackScopeGuard(fb)
{
  m_fb->m_offsets.push(m_fb->m_offsets.top() + offset);
}

FrameBuffer::Offset::Offset(Offset &&other) noexcept
    : StackScopeGuard(std::move(other))
{
}

FrameBuffer::Offset::~Offset()
{
  if(m_fb)
    m_fb->m_offsets.pop();
}

FrameBuffer::FrameBuffer()
    : m_api(std::make_unique<nltools::msg::WebSocketJsonAPI>(PLAYGROUND_OLED_WEBSOCKET_PORT,
                                                             [this](auto) { return m_backBuffer; }))
{
  initStacks();
  openAndMap();
  clear();

  nltools::msg::onConnectionEstablished(nltools::msg::EndPoint::Oled, [this] {
    m_oledCurrentlyShowsMessageId = 0;

    if(!swapBuffers())
      nltools::Log::warning("Could not send new framebuffer to BBBB");
  });

  nltools::msg::receive<nltools::msg::OLEDState>(nltools::msg::EndPoint::Playground, [this](const auto &msg) {
    m_oledCurrentlyShowsMessageId = msg.displaysMessageId;

    if(m_oledsDirty)
      swapBuffers();
  });
}

void FrameBuffer::initStacks()
{
  m_clips.push(Rect(0, 0, 256, 64 + 32));
  m_offsets.push(Point(0, 0));
}

void FrameBuffer::openAndMap()
{
  constexpr const auto bytesPerPixel = 1;
  constexpr const auto buffersize = 256 * 96 * bytesPerPixel;
  m_backBuffer.resize(buffersize);
}

FrameBuffer::~FrameBuffer()
{
  unmapAndClose();
}

void FrameBuffer::setPixel(tCoordinate x, tCoordinate y)
{
  const auto &offset = m_offsets.top();
  setOffsetPixel(x + offset.getX(), y + offset.getY());
}

inline void FrameBuffer::setOffsetPixel(tCoordinate x, tCoordinate y)
{
  if(m_clips.top().contains(x, y))
    setRawPixel(x, y);
}

inline void FrameBuffer::setRawPixel(tCoordinate x, tCoordinate y)
{
  const long index = getIndex(x, y);
  m_backBuffer[index] = static_cast<tPixel>(m_currentColor);

  if(m_perPixelDebug)
  {
    swapBuffers();
    nltools::msg::flush(nltools::msg::EndPoint::Oled, std::chrono::seconds(1));
  }
}

inline long FrameBuffer::getIndex(tCoordinate x, tCoordinate y) const
{
  return (x + y * 256);
}

void FrameBuffer::unmapAndClose()
{
}

void FrameBuffer::clear()
{
  std::fill(m_backBuffer.begin(), m_backBuffer.end(), 0);

  if(m_perPixelDebug)
  {
    swapBuffers();
    nltools::msg::flush(nltools::msg::EndPoint::Oled, std::chrono::seconds(1));
  }
}

void FrameBuffer::setPerPixelDebug(bool onOff)
{
  m_perPixelDebug = onOff;
}

void FrameBuffer::setColor(const Colors &c)
{
  m_currentColor = c;
  g_assert(isValidColor(c));
}

void FrameBuffer::fiddleColor(tPixel p)
{
  m_currentColor = (Colors)(p);
}

FrameBuffer::Colors FrameBuffer::getColor() const
{
  return m_currentColor;
}

void FrameBuffer::fillRect(tCoordinate left, tCoordinate top, tCoordinate width, tCoordinate height)
{
  fillRect(Rect(left, top, width, height));
}

void FrameBuffer::fillRect(const Rect &rect)
{
  Rect fill = rect.getMovedBy(m_offsets.top()).getIntersection(m_clips.top());

  if(!fill.isEmpty() && m_currentColor != Colors::Transparent)
  {
    auto left = fill.getLeft();
    auto width = fill.getWidth();
    auto bottom = fill.getBottom();

    for(auto y = fill.getTop(); y <= bottom; y++)
      drawRawHorizontalLine(left, y, width);
  }
}

void FrameBuffer::fillCircle(const Point &center, int radius)
{
  if(radius % 2 == 0 && m_currentColor != Colors::Transparent)
  {
    auto middleX = center.getX() - 0.5;
    auto middleY = center.getY() - 0.5;
    for(auto y = center.getY() - radius; y < center.getY() + radius; y++)
    {
      for(auto x = center.getX() - radius; x < center.getX() + radius; x++)
      {
        auto distance = sqrt(pow(x - middleX, 2) + pow(y - middleY, 2));
        if(distance <= radius)
        {
          setPixel(x, y);
        }
      }
    }
  }
}

inline void FrameBuffer::drawRawHorizontalLine(tCoordinate x, tCoordinate y, tCoordinate length)
{
  if(m_currentColor != Colors::Transparent)
  {
    auto fromIdx = getIndex(x, y);
    auto data = m_backBuffer.data() + fromIdx;

    for(long i = 0; i < length; i++)
      data[i] = static_cast<tPixel>(m_currentColor);

    if(m_perPixelDebug)
    {
      swapBuffers();
      nltools::msg::flush(nltools::msg::EndPoint::Oled, std::chrono::seconds(1));
    }
  }
}

void FrameBuffer::drawRect(tCoordinate x, tCoordinate y, tCoordinate width, tCoordinate height)
{
  if(m_currentColor != Colors::Transparent)
    drawRect(Rect(x, y, width, height));
}

void FrameBuffer::drawRect(const Rect &rect)
{
  if(!rect.isEmpty() && m_currentColor != Colors::Transparent)
  {
    drawHorizontalLine(rect.getLeft(), rect.getTop(), rect.getWidth());
    drawHorizontalLine(rect.getLeft(), rect.getBottom(), rect.getWidth());
    drawVerticalLine(rect.getLeft(), rect.getTop(), rect.getHeight());
    drawVerticalLine(rect.getRight(), rect.getTop(), rect.getHeight());
  }
}

void FrameBuffer::drawHorizontalLine(tCoordinate x, tCoordinate y, tCoordinate length)
{
  if(m_currentColor != Colors::Transparent)
  {
    auto &clip = m_clips.top();
    auto &offset = m_offsets.top();

    x += offset.getX();
    y += offset.getY();

    if(y >= clip.getTop() && y <= clip.getBottom() && length > 0)
    {
      auto left = std::max(x, clip.getLeft());
      auto right = std::min(x + length, clip.getLeft() + clip.getWidth());
      drawRawHorizontalLine(left, y, right - left);
    }
  }
}

void FrameBuffer::drawVerticalLine(tCoordinate x, tCoordinate y, tCoordinate length)
{
  if(m_currentColor != Colors::Transparent)
  {
    auto &clip = m_clips.top();
    auto &offset = m_offsets.top();

    x += offset.getX();
    y += offset.getY();

    if(x >= clip.getLeft() && x <= clip.getRight() && length > 0)
    {
      auto top = std::max(y, clip.getTop());
      auto bottom = std::min(y + length, clip.getTop() + clip.getHeight());

      for(auto i = top; i < bottom; i++)
        setRawPixel(x, i);
    }
  }
}

bool FrameBuffer::swapBuffers()
{
  using namespace nltools::msg;

  if(m_api->hasClients())
  {
    m_api->sendAllUpdating(m_backBuffer);
  }

  if(Application::get().getOptions()->sendBBBBTurnaroundTimestamps())
  {
    SetTimestampedOledMessage msg {};
    msg.m_timestamp
        = Application::get().getHWUI()->getPanelUnit().getEditPanel().getKnob().resetOldestPendingTimestamp();
    memcpy(msg.m_oledMessage.pixels, m_backBuffer.data(), m_backBuffer.size());
    return send(EndPoint::Oled, msg);
  }
  else
  {
    if(m_oledCurrentlyShowsMessageId == 0 || m_oledCurrentlyShowsMessageId == m_oledMessageId)
    {
      SetOLEDMessage msg {};
      m_oledsDirty = false;
      msg.messageId = ++m_oledMessageId;
      memcpy(msg.pixels, m_backBuffer.data(), m_backBuffer.size());
      return send(EndPoint::Oled, msg);
    }
    m_oledsDirty = true;
    return true;
  }
}

bool FrameBuffer::isValidColor(Colors c) const
{
  return c == Colors::C43 || c == Colors::C77 || c == Colors::C103 || c == Colors::C128 || c == Colors::C179
      || c == Colors::C204 || c == Colors::C255 || c == Colors::Transparent;
}

FrameBuffer::Clip FrameBuffer::clip(const Rect &rect)
{
  return Clip(this, rect);
}

FrameBuffer::Clip FrameBuffer::clipRespectingOffset(const Rect &rect)
{
  return clip(rect.getMovedBy(m_offsets.top()));
}

FrameBuffer::Offset FrameBuffer::offset(const Point &offset)
{
  return Offset(this, offset);
}

const std::vector<FrameBuffer::tPixel> &FrameBuffer::getBackBuffer() const
{
  return m_backBuffer;
}
