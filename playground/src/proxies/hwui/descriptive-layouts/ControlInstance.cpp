#include <utility>

#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MacroControlEditButtonMenu.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationCarousel.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/ConvertSoundMenu.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/SingleSoundEditMenu.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ParameterCarousel.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/LayerSoundEditMenu.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/SplitSoundEditMenu.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetList.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Preset/LoadVoiceGroupPresetList.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Sound/Menus/SplitSoundEditMenu.h>
#include "ControlInstance.h"
#include "GenericControl.h"
#include "proxies/hwui/panel-unit/boled/parameter-screens/controls/ParameterEditButtonMenu.h"

namespace DescriptiveLayouts
{
  ControlInstance::ControlInstance(ControlInstances controlInstance, ControlClasses control, Point position,
                                   const EventConnections& eventConnections, StaticInitList staticInit,
                                   VisibilityEvent visibilitySource)
      : controlInstance(std::move(controlInstance))
      , controlClass(std::move(control))
      , position(position)
      , eventConnections(eventConnections)
      , staticInitList(std::move(staticInit))
      , visibility(visibilitySource)
  {
  }

  struct RuntimeUICache
  {
    ModulationCarousel::Mode m_lastModulationCarouselMode = ModulationCarousel::Mode::UpperBound;
  };

  Control* ControlInstance::instantiate() const
  {

    if(controlClass == "MacroControlEditButtonMenu")
    {
      return new MacroControlEditButtonMenu(Rect(position.getX(), position.getY(), 58, 62));
    }
    if(controlClass == "ModulationCarousel")
    {
      return new ModulationCarousel(Rect(position.getX(), position.getY(), 58, 72));
    }
    if(controlClass == "ParameterEditButtonMenu")
    {
      return new ParameterEditButtonMenu(Rect(position.getX(), position.getY(), 58, 62));
    }
    if(controlClass == "ConvertSoundMenu")
    {
      return new ConvertSoundMenu(Rect(position.getX(), position.getY(), 58, 26));
    }
    if(controlClass == "SingleSoundEditMenu")
    {
      return new SingleSoundEditMenu(Rect(position.getX(), position.getY(), 254, 52));
    }
    if(controlClass == "LayerSoundEditMenu")
    {
      return new LayerSoundEditMenu(Rect(position.getX(), position.getY(), 254, 52));
    }
    if(controlClass == "SplitSoundEditMenu")
    {
      return new SplitSoundEditMenu(Rect(position.getX(), position.getY(), 254, 52));
    }
    if(controlClass == "ParameterCarousel")
    {
      return new ParameterCarousel(Rect(position.getX(), position.getY(), 58, 62));
    }
    if(controlClass == "LoadVoiceGroupPresetList")
    {
      return new LoadVoiceGroupPresetList(position);
    }
    return new GenericControl(*this);
  }
}
