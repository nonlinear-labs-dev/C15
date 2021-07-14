#include "LockedIndicator.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "parameters/Parameter.h"
#include <groups/ParameterGroup.h>
#include "proxies/hwui/panel-unit/boled/BOLED.h"
#include <sigc++/sigc++.h>
#include <proxies/hwui/Oleds.h>
#include <proxies/hwui/FrameBuffer.h>
#include <proxies/hwui/HWUI.h>

LockedIndicator::LockedIndicator(const Rect &pos)
    : super(pos)
{
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::hide<0>(sigc::mem_fun(this, &LockedIndicator::onParameterSelected)), getHWUI()->getCurrentVoiceGroup());
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
  auto group = Application::get()
                   .getPresetManager()
                   ->getEditBuffer()
                   ->getSelected(getHWUI()->getCurrentVoiceGroup())
                   ->getParentGroup();
  setText(StringAndSuffix { group->areAllParametersLocked() ? "\uE30E" : "" });
}

void LockedIndicator::setFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBufferColors::C255);
}

std::shared_ptr<Font> LockedIndicator::getFont() const
{
  return Oleds::get().getFont("Emphase-8-Regular", getFontHeight());
}

int LockedIndicator::getFontHeight() const
{
  return 8;
}
