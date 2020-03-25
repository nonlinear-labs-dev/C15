#include <Application.h>
#include <parameters/Parameter.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/ModulateableParameter.h>
#include "ConditionRegistry.h"
#include "conditions/ParameterConditions.h"
#include "conditions/SoundConditions.h"
#include <device-settings/LayoutMode.h>
#include <proxies/hwui/descriptive-layouts/conditions/HWUIConditions.h>

ConditionRegistry::tCondition ConditionRegistry::getCondition(const std::string& key)
{
  return m_theConditionMap.at(key).get();
}

ConditionRegistry& ConditionRegistry::get()
{
  static ConditionRegistry theRegistry;
  return theRegistry;
}

ConditionRegistry::ConditionRegistry()
{
  using namespace DescriptiveLayouts;
  m_theConditionMap["isParameterModulateable"] = std::make_unique<ParameterConditions::IsParameterModulateable>();
  m_theConditionMap["isParameterUnmodulateable"] = std::make_unique<ParameterConditions::IsParameterUnmodulateable>();
  m_theConditionMap["hasNoMcSelected"] = std::make_unique<ParameterConditions::HasNoMcSelected>();
  m_theConditionMap["hasMcSelected"] = std::make_unique<ParameterConditions::HasMcSelected>();
  m_theConditionMap["isSingleSound"] = std::make_unique<SoundConditions::IsSingleSound>();
  m_theConditionMap["isSplitSound"] = std::make_unique<SoundConditions::IsSplitSound>();
  m_theConditionMap["isLayerSound"] = std::make_unique<SoundConditions::IsLayerSound>();
  m_theConditionMap["isFineActive"] = std::make_unique<HWUIConditions::IsFineActive>();

  for(auto& c : m_theConditionMap)
    c.second->connect([this] { this->onConditionChanged(); });
}

sigc::connection ConditionRegistry::onChange(const std::function<void()>& cb)
{
  return m_signal.connect(cb);
}

void ConditionRegistry::onConditionChanged()
{
  if(Application::get().getSettings()->getSetting<LayoutMode>()->get() != LayoutVersionMode::Old)
    m_signal.deferedSend();
}
