#include "SelectedMacroControlsHWSourceAmount.h"
#include <proxies/hwui/HWUI.h>
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "parameters/MacroControlParameter.h"
#include "parameters/ModulationRoutingParameter.h"
#include "parameters/PhysicalControlParameter.h"
#include "groups/HardwareSourcesGroup.h"
#include <groups/MacroControlMappingGroup.h>

SelectedMacroControlsHWSourceAmount::SelectedMacroControlsHWSourceAmount(const Rect &rect)
    : super(rect)
{
  getEditBuffer()->onSelectionChanged(
      sigc::hide<0>(sigc::mem_fun(this, &SelectedMacroControlsHWSourceAmount::onParameterSelected)));
  Application::get().getHWUI()->onModifiersChanged(
      sigc::hide(sigc::mem_fun(this, &SelectedMacroControlsHWSourceAmount::onModifiersChanged)));
}

SelectedMacroControlsHWSourceAmount::~SelectedMacroControlsHWSourceAmount()
{
}

void SelectedMacroControlsHWSourceAmount::onParameterSelected(Parameter *newOne)
{
  m_mcChanged.disconnect();
  m_mcChanged = newOne->onParameterChanged(sigc::mem_fun(this, &SelectedMacroControlsHWSourceAmount::onMCChanged));
}

shared_ptr<EditBuffer> SelectedMacroControlsHWSourceAmount::getEditBuffer()
{
  return Application::get().getPresetManager()->getEditBuffer();
}

void SelectedMacroControlsHWSourceAmount::onMCChanged(const Parameter *param)
{
  if(auto mc = dynamic_cast<const MacroControlParameter *>(param))
  {
    int hwSourceID = mc->getUiSelectedHardwareSource();

    m_hwParamID = hwSourceID;

    if(hwSourceID > 0)
    {
      if(auto hwParam = getEditBuffer()->findParameterByID(hwSourceID))
      {
        m_srcChanged.disconnect();
        m_srcChanged = hwParam->onParameterChanged(
            sigc::mem_fun(this, &SelectedMacroControlsHWSourceAmount::updateTextFromRouter));

        auto csGroup = static_cast<MacroControlMappingGroup *>(getEditBuffer()->getParameterGroupByID("MCM"));
        auto routers = csGroup->getModulationRoutingParametersFor(dynamic_cast<const MacroControlParameter *>(param));

        for(auto router : routers)
        {
          if(router->routes(dynamic_cast<const PhysicalControlParameter *>(hwParam)))
          {
            m_router = router;
            m_amountChanged.disconnect();
            m_amountChanged
                = router->onParameterChanged(sigc::mem_fun(this, &SelectedMacroControlsHWSourceAmount::updateText));
            break;
          }
        }
      }
    }
  }
}

void SelectedMacroControlsHWSourceAmount::updateText(const Parameter *param)
{
  auto str = param->getDisplayString();

  if(isHighlight() && Application::get().getHWUI()->isModifierSet(ButtonModifier::FINE) && str != "On" && str != "Off")
  {
    setText(str + " F", 2);
  }
  else
  {
    setText(str);
  }
}

void SelectedMacroControlsHWSourceAmount::setSuffixFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::Colors::C103);
}

void SelectedMacroControlsHWSourceAmount::updateTextFromRouter(const Parameter *param)
{
  if(m_router)
    updateText(m_router);
}

void SelectedMacroControlsHWSourceAmount::onModifiersChanged()
{
  onParameterSelected(Application::get().getPresetManager()->getEditBuffer()->getSelectedParameter());
}
