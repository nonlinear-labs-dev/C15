#include "ModulationModeButton.h"
#include "Application.h"
#include "parameters/ModulateableParameter.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"

ModulationModeButton::ModulationModeButton(const Glib::ustring &caption, Buttons id)
    : super(caption, id)
{
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::mem_fun(this, &ModulationModeButton::onParameterSelectionChanged));
}

ModulationModeButton::~ModulationModeButton()
{
}

void ModulationModeButton::onParameterSelectionChanged(Parameter *oldParameter, Parameter *newParameter)
{
  if(newParameter)
  {
    m_paramValueConnection.disconnect();
    m_paramValueConnection = newParameter->onParameterChanged(sigc::mem_fun(this, &ModulationModeButton::update));
  }
}
