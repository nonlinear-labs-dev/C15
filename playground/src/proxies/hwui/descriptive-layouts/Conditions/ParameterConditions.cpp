#include "ParameterConditions.h"

using namespace DescriptiveLayouts;

namespace conditiondetail {
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
    return getEditBuffer().getSelected();
  }

  const ModulateableParameter *getModulateableParameter()
  {
    return dynamic_cast<const ModulateableParameter *>(getCurrentParameter());
  }

  void bruteForceLayouts()
  {
    auto hwui = Application::get().getHWUI();
    hwui->getPanelUnit().getEditPanel().getBoled().bruteForce();
  }

}

void ParameterConditions::IsParameterModulateable::onConditionChanged() const
{
  conditiondetail::bruteForceLayouts();
}

bool ParameterConditions::IsParameterModulateable::check() const
{
  return conditiondetail::getModulateableParameter() != nullptr;
}

ParameterConditions::IsParameterModulateable::IsParameterModulateable()
    : m_selChanged{ this }
{
}

void ParameterConditions::IsParameterModulateable::onParameterSelectionChanged(const Parameter *o, Parameter *n)
{
  get();
}

bool ParameterConditions::IsParameterUnmodulateable::check() const
{
  return !IsParameterModulateable::check();
}

ParameterConditions::HasNoMcSelected::HasNoMcSelected()
    : m_paramSig{ this }
{
}

void ParameterConditions::HasNoMcSelected::onConditionChanged() const
{
  conditiondetail::bruteForceLayouts();
}

bool ParameterConditions::HasNoMcSelected::check() const
{
  if(auto parameter = conditiondetail::getModulateableParameter())
  {
    return parameter->getModulationSource() == ModulationSource::NONE;
  }
  return true;
}

void ParameterConditions::HasNoMcSelected::onParameterChanged(const Parameter *parameter)
{
  get();
}

bool ParameterConditions::HasMcSelected::check() const
{
  return !HasNoMcSelected::check();
}
