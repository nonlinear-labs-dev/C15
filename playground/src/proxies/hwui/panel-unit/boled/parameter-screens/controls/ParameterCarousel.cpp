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

class NeverHighlitButton : public Button
{
 public:
  using Button::Button;

  virtual void setHighlight(bool isHighlight) override
  {
  }
};

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
      int button = edit->findButtonForParameter(selectedParameter);

      if(button != -1)
      {
        setupChildControls(edit, selectedParameter, (Buttons)button);
      }
    }
  }

  if(getNumChildren() == 0)
  {
    addControl(new NeverHighlitButton("", Rect(0, 51, 58, 11)));
  }

  setDirty();
}

void ParameterCarousel::setupChildControls(const std::shared_ptr<PanelUnitParameterEditMode>& edit,
                                           Parameter* selectedParameter, Buttons button)
{
  std::list<int> buttonAssignments = edit->getButtonAssignments((int)button);

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
    auto param = Application::get().getPresetManager()->getEditBuffer()->findParameterByID(i);
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
  for(auto ctrl : getControls())
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
  tIfCallback cb = ([&](tControlPtr ctrl) -> bool {
    if(auto p = std::dynamic_pointer_cast<MiniParameter>(ctrl))
    {
      if(found)
      {
        Application::get().getPresetManager()->getEditBuffer()->undoableSelectParameter(
            to_string(p->getParameter()->getID()));
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
          to_string(p->getParameter()->getID()));
}
