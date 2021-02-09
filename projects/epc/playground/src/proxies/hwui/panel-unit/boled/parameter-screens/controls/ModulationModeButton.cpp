#include <sigc++/adaptors/hide.h>
#include "ModulationModeButton.h"
#include "Application.h"
#include "parameters/ModulateableParameter.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include <proxies/hwui/HWUI.h>

ModulationModeButton::ModulationModeButton(const Glib::ustring &caption, Buttons id)
    : super(caption, id)
{
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::hide<2>(sigc::mem_fun(this, &ModulationModeButton::onParameterSelectionChanged)),
      getHWUI()->getCurrentVoiceGroup());

  Application::get().getPresetManager()->getEditBuffer()->onSoundTypeChanged(
      sigc::hide(sigc::mem_fun(this, &ModulationModeButton::onSoundTypeChanged)), false);
}

ModulationModeButton::~ModulationModeButton()
{
}

void ModulationModeButton::onParameterSelectionChanged(Parameter *oldParameter, Parameter *newParameter)
{
  if(newParameter)
  {
    m_param = newParameter;
    m_paramValueConnection.disconnect();
    m_paramValueConnection = newParameter->onParameterChanged(sigc::mem_fun(this, &ModulationModeButton::update));
  }
}

void ModulationModeButton::onSoundTypeChanged()
{
  update(m_param);
}
