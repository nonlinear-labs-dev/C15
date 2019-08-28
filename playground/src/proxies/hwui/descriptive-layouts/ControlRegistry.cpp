#include "ControlRegistry.h"
#include "GenericControl.h"

#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ParameterEditButtonMenu.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MacroControlEditButtonMenu.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationCarousel.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/ConvertSoundMenu.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/SingleSoundEditMenuLegacy.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ParameterCarousel.h>

#include <proxies/hwui/descriptive-layouts/Concrete/Preset/LoadVoiceGroupPresetList.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Sound/Menus/SplitSoundEditMenu.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Sound/Menus/SingleSoundEditMenu.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Sound/Menus/LayerSoundEditMenu.h>

namespace DescriptiveLayouts
{

  ControlRegistry::ControlRegistry()
  {

    m_discreteRegistry["ParameterEditButtonMenu"] = [](const Point& position) {
      return new ParameterEditButtonMenu(Rect(position.getX(), position.getY(), 58, 62));
    };
    m_discreteRegistry["MacroControlEditButtonMenu"] = [](const Point& position) {
      return new MacroControlEditButtonMenu(Rect(position.getX(), position.getY(), 58, 62));
    };
    m_discreteRegistry["ModulationCarousel"]
        = [](const Point& position) { return new ModulationCarousel(Rect(position.getX(), position.getY(), 58, 72)); };

    m_discreteRegistry["ConvertSoundMenu"]
        = [](const Point& position) { return new ConvertSoundMenu(Rect(position.getX(), position.getY(), 58, 26)); };

    m_discreteRegistry["SingleSoundEditMenu"] = [](const Point& position) {
      return new SingleSoundEditMenu(Rect(position.getX(), position.getY(), 254, 52));
    };
    m_discreteRegistry["LayerSoundEditMenu"]
        = [](const Point& position) { return new LayerSoundEditMenu(Rect(position.getX(), position.getY(), 254, 52)); };
    m_discreteRegistry["SplitSoundEditMenu"]
        = [](const Point& position) { return new SplitSoundEditMenu(Rect(position.getX(), position.getY(), 254, 52)); };

    m_discreteRegistry["ParameterCarousel"]
        = [](const Point& position) { return new ParameterCarousel(Rect(position.getX(), position.getY(), 58, 62)); };

    m_discreteRegistry["LoadVoiceGroupPresetList"]
        = [](const Point& position) { return new LoadVoiceGroupPresetList(position); };
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

  const ControlClass& ControlRegistry::find(const ControlClasses& id) const
  {
    return m_controlRegistry.at(id);
  }

  bool ControlRegistry::exists(const ControlClasses& id) const
  {
    auto genericIt = m_controlRegistry.find(id);
    auto discreteIt = m_discreteRegistry.find(id);
    return genericIt != m_controlRegistry.end() | discreteIt != m_discreteRegistry.end();
  }

  Control* ControlRegistry::instantiateConcrete(const ControlClasses& cc, const Point& position)
  {
    try
    {
      return m_discreteRegistry.at(cc)(position);
    }
    catch(...)
    {
      return nullptr;
    }
  }
}
