#pragma once

#include <proxies/hwui/controls/LeftAlignedLabel.h>
#include <proxies/hwui/Oleds.h>

class LeftAligned9pxCaptionLabel : public LeftAlignedLabel
{
 public:
  using LeftAlignedLabel::LeftAlignedLabel;

  std::shared_ptr<Font> getFont() const override;
  void setFontColor(FrameBuffer& fb) const override;
};
