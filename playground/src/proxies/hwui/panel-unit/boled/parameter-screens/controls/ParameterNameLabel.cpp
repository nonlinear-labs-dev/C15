#include "ParameterNameLabel.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "parameters/Parameter.h"
#include "proxies/hwui/panel-unit/boled/BOLED.h"

ParameterNameLabel::ParameterNameLabel(const Rect &pos)
    : Label(pos)
{
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::hide<0>(sigc::mem_fun(this, &ParameterNameLabel::onParameterSelected)));
}

ParameterNameLabel::~ParameterNameLabel()
{
}

void ParameterNameLabel::onParameterSelected(Parameter *param)
{
  if(param)
  {
    m_connection.disconnect();
    m_connection = param->onParameterChanged(mem_fun(this, &ParameterNameLabel::onParameterChanged));
  }
}

void ParameterNameLabel::setFontColor(FrameBuffer &fb) const
{
  if(isHighlight())
    fb.setColor(FrameBuffer::Colors::C255);
  else
    fb.setColor(FrameBuffer::Colors::C128);
}

shared_ptr<Font> ParameterNameLabel::getFont() const
{
  return Oleds::get().getFont("Emphase_9_Bold", getFontHeight());
}

void ParameterNameLabel::onParameterChanged(const Parameter *param)
{
  setText(param->getLongName());
}

int ParameterNameLabel::getFontHeight() const
{
  return 9;
}
