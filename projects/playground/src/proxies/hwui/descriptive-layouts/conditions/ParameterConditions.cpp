#include <proxies/hwui/descriptive-layouts/ConditionRegistry.h>
#include <parameters/ModulateableParameter.h>
#include "ParameterConditions.h"
#include <proxies/hwui/HWUI.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>

using namespace DescriptiveLayouts;

#warning "IMPROVEMENT: maybe add inversion to condition rules, so we do not need IsModulateable AND IsNotModulateable"

namespace conditiondetail
{
  const EditBuffer &getEditBuffer()
  {
    return *Application::get().getPresetManager()->getEditBuffer();
  }

  const HWUI *getHWUI()
  {
    return Application::get().getHWUI();
  }

  const Parameter *getCurrentParameter()
  {
    return getEditBuffer().getSelected(Application::get().getHWUI()->getCurrentVoiceGroup());
  }

  const ModulateableParameter *getModulateableParameter()
  {
    return dynamic_cast<const ModulateableParameter *>(getCurrentParameter());
  }
}

bool ParameterConditions::IsParameterModulateable::check() const
{
  return conditiondetail::getModulateableParameter() != nullptr;
}

bool ParameterConditions::IsParameterUnmodulateable::check() const
{
  return !IsParameterModulateable::check();
}

bool ParameterConditions::HasNoMcSelected::check() const
{
  if(auto parameter = conditiondetail::getModulateableParameter())
  {
    return parameter->getModulationSource() == MacroControls::NONE;
  }
  return true;
}

bool ParameterConditions::HasMcSelected::check() const
{
  return !HasNoMcSelected::check();
}

ParameterConditions::ParameterCondition::ParameterCondition()
{
  m_paramChangedConnection = Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::mem_fun(this, &ParameterCondition::onParameterSelectionChanged),
      Application::get().getHWUI()->getCurrentVoiceGroup());
}

ParameterConditions::ParameterCondition::~ParameterCondition()
{
  m_paramChangedConnection.disconnect();
  m_paramConnection.disconnect();
}

void ParameterConditions::ParameterCondition::onParameterSelectionChanged(const Parameter *oldParam,
                                                                          Parameter *newParam)
{
  m_paramConnection.disconnect();

  if(newParam)
  {
    m_paramConnection = newParam->onParameterChanged(sigc::mem_fun(this, &ParameterCondition::onParameterChanged));
  }
}

void ParameterConditions::ParameterCondition::onParameterChanged(const Parameter *param)
{
  update();
}
