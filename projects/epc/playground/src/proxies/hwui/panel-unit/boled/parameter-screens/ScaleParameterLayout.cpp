#include <Application.h>
#include <groups/ParameterGroup.h>
#include <groups/ScaleGroup.h>
#include <http/UndoScope.h>
#include <parameters/Parameter.h>
#include <presets/EditBuffer.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/controls/Button.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ScaleParameterLayout.h>

void toggleHightlight(Control* c);

void ScaleParameterSelectLayout::init()
{
  super::init();

  auto eb = Application::get().getPresetManager()->getEditBuffer();
  eb->getParameterGroupByID({ "Scale", VoiceGroup::Global })
      ->onGroupChanged(sigc::mem_fun(this, &ScaleParameterSelectLayout::updateResetButton));
}

void ScaleParameterSelectLayout::addButtons()
{
  m_resetButton = addControl(new Button("", Buttons::BUTTON_A));
  addControl(new Button("<", Buttons::BUTTON_B));
  addControl(new Button(">", Buttons::BUTTON_C));
  updateResetButton();
}

void ScaleParameterSelectLayout::updateResetButton()
{
  m_resetButton->setText(StringAndSuffix { resetEnabled() ? "Reset" : "" });
}

bool ScaleParameterSelectLayout::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  if(super::onButton(i, down, modifiers))
    return true;

  if(down)
  {
    switch(i)
    {

      case Buttons::BUTTON_A:
        if(resetEnabled())
        {
          toggleHightlight(m_resetButton);
        }
        return true;

      case Buttons::BUTTON_B:
        selectParameter(-1);
        return true;

      case Buttons::BUTTON_C:
        selectParameter(+1);
        return true;

      case Buttons::BUTTON_ENTER:
        if(m_resetButton->isHighlight())
        {
          reset();
          m_resetButton->setHighlight(false);
          return true;
        }
    }
  }
  return false;
}

void ScaleParameterSelectLayout::reset()
{
  EditBufferUseCases ebUseCases(Application::get().getPresetManager()->getEditBuffer());
  ebUseCases.resetCustomScale();
}

bool ScaleParameterSelectLayout::resetEnabled() const
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  auto scaleGroup = dynamic_cast<ScaleGroup*>(eb->getParameterGroupByID({ "Scale", VoiceGroup::Global }));
  return scaleGroup->isAnyOffsetChanged();
}

void ScaleParameterSelectLayout::selectParameter(int inc)
{
  static const auto ids = std::vector<int> { 312, 391, 313, 314, 315, 316, 317, 318, 319, 320, 321, 322, 323 };
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  EditBufferUseCases ebUseCases { eb };
  auto id = eb->getSelectedParameterNumber();

  auto idIt = std::find(ids.begin(), ids.end(), id);
  if(idIt == ids.end() || (*idIt == 323 && inc > 0))
  {
    id = 312;
  }
  else
  {
    auto toEnd = std::distance(idIt, ids.end());
    auto toBegin = std::distance(ids.begin(), idIt);

    if(inc > 0 && toEnd != 1)
      inc = std::min<int>(toEnd, inc);
    else if(inc > 0)
      idIt = ids.begin();

    if(inc < 0 && toBegin != 0)
      inc = std::min<int>(toBegin, inc);
    else if(inc < 0)
      idIt = ids.end();

    idIt += inc;
    id = *idIt;
  }

  ebUseCases.selectParameter({ id, VoiceGroup::Global }, true);
}

void toggleHightlight(Control* c)
{
  c->setHighlight(!c->isHighlight());
}
