#include "ModulationRoutersCarousel.h"
#include "Application.h"
#include "proxies/hwui/HWUI.h"
#include "parameters/Parameter.h"
#include "MiniParameter.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "MiniModulationRouter.h"

#include "parameters/MacroControlParameter.h"
#include "parameters/ModulationRoutingParameter.h"
#include "parameters/PhysicalControlParameter.h"
#include "groups/HardwareSourcesGroup.h"
#include <groups/MacroControlMappingGroup.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MiniParameterBarSlider.h>

ModulationRoutersCarousel::ModulationRoutersCarousel(const Rect &pos)
    : super(pos)
{
}

ModulationRoutersCarousel::~ModulationRoutersCarousel()
{
}

void ModulationRoutersCarousel::setup(Parameter *newOne)
{
  clear();

  const int ySpaceing = 0;
  const int sliderHeight = 16;
  const int paramWidth = 56;

  int yPos = ySpaceing;

  if(auto p = dynamic_cast<PhysicalControlParameter *>(newOne))
  {
    auto group = Application::get().getPresetManager()->getEditBuffer()->getParameterGroupByID("MCM");
    auto csGroup = dynamic_cast<MacroControlMappingGroup *>(group);
    auto routingParams = csGroup->getModulationRoutingParametersFor(p);

    for(auto routingParam : routingParams)
    {
      auto miniParam = new MiniModulationRouter(routingParam, Rect(0, yPos, paramWidth, sliderHeight));
      addControl(miniParam);

      yPos += sliderHeight;
      yPos += ySpaceing;
    }
  }

  setDirty();
}

void ModulationRoutersCarousel::turn()
{
  if(auto p = dynamic_cast<PhysicalControlParameter *>(
         Application::get().getPresetManager()->getEditBuffer()->getSelectedParameter()))
    p->toggleUiSelectedModulationRouter(1);
}

void ModulationRoutersCarousel::antiTurn()
{
  if(auto p = dynamic_cast<PhysicalControlParameter *>(
         Application::get().getPresetManager()->getEditBuffer()->getSelectedParameter()))
    p->toggleUiSelectedModulationRouter(-1);
}
