#include "HWSourceAmountCarousel.h"
#include "Application.h"
#include "proxies/hwui/HWUI.h"
#include "parameters/Parameter.h"
#include "MiniParameter.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "parameters/MacroControlParameter.h"
#include "parameters/ModulationRoutingParameter.h"
#include "parameters/PhysicalControlParameter.h"
#include "groups/HardwareSourcesGroup.h"
#include <groups/MacroControlMappingGroup.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MiniParameterBarSlider.h>

HWSourceAmountCarousel::HWSourceAmountCarousel(const Rect &pos)
    : super(pos)
{
}

HWSourceAmountCarousel::~HWSourceAmountCarousel()
{
}

void HWSourceAmountCarousel::turn()
{
  if(auto p
     = dynamic_cast<MacroControlParameter *>(Application::get().getPresetManager()->getEditBuffer()->getSelected()))
    p->toggleUiSelectedHardwareSource(1);
}

void HWSourceAmountCarousel::antiTurn()
{
  if(auto p
     = dynamic_cast<MacroControlParameter *>(Application::get().getPresetManager()->getEditBuffer()->getSelected()))
    p->toggleUiSelectedHardwareSource(-1);
}

void HWSourceAmountCarousel::setup(Parameter *newOne)
{
  clear();

  const int ySpaceing = 5;
  const int sliderHeight = 2;
  const int yGroupSpacing = 8;
  const int paramWidth = 56;

  int yPos = ySpaceing;

  if(auto p = dynamic_cast<MacroControlParameter *>(newOne))
  {
    if(p->getUiSelectedHardwareSource() == 0)
      p->toggleUiSelectedHardwareSource(1);

    auto group = Application::get().getPresetManager()->getEditBuffer()->getParameterGroupByID("MCM");
    auto csGroup = dynamic_cast<MacroControlMappingGroup *>(group);
    auto routingParams = csGroup->getModulationRoutingParametersFor(p);

    for(auto routingParam : routingParams)
    {
      auto miniParam
          = new MiniParameterBarSlider(static_cast<Parameter *>(routingParam), Rect(0, yPos, paramWidth, sliderHeight));
      addControl(miniParam);

      yPos += sliderHeight;

      if(getNumChildren() == 4)
        yPos += yGroupSpacing;
      else
        yPos += ySpaceing;
    }

    m_mcConnection.disconnect();
    m_mcConnection = p->onParameterChanged(sigc::mem_fun(this, &HWSourceAmountCarousel::onMacroControlChanged));
  }

  setDirty();
}

void HWSourceAmountCarousel::highlightSelected()
{
  setup(Application::get().getPresetManager()->getEditBuffer()->getSelected());
}

void HWSourceAmountCarousel::onMacroControlChanged(const Parameter *param)
{
  if(auto mc = dynamic_cast<const MacroControlParameter *>(param))
  {
    forEach([=](tControlPtr c) {
      if(auto miniSlider = std::dynamic_pointer_cast<MiniParameterBarSlider>(c))
        if(auto p = dynamic_cast<ModulationRoutingParameter *>(miniSlider->getParameter()))
          miniSlider->setHighlight(isHighlight()
                                   || (mc->getUiSelectedHardwareSource() == p->getSourceParameter()->getID()));
    });
  }
}

void HWSourceAmountCarousel::setHighlight(bool isHighlight)
{
  super::setHighlight(isHighlight);
}
