#include "Boled.h"

#ifdef _DEVELOPMENT_PC

#include <png++/png.hpp>
#include <algorithm>

constexpr auto frameBufferDimX = 256;
constexpr auto frameBufferDimY = 96;

typedef std::tuple<uint8_t, uint8_t, uint8_t> RGB;
static std::map<uint8_t, RGB> colorMap
    = { { 0x00, std::make_tuple(43, 32, 21) },   { 0x02, std::make_tuple(77, 60, 10) },
        { 0x05, std::make_tuple(103, 81, 12) },  { 0x06, std::make_tuple(128, 102, 16) },
        { 0x0A, std::make_tuple(179, 142, 21) }, { 0x0B, std::make_tuple(204, 162, 24) },
        { 0x0F, std::make_tuple(255, 203, 31) } };

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
  auto data = reinterpret_cast<const uint8_t *>(m_frameBuffer.pixels);

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

void Boled::exportBoled() const
{
  exportOled(0, 0, 256, 64, createPNGFileName("Boled"));
}

void Boled::exportSoled() const
{
  exportOled(0, 64, 128, 32, createPNGFileName("Soled"));
}

std::string Boled::createPNGFileName(const std::string &prefix) const
{
  auto time = std::time(nullptr);
  auto fileName = "/tmp/" + prefix + "-" + std::ctime(&time) + ".png";
  std::replace(fileName.begin(), fileName.end(), ' ', '-');
  return fileName;
}

void Boled::exportOled(uint32_t x, uint32_t y, uint32_t w, uint32_t h, const std::string &fileName) const
{
  auto data = reinterpret_cast<const uint8_t *>(m_frameBuffer.pixels);
  png::image<png::rgb_pixel> boledFile(w, h);

  for(png::uint_32 iy = 0; iy < h; ++iy)
  {
    for(png::uint_32 ix = 0; ix < w; ++ix)
    {
      int idx = (iy + y) * frameBufferDimX + (ix + x);
      auto pixel = data[idx];
      try
      {
        auto rgb = colorMap.at(pixel);
        boledFile[iy][ix] = png::rgb_pixel(std::get<0>(rgb), std::get<1>(rgb), std::get<2>(rgb));
      }
      catch(...)
      {
        boledFile[iy][ix] = png::rgb_pixel();
      }
    }
  }

  boledFile.write(fileName);
}

#endif
