#include <proxies/hwui/panel-unit/PanelUnitParameterEditMode.h>
#include <groups/ScaleGroup.h>
#include <parameters/ScaleParameter.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterLayout.h>
#include "ParameterCarousel.h"
#include "Application.h"
#include "proxies/hwui/HWUI.h"
#include "parameters/Parameter.h"
#include "MiniParameter.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "proxies/hwui/controls/Button.h"
#include "NeverHighlitButton.h"

ParameterCarousel::ParameterCarousel(const Rect& pos)
    : super(pos)
{
}

ParameterCarousel::~ParameterCarousel()
{
}

void ParameterCarousel::setup(Parameter* selectedParameter)
{
  clear();

  auto um = Application::get().getHWUI()->getPanelUnit().getUsageMode();
  const auto paramAvailable = !selectedParameter->isDisabled();

  if(std::shared_ptr<PanelUnitParameterEditMode> edit = std::dynamic_pointer_cast<PanelUnitParameterEditMode>(um))
  {
    if(selectedParameter && paramAvailable)
    {
      auto button = edit->findButtonForParameter(selectedParameter);

      if(button != Buttons::INVALID && static_cast<int>(button) != -1)
      {
        setupChildControls(edit, selectedParameter, button);
      }
      else
      {
        setupChildControlsForParameterWithoutButtonMapping(selectedParameter);
      }
    }
  }

  if(getNumChildren() == 0)
  {
    addControl(new NeverHighlitButton("", Rect(0, 51, 58, 11)));
  }
  else
  {
    setHighlight(true);
  }

  setDirty();
}

void ParameterCarousel::setupChildControls(const std::shared_ptr<PanelUnitParameterEditMode>& edit,
                                           Parameter* selectedParameter, Buttons button)
{
  std::list<int> buttonAssignments
      = edit->getButtonAssignments(button, Application::get().getPresetManager()->getEditBuffer()->getType());

  if(buttonAssignments.size() > 1)
  {
    setupChildControls(selectedParameter, buttonAssignments);
  }
}

void ParameterCarousel::setupChildControls(Parameter* selectedParameter, const std::list<int>& buttonAssignments)
{
  const int ySpaceing = 3;
  const int miniParamHeight = 12;
  const int miniParamWidth = 56;
  auto yPos = ySpaceing;
  auto maxNumParameters = 4;
  auto missingParams = maxNumParameters - buttonAssignments.size();
  yPos += missingParams * (miniParamHeight + ySpaceing);

  for(int i : buttonAssignments)
  {
    auto vg = Application::get().getHWUI()->getCurrentVoiceGroup();
    auto eb = Application::get().getPresetManager()->getEditBuffer();

    auto param = eb->findParameterByID({ i, vg });

    if(!param)
      param = eb->findParameterByID({ i, VoiceGroup::Global });

    if(!param)
      continue;

    auto miniParam = new MiniParameter(param, Rect(0, yPos, miniParamWidth, miniParamHeight));

    if(dynamic_cast<ScaleParameter*>(selectedParameter) != nullptr)
    {
      miniParam->setSelected(param->getID().getNumber() == ScaleGroup::getScaleBaseParameterNumber());
    }
    else
    {
      miniParam->setSelected(param->getID().getNumber() == selectedParameter->getID().getNumber());
    }

    addControl(miniParam);
    yPos += ySpaceing;
    yPos += miniParamHeight;
  }
}

void ParameterCarousel::antiTurn()
{
  auto foundCtrl = std::dynamic_pointer_cast<MiniParameter>(*getControls().rbegin());
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  EditBufferUseCases ebUseCases { eb };
  for(const auto& ctrl : getControls())
  {
    if(auto p = std::dynamic_pointer_cast<MiniParameter>(ctrl))
    {
      if(p->isSelected())
      {
        ebUseCases.selectParameter(foundCtrl->getParameter()->getID(), true);
        return;
      }
      foundCtrl = p;
    }
  }
}

void ParameterCarousel::turn()
{
  bool found = false;
  bool handled = false;
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  EditBufferUseCases ebUseCases { eb };

  tIfCallback cb = ([&](const tControlPtr& ctrl) -> bool {
    if(auto p = std::dynamic_pointer_cast<MiniParameter>(ctrl))
    {
      if(found)
      {
        ebUseCases.selectParameter(p->getParameter()->getID(), true);
        handled = true;
        return false;
      }

      if(p->isSelected())
      {
        found = true;
      }
    }

    return true;
  });

  forEach(cb);

  if(!handled)
    if(auto p = std::dynamic_pointer_cast<MiniParameter>(first()))
      ebUseCases.selectParameter(p->getParameter()->getID(), true);
}

void ParameterCarousel::setupChildControlsForParameterWithoutButtonMapping(Parameter* selectedParameter)
{
  const auto paramID = selectedParameter->getID().getNumber();

  if(ScaleGroup::isScaleParameter(selectedParameter) || paramID == 247 || paramID == 248)
  {
    setupChildControls(selectedParameter, { 247, 248, 312 });
  }
  else if(paramID == 249 || paramID == 250 || paramID == 252 || paramID == 253)
  {
    setupChildControls(selectedParameter, { 249, 250, 252, 253 });
  }
}
