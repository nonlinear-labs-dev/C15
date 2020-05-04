#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterLayout.h>
#include "ParameterNotAvailableInSoundInfo.h"
#include <proxies/hwui/FrameBuffer.h>
#include <proxies/hwui/HWUI.h>

ParameterNotAvailableInSoundInfo::ParameterNotAvailableInSoundInfo(const Rect &r, const Glib::ustring &text)
    : MultiLineLabel(r, text)
{
  m_parameterSelectionConnection = Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::mem_fun(this, &ParameterNotAvailableInSoundInfo::onSelectionChanged));

  m_sountTypeConnection = Application::get().get().getPresetManager()->getEditBuffer()->onSoundTypeChanged(
      sigc::mem_fun(this, &ParameterNotAvailableInSoundInfo::onSoundTypeChanged));

  setVisible(false);
}

void ParameterNotAvailableInSoundInfo::setBackgroundColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBufferColors::C43);
}

void ParameterNotAvailableInSoundInfo::onSelectionChanged(const Parameter *old, const Parameter *newParam)
{
  auto vis = !ParameterLayout2::isParameterAvailableInSoundType(newParam);
  setVisible(vis);
}

void ParameterNotAvailableInSoundInfo::onSoundTypeChanged()
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  auto current = eb->getSelected();
  const auto vis = !ParameterLayout2::isParameterAvailableInSoundType(current, eb);
  setVisible(vis);
}