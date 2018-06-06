#include "Boled.h"

#ifdef _DEVELOPMENT_PC

constexpr auto framebufferDimX = 256;
constexpr auto framebufferDimY = 96;

Boled::Boled()
{
}

Boled::~Boled()
{
}

void Boled::setBuffer(WebSocketServer::tMessage frameBuffer)
{
  m_frameBuffer = frameBuffer;
  queue_draw();
}

bool Boled::on_draw(const ::Cairo::RefPtr<::Cairo::Context> & cr)
{
  static std::map<uint8_t, std::tuple<uint8_t, uint8_t, uint8_t>> colorMap =
  {
   { 0x00, std::make_tuple(43, 32, 21) },
   { 0x02, std::make_tuple(77, 60, 10) },
   { 0x05, std::make_tuple(103, 81, 12) },
   { 0x06, std::make_tuple(128, 102, 16) },
   { 0x0A, std::make_tuple(179, 142, 21) },
   { 0x0B, std::make_tuple(204, 162, 24) },
   { 0x0F, std::make_tuple(255, 203, 31) }
  };

  if(m_frameBuffer)
  {
    gsize len = 0;
    const int8_t *data = reinterpret_cast<const int8_t *>(m_frameBuffer->get_data(len));

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

        auto &rgb = colorMap.at(data[idx]);
        cr->rectangle(offsetX + x * multiplier, y * multiplier, multiplier, multiplier);
        cr->set_source_rgb(std::get < 0 > (rgb) / 256.0, std::get < 1 > (rgb) / 256.0, std::get < 2 > (rgb) / 256.0);
        cr->fill();
      }
    }
  }
  return true;
}

#endif
