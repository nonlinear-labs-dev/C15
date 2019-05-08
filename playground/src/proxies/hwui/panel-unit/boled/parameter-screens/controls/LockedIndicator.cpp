#include "LockedIndicator.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "parameters/Parameter.h"
#include "proxies/hwui/panel-unit/boled/BOLED.h"

LockedIndicator::LockedIndicator(const Rect &pos)
    : super(pos)
{
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::hide<0>(sigc::mem_fun(this, &LockedIndicator::onParameterSelected)));
}

LockedIndicator::~LockedIndicator()
{
}

void LockedIndicator::onParameterSelected(Parameter *newOne)
{
  if(newOne)
  {
    m_groupConnection.disconnect();
    m_groupConnection
        = newOne->getParentGroup()->onGroupChanged(sigc::mem_fun(this, &LockedIndicator::onParameterGroupChanged));
  }
}

void LockedIndicator::onParameterGroupChanged()
{
  auto group = Application::get().getPresetManager()->getEditBuffer()->getSelectedParameter()->getParentGroup();
  setText(group->areAllParametersLocked() ? "L" : "");
}

void LockedIndicator::setFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::Colors::C255);
}

shared_ptr<Font> LockedIndicator::getFont() const
{
  return Oleds::get().getFont("Emphase_8_Regular", getFontHeight());
}

int LockedIndicator::getFontHeight() const
{
  return 8;
}
