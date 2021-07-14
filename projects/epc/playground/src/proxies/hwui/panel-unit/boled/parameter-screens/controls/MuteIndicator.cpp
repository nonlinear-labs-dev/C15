#include "MuteIndicator.h"

#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/Parameter.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/Oleds.h>
#include <proxies/hwui/controls/SwitchVoiceGroupButton.h>
#include <sigc++/adaptors/hide.h>

MuteIndicator::MuteIndicator(const Rect& r)
    : Label(r)
{
  m_soundTypeConnection = Application::get().getPresetManager()->getEditBuffer()->onSoundTypeChanged(
      sigc::hide(sigc::mem_fun(this, &MuteIndicator::setup)));

  m_vgConnection = Application::get().getHWUI()->onCurrentVoiceGroupChanged(
      sigc::hide(sigc::mem_fun(this, &MuteIndicator::setup)));
}

void MuteIndicator::setup()
{
  auto isInLayerMode = Application::get().getPresetManager()->getEditBuffer()->getType() == SoundType::Layer;

  if(isInLayerMode)
  {
    auto currentVG = Application::get().getHWUI()->getCurrentVoiceGroup();
    m_parameterConnection = Application::get()
                                .getPresetManager()
                                ->getEditBuffer()
                                ->findParameterByID({ 395, currentVG })
                                ->onParameterChanged(sigc::mem_fun(this, &MuteIndicator::onParameterChanged));
  }
}

MuteIndicator::~MuteIndicator()
{
  m_soundTypeConnection.disconnect();
  m_vgConnection.disconnect();
  m_parameterConnection.disconnect();
}

void MuteIndicator::onParameterChanged(const Parameter* p)
{
  const auto muteActive = p->getControlPositionValue() != 0;
  const auto currentFocusIsNotGlobal
      = SwitchVoiceGroupButton::allowToggling(p, Application::get().getPresetManager()->getEditBuffer());

  if(muteActive && currentFocusIsNotGlobal)
    setText(StringAndSuffix { "\uE0BA", 0 });
  else
    setText(StringAndSuffix::empty());
}

std::shared_ptr<Font> MuteIndicator::getFont() const
{
  return Oleds::get().getFont("Emphase-8-Regular", getFontHeight());
}

int MuteIndicator::getFontHeight() const
{
  return 8;
}
