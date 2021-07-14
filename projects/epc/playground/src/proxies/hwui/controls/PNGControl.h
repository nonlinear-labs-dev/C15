#pragma once
#include "Control.h"
#include <png++/png.hpp>
#include <proxies/hwui/HWUIEnums.h>

class PNGControl : public Control
{
 public:
  explicit PNGControl(const Rect& pos);
  PNGControl(const Rect& pos, const std::string& imagePath);

  void useImageColors(bool useColors);
  bool redraw(FrameBuffer& fb) override;
  void loadImage(const std::string& l);
  void setColor(FrameBufferColors color);
  void setOffset(const std::pair<int, int>& offset);
  void setTransparent(bool transparent);
  void drawBackground(FrameBuffer& fb) override;

 private:
  void recalculatePixels();

  bool m_useColors = false;
  bool m_transparentBackground = true;

  int offsetX = 0;
  int offsetY = 0;

  FrameBufferColors m_color = FrameBufferColors::C128;
  std::string m_imagePath;
  png::image<png::rgba_pixel> m_image;
  std::vector<std::vector<FrameBufferColors>> m_coloredPixels;
};
