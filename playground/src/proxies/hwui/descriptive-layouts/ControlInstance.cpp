#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MacroControlEditButtonMenu.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationCarousel.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/ConvertSoundMenu.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/SingleSoundEditMenu.h>
#include "ControlInstance.h"
#include "GenericControl.h"
#include "proxies/hwui/panel-unit/boled/parameter-screens/controls/ParameterEditButtonMenu.h"

namespace DescriptiveLayouts
{
  ControlInstance::ControlInstance(ControlInstances controlInstance, ControlClasses control, Point position,
                                   const EventConnections& eventConnections, StaticInitList staticInit)
      : controlInstance(controlInstance)
      , controlClass(control)
      , position(position)
      , eventConnections(eventConnections)
      , staticInitList(staticInit)
  {
  }

  Control* ControlInstance::instantiate() const
  {
    if(controlClass == "MacroControlEditButtonMenu")
    {
      return new MacroControlEditButtonMenu(Rect(position.getX(), position.getY(), 58, 62));
    }
    if(controlClass == "ModulationCarousel")
    {
      return new ModulationCarousel(ModulationCarousel::Mode::None, Rect(position.getX(), position.getY(), 58, 62));
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
        return new SingleSoundEditMenu(Rect(position.getX(), position.getY(), 256, 64));
    }

    return new GenericControl(*this);
  }
}
