#include "ModuleCaption.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "parameters/Parameter.h"
#include "proxies/hwui/panel-unit/boled/BOLED.h"

ModuleCaption::ModuleCaption(const Rect &pos)
    : super(pos)
{
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::hide<0>(sigc::mem_fun(this, &ModuleCaption::onParameterSelected)));
}

ModuleCaption::~ModuleCaption()
{
}

void ModuleCaption::onParameterSelected(Parameter *newOne)
{
  if(newOne)
  {
    auto group = newOne->getParentGroup();
    auto label = group->getShortName();
    setText(label);
  }
}

bool ModuleCaption::redraw(FrameBuffer &fb)
{
  const Rect &r = getPosition();

  fb.setColor(FrameBuffer::Colors::C128);
  fb.fillRect(r.getX(), r.getY(), r.getWidth(), r.getHeight());

  super::redraw(fb);
  return true;
}

void ModuleCaption::setFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::Colors::C43);
}

shared_ptr<Font> ModuleCaption::getFont() const
{
  return Oleds::get().getFont("Emphase_8_Regular", getFontHeight());
}

int ModuleCaption::getFontHeight() const
{
  return 8;
}
