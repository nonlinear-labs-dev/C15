#include "ControlRegistry.h"

namespace DescriptiveLayouts
{

  ControlRegistry::ControlRegistry()
  {
    m_discreteRegistry.emplace_back("ParameterEditButtonMenu");
    m_discreteRegistry.emplace_back("MacroControlEditButtonMenu");
    m_discreteRegistry.emplace_back("ModulationCarousel");
    m_discreteRegistry.emplace_back("ConvertSoundMenu");
    m_discreteRegistry.emplace_back("SingleSoundEditMenuLegacy");
    m_discreteRegistry.emplace_back("LayerSoundEditMenu");
    m_discreteRegistry.emplace_back("SplitSoundEditMenu");
    m_discreteRegistry.emplace_back("ParameterCarousel");
    m_discreteRegistry.emplace_back("LoadVoiceGroupPresetList");
  }

  void ControlRegistry::registerControl(ControlClass&& cp)
  {
    m_controlRegistry.insert(std::make_pair(cp.controlClass, std::move(cp)));
  }

  void ControlRegistry::clear()
  {
    m_controlRegistry.clear();
  }

  ControlRegistry& ControlRegistry::get()
  {
    static ControlRegistry cr;
    return cr;
  }

  const ControlClass& ControlRegistry::find(ControlClasses id) const
  {
    return m_controlRegistry.at(id);
  }

  const bool ControlRegistry::exists(ControlClasses id) const
  {
    auto genericIt = m_controlRegistry.find(id);
    auto discreteIt = std::find(m_discreteRegistry.begin(), m_discreteRegistry.end(), id);
    return genericIt != m_controlRegistry.end() | discreteIt != m_discreteRegistry.end();
  }
}
