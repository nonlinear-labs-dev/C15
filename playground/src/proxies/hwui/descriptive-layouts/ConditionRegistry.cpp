#include <Application.h>
#include <parameters/Parameter.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/ModulateableParameter.h>
#include "ConditionRegistry.h"

ConditionRegistry::tCondition ConditionRegistry::getLambda(std::string key) {
  return m_theConditonMap.at(key);
}

ConditionRegistry& ConditionRegistry::get() {
  static ConditionRegistry theRegistry;
  return theRegistry;
}

std::shared_ptr<PresetManager> getPresetManager() {
  return Application::get().getPresetManager();
}

Parameter* getSelectedParam() {
  return getPresetManager()->getEditBuffer()->getSelected();
}

ConditionRegistry::ConditionRegistry() {
  m_theConditonMap["isParameterModulateable"] = []() {
    auto modParam = dynamic_cast<ModulateableParameter*>(getSelectedParam());
    return modParam != nullptr;
  };

  m_theConditonMap["isParameterUnmodulateable"] = [&]() {
    return !m_theConditonMap["isParameterModulateable"];
  };

  m_theConditonMap["hasNoMcSelected"] = []() {
    auto modParam = dynamic_cast<ModulateableParameter*>(getSelectedParam());
    if(modParam != nullptr) {
      return ModulateableParameter::ModulationSource::NONE == modParam->getModulationSource();
    }
    return false;
  };
}