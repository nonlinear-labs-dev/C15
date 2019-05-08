#include "HeadlineLabel.h"
#include <proxies/hwui/Oleds.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/MacroControlParameter.h>

HeadlineLabel::HeadlineLabel(const Rect &pos)
    : super(pos)
{
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(mem_fun(this, &HeadlineLabel::updateText));
  Application::get().getPresetManager()->getEditBuffer()->getSelectedParameter()->onParameterChanged(
      mem_fun(this, &HeadlineLabel::updateMc));
}

shared_ptr<Font> HeadlineLabel::getFont() const
{
  return Oleds::get().getFont("Emphase_9_Bold", getFontHeight());
}

int HeadlineLabel::getFontHeight() const
{
  return 9;
}

void HeadlineLabel::setFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::Colors::C255);
}

bool HeadlineLabel::redraw(FrameBuffer &fb)
{
  const Rect &r = getPosition();

  fb.setColor(FrameBuffer::Colors::C77);
  fb.fillRect(r.getX(), r.getY(), r.getWidth(), r.getHeight());

  super::redraw(fb);
  return true;
}

void HeadlineLabel::updateText(Parameter *oldParam, Parameter *newParam)
{
  if(newParam)
    setText(newParam->getLongName());
}

void HeadlineLabel::updateMc(const Parameter *param)
{
  auto mc = dynamic_cast<const MacroControlParameter *>(param);

  if(mc != nullptr)
  {
    setText(mc->getLongName());
  }
}
