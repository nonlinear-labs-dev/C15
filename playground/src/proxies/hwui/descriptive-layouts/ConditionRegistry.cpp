#include <Application.h>
#include <parameters/Parameter.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/ModulateableParameter.h>
#include "ConditionRegistry.h"

ConditionRegistry::tCondition ConditionRegistry::getLambda(std::string key)
{
  return m_theConditonMap.at(key);
}

ConditionRegistry& ConditionRegistry::get()
{
  static ConditionRegistry theRegistry;
  return theRegistry;
}

PresetManager* getPresetManager()
{
  return Application::get().getPresetManager();
}

Parameter* getSelectedParam()
{
  return getPresetManager()->getEditBuffer()->getSelected();
}

ConditionRegistry::ConditionRegistry()
{
  m_theConditonMap["isParameterModulateable"] = []() {
    auto modParam = dynamic_cast<ModulateableParameter*>(getSelectedParam());
    return modParam != nullptr;
  };

  m_theConditonMap["isParameterUnmodulateable"] = [&]() { return !m_theConditonMap["isParameterModulateable"]; };

  m_theConditonMap["hasNoMcSelected"] = []() {
    auto modParam = dynamic_cast<ModulateableParameter*>(getSelectedParam());
    if(modParam != nullptr)
    {
      return ModulationSource::NONE == modParam->getModulationSource();
    }
    return false;
  };

  m_theConditonMap["hasMcSelected"] = [&]() { return !m_theConditonMap["hasNoMcSelected"]; };

  m_theConditonMap["isSingleSound"] = [&]() { return true; };

  m_theConditonMap["isDualSound"] = [&]() { return false; };
}
