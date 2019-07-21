#include "Boled.h"

#ifdef _DEVELOPMENT_PC

constexpr auto framebufferDimX = 256;
constexpr auto framebufferDimY = 96;

Boled::Boled()
{
  memset(m_frameBuffer.pixels, 0, sizeof(m_frameBuffer.pixels));
}

Boled::~Boled()
{
}

void Boled::setBuffer(const nltools::msg::SetOLEDMessage &msg)
{
  m_frameBuffer = msg;
  queue_draw();
}

bool Boled::on_draw(const ::Cairo::RefPtr<::Cairo::Context> &cr)
{
  typedef std::tuple<uint8_t, uint8_t, uint8_t> RGB;
  static std::map<uint8_t, RGB> colorMap
      = { { 0x00, std::make_tuple(43, 32, 21) },   { 0x02, std::make_tuple(77, 60, 10) },
          { 0x05, std::make_tuple(103, 81, 12) },  { 0x06, std::make_tuple(128, 102, 16) },
          { 0x0A, std::make_tuple(179, 142, 21) }, { 0x0B, std::make_tuple(204, 162, 24) },
          { 0x0F, std::make_tuple(255, 203, 31) } };

  auto data = reinterpret_cast<const uint8_t *>(m_frameBuffer.pixels);

  auto frameBufferDimX = framebufferDimX;
  auto frameBufferDimY = framebufferDimY;

  auto width = get_allocated_width();
  auto height = get_allocated_height();

  int multiplier = std::min(width / frameBufferDimX, height / frameBufferDimY);
  auto offsetX = (width - (frameBufferDimX * multiplier)) / 2;

  for(int y = 0; y < frameBufferDimY; y++)
  {
    for(int x = 0; x < frameBufferDimX; x++)
    {
      int idx = y * frameBufferDimX + x;

      RGB rgb;
      try
      {
        rgb = colorMap.at(data[idx]);
      }
      catch(...)
      {
        rgb = { 187, 0, 187 };
      }
      cr->rectangle(offsetX + x * multiplier, y * multiplier, multiplier, multiplier);
      cr->set_source_rgb(std::get<0>(rgb) / 256.0, std::get<1>(rgb) / 256.0, std::get<2>(rgb) / 256.0);
      cr->fill();
    }
  }
  return true;
}

#endif
