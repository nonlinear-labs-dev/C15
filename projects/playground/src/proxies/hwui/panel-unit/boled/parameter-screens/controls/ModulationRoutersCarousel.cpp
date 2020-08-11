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
#include <groups/MacroControlMappingGroup.h>
#include <proxies/hwui/controls/LabelRegular8.h>

ModulationRoutersCarousel::ModulationRoutersCarousel(const Rect &pos)
    : super(pos)
{
}

ModulationRoutersCarousel::~ModulationRoutersCarousel() = default;

void ModulationRoutersCarousel::setup(Parameter *newOne)
{
  clear();

  const int ySpaceing = 0;
  const int sliderHeight = 17;
  const int paramWidth = 28;
  constexpr const auto startY = 10;

  int yPos = startY;
  addControl(new LabelRegular8("to", { 0, 0, 56, 10 }));

  int xPos = 0;

  int numItemsInRow = 0;

  if(auto p = dynamic_cast<PhysicalControlParameter *>(newOne))
  {
    auto group
        = Application::get().getPresetManager()->getEditBuffer()->getParameterGroupByID({ "MCM", VoiceGroup::Global });
    auto csGroup = dynamic_cast<MacroControlMappingGroup *>(group);
    auto routingParams = csGroup->getModulationRoutingParametersFor(p);

    for(auto routingParam : routingParams)
    {
      if(numItemsInRow == 2)
      {
        yPos += ySpaceing + sliderHeight;
        xPos = 0;
        numItemsInRow = 0;
      }

      auto miniParam = new MiniModulationRouter(routingParam, Rect(xPos, yPos, paramWidth, sliderHeight));
      addControl(miniParam);
      numItemsInRow++;

      xPos += paramWidth + 1;
    }
  }

  setDirty();
}

void ModulationRoutersCarousel::turn()
{
  if(auto p = dynamic_cast<PhysicalControlParameter *>(
         Application::get().getPresetManager()->getEditBuffer()->getSelected(getHWUI()->getCurrentVoiceGroup())))
    p->toggleUiSelectedModulationRouter(1);
}

void ModulationRoutersCarousel::antiTurn()
{
  if(auto p = dynamic_cast<PhysicalControlParameter *>(
         Application::get().getPresetManager()->getEditBuffer()->getSelected(getHWUI()->getCurrentVoiceGroup())))
    p->toggleUiSelectedModulationRouter(-1);
}

void ModulationRoutersCarousel::setHighlight(bool isHighlight)
{
  Control::setHighlight(isHighlight);
}
