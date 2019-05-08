#include "PhysicalControlSlider.h"
#include <parameters/PhysicalControlParameter.h>

void PhysicalControlSlider::drawSliderBackground(FrameBuffer &fb)
{
  super::drawSliderBackground(fb);

  if(auto p = dynamic_cast<PhysicalControlParameter *>(getParameter()))
  {
    auto returnMode = p->getReturnMode();
    const auto &r = getPosition();
    const int centerMarkWidth = 6;

    switch(returnMode)
    {
      case PhysicalControlParameter::ReturnMode::Center:
        fb.setColor(FrameBuffer::Colors::C128);
        fb.fillRect(r.getCenter().getX() - centerMarkWidth / 2, r.getTop(), centerMarkWidth, r.getHeight());
        break;

      case PhysicalControlParameter::ReturnMode::Zero:
        fb.setColor(FrameBuffer::Colors::C128);
        fb.fillRect(r.getLeft(), r.getTop(), centerMarkWidth, r.getHeight());
        break;

      case PhysicalControlParameter::ReturnMode::None:
      default:
        break;
    }
  }
}
