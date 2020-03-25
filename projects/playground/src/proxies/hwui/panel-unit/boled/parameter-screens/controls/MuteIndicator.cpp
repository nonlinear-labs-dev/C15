#include "MuteIndicator.h"

#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/Parameter.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/Oleds.h>

MuteIndicator::MuteIndicator(const Rect& r)
    : Label(r)
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
  if(m_parameterConnection)
    m_parameterConnection.disconnect();
}

void MuteIndicator::onParameterChanged(const Parameter* p)
{
  const auto muteActive = p->getControlPositionValue() != 0;
  const auto currentFocusIsNotGlobal
      = Application::get().getPresetManager()->getEditBuffer()->getSelected()->getID().getVoiceGroup()
      != VoiceGroup::Global;
  
  if(muteActive && currentFocusIsNotGlobal)
    setText({ "\uE0BA", 0 });
  else
    setText({ "", 0 });
}

std::shared_ptr<Font> MuteIndicator::getFont() const
{
  return Oleds::get().getFont("Emphase-8-Regular", getFontHeight());
}

int MuteIndicator::getFontHeight() const
{
  return 8;
}
