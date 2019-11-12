#include <proxies/hwui/panel-unit/PanelUnitParameterEditMode.h>
#include <groups/ScaleGroup.h>
#include <parameters/ScaleParameter.h>
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

  if(std::shared_ptr<PanelUnitParameterEditMode> edit = std::dynamic_pointer_cast<PanelUnitParameterEditMode>(um))
  {
    if(selectedParameter)
    {
      auto button = edit->findButtonForParameter(selectedParameter);

      if(static_cast<int>(button) != -1)
      {
        setupChildControls(edit, selectedParameter, button);
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
  std::list<int> buttonAssignments = edit->getButtonAssignments(button);

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
  int yPos = ySpaceing;
  size_t maxNumParameters = 4;
  size_t missingParams = maxNumParameters - buttonAssignments.size();
  yPos += missingParams * (miniParamHeight + ySpaceing);
  for(int i : buttonAssignments)
  {
    auto eb = Application::get().getPresetManager()->getEditBuffer();
    auto param = eb->findParameterByID(i);
    if(!param)
      param = eb->findGlobalParameterByID(i);
    auto miniParam = new MiniParameter(param, Rect(0, yPos, miniParamWidth, miniParamHeight));
    if(dynamic_cast<ScaleParameter*>(selectedParameter) != nullptr)
    {
      miniParam->setSelected(param->getID() == ScaleGroup::getScaleBaseParameterID());
    }
    else
    {
      miniParam->setSelected(param == selectedParameter);
    }
    addControl(miniParam);
    yPos += ySpaceing;
    yPos += miniParamHeight;
  }
}

void ParameterCarousel::antiTurn()
{
  auto foundCtrl = std::dynamic_pointer_cast<MiniParameter>(*getControls().rbegin());
  for(const auto& ctrl : getControls())
  {
    if(auto p = std::dynamic_pointer_cast<MiniParameter>(ctrl))
    {
      if(p->isSelected())
      {
        Application::get().getPresetManager()->getEditBuffer()->undoableSelectParameter(
            to_string(foundCtrl->getParameter()->getID()));
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
  tIfCallback cb = ([&](const tControlPtr& ctrl) -> bool {
    if(auto p = std::dynamic_pointer_cast<MiniParameter>(ctrl))
    {
      if(found)
      {
        Application::get().getPresetManager()->getEditBuffer()->undoableSelectParameter(
            p->getParameter()->getID(), p->getParameter()->getVoiceGroup());
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
      Application::get().getPresetManager()->getEditBuffer()->undoableSelectParameter(
          p->getParameter()->getID(), p->getParameter()->getVoiceGroup());
}

bool ParameterCarousel::containsSelectedParameter() const
{
  for(auto& p : getControls())
  {
    if(auto mp = dynamic_cast<MiniParameter*>(p.get()))
    {
      if(mp->getParameter() == Application::get().getPresetManager()->getEditBuffer()->getSelected())
        return true;
    }
  }
  return false;
}