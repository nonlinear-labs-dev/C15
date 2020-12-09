#include <png++/image.hpp>
#include <CompileTimeOptions.h>
#include "PNGControl.h"
#include <map>
#include <proxies/hwui/FrameBuffer.h>
#include <nltools/logging/Log.h>

PNGControl::PNGControl(const Rect& pos)
    : Control(pos)
{
}

PNGControl::PNGControl(const Rect& pos, const std::string& imagePath)
    : Control(pos)
{
  loadImage(imagePath);
}

void PNGControl::setOffset(const std::pair<int, int>& offset)
{
  offsetX = offset.first;
  offsetY = offset.second;
}

bool PNGControl::redraw(FrameBuffer& fb)
{
  using FBC = FrameBufferColors;

  auto pos = getPosition();

  if(!m_transparentBackground && isVisible())
  {
    drawBackground(fb);
  }

  if(m_imagePath.empty())
  {
    return true;
  }

  if(!isVisible())
  {
    return true;
  }

  for(auto y = 0; y < m_coloredPixels.size(); y++)
  {
    auto& row = m_coloredPixels[y];
    for(auto x = 0; x < row.size(); x++)
    {
      auto pixel = row[x];
      if(pixel != FBC::Transparent)
      {
        fb.setColor(pixel);
        fb.setPixel(pos.getLeft() + x + offsetX, pos.getTop() + y + offsetY);
      }
    }
  }
  return true;
}

void PNGControl::drawBackground(FrameBuffer& fb)
{
  using FBC = FrameBufferColors;
  fb.setColor(FBC::C43);
  fb.fillRect(getPosition());
}

void PNGControl::loadImage(const std::string& l)
{
  if(l.empty())
  {
    m_imagePath = "";
  }

  try
  {
    m_image.read(getResourcesDir() + "/png/" + l);
    m_imagePath = getResourcesDir() + "/png/" + l;
  }
  catch(const std::runtime_error& err)
  {
    nltools::Log::error(__LINE__, __FILE__, err.what(), "File: ", l);
  }

  recalculatePixels();
}

void PNGControl::setColor(FrameBufferColors color)
{
  m_color = color;
  recalculatePixels();
}

void PNGControl::setTransparent(bool transparent)
{
  m_transparentBackground = transparent;
  recalculatePixels();
}

void PNGControl::useImageColors(bool useColors)
{
  m_useColors = useColors;
  recalculatePixels();
}

void PNGControl::recalculatePixels()
{
  using FBC = FrameBufferColors;

  static std::vector<std::pair<int, FBC>> colors
      = { { 43, FBC::C43 },   { 77, FBC::C77 },   { 103, FBC::C103 }, { 128, FBC::C128 },
          { 179, FBC::C179 }, { 204, FBC::C204 }, { 255, FBC::C255 } };

  auto nearestColorForGrey = [&](int grey) {
    auto minDiff = std::numeric_limits<int>::max();
    auto nearestColor = FBC::C43;

    for(auto c : colors)
    {
      auto diff = std::abs(c.first - grey);
      if(diff < minDiff)
      {
        minDiff = diff;
        nearestColor = c.second;
      }
    }

    return nearestColor;
  };

  m_coloredPixels.clear();

  for(auto y = 0; y < m_image.get_height(); y++)
  {
    auto& row = m_coloredPixels.emplace_back();

    for(auto x = 0; x < m_image.get_width(); x++)
    {
      auto pixel = m_image.get_pixel(x, y);
      auto& outColoredPixel = row.emplace_back();

      if(pixel.alpha != 0)
      {
        if(m_useColors)
        {
          auto greyRaw = pixel.red;
          outColoredPixel = nearestColorForGrey(greyRaw);
        }
        else
        {
          outColoredPixel = m_color;
        }
      }
      else
      {
        outColoredPixel = FBC::Transparent;
      }
    }
  }

  assert(m_coloredPixels.size() == m_image.get_height());

  for(auto& rows : m_coloredPixels)
  {
    assert(rows.size() == m_image.get_width());
  }

  setDirty();
}
