#include "ControlRegistry.h"
#include "GenericControl.h"

#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ParameterEditButtonMenu.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MacroControlEditButtonMenu.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationCarousel.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/ConvertSoundMenu.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/SingleSoundEditMenuLegacy.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ParameterCarousel.h>

#include <proxies/hwui/descriptive-layouts/concrete/preset/LoadVoiceGroupPresetList.h>
#include <proxies/hwui/descriptive-layouts/concrete/sound/menus/DualSoundEditMenu.h>
#include <proxies/hwui/descriptive-layouts/concrete/sound/menus/SingleSoundEditMenu.h>
#include <proxies/hwui/descriptive-layouts/concrete/sound/MonoGroupControl.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/CurrentVoiceGroupSelectionIndicator.h>
#include <proxies/hwui/controls/FadeIndicator.h>

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
    m_discreteRegistry["ModulationCarousel"] = [](const Point& position) {
      return new ModulationCarousel(ModulationCarousel::Mode::UpperBound,
                                    Rect(position.getX(), position.getY(), 58, 72));
    };

    m_discreteRegistry["ConvertSoundMenu"]
        = [](const Point& position) { return new ConvertSoundMenu(Rect(position.getX(), position.getY(), 58, 26)); };

    m_discreteRegistry["SingleSoundEditMenu"] = [](const Point& position) {
      return new SingleSoundEditMenu(Rect(position.getX(), position.getY(), 256, 52));
    };
    m_discreteRegistry["DualSoundEditMenu"]
        = [](const Point& position) { return new DualSoundEditMenu(Rect(position.getX(), position.getY(), 256, 52)); };

    m_discreteRegistry["ParameterCarousel"]
        = [](const Point& position) { return new ParameterCarousel(Rect(position.getX(), position.getY(), 58, 62)); };

    m_discreteRegistry["LoadVoiceGroupPresetList"]
        = [](const Point& position) { return new LoadVoiceGroupPresetList(position); };
    m_discreteRegistry["MonoGroupControl"]
        = [](const Point& position) { return new MonoGroupControl(Rect(position.getX(), position.getY(), 125, 33)); };

    m_discreteRegistry["CurrentVoiceGroupSelectionIndicator"] = [](const Point& position) {
      return new CurrentVoiceGroupSelectionIndicator(Rect(position.getX(), position.getY(), 10, 8));
    };

    m_discreteRegistry["FadeIndicatorI"]
        = [](const Point& pos) { return new FadeIndicator(Rect(pos.getX(), pos.getY(), 14, 7), VoiceGroup::I); };

    m_discreteRegistry["FadeIndicatorII"]
        = [](const Point& pos) { return new FadeIndicator(Rect(pos.getX(), pos.getY(), 14, 7), VoiceGroup::II); };
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
    auto it = m_discreteRegistry.find(cc);

    if(it != m_discreteRegistry.end())
      return it->second(position);

    return nullptr;
  }
}
