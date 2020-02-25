#include "LeftAligned9pxCaptionLabel.h"
#include "proxies/hwui/FrameBuffer.h"

std::shared_ptr<Font> LeftAligned9pxCaptionLabel::getFont() const
{
  return Oleds::get().getFont("Emphase-9-Regular", 9);
}

void LeftAligned9pxCaptionLabel::setFontColor(FrameBuffer& fb) const
{
  if(isHighlight())
    fb.setColor(FrameBufferColors::C255);
  else
    fb.setColor(FrameBufferColors::C179);
}
