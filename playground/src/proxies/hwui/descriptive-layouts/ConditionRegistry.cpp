#include <Application.h>
#include <parameters/Parameter.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/ModulateableParameter.h>
#include "ConditionRegistry.h"
#include "Conditions/ParameterConditions.h"

ConditionRegistry::tCondition ConditionRegistry::getLambda(std::string key)
{
  try {
    return m_theConditionMap.at(key).get();
  } catch(...) {
    DebugLevel::warning("Could not find condition:", key, "in condition map.");
    return nullptr;
  }
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
  using namespace DescriptiveLayouts;
  m_theConditionMap["isParameterModulateable"] = std::make_unique<ParameterConditions::IsParameterModulateable>();
  m_theConditionMap["isParameterUnmodulateable"] = std::make_unique<ParameterConditions::IsParameterUnmodulateable>();
  m_theConditionMap["hasNoMcSelected"] = std::make_unique<ParameterConditions::HasNoMcSelected>();
  m_theConditionMap["hasMcSelected"] = std::make_unique<ParameterConditions::HasMcSelected>();
}
