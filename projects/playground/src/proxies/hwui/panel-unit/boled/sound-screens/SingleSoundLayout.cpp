#include <Application.h>
#include <device-settings/RandomizeAmount.h>
#include <device-settings/Settings.h>
#include <device-settings/TransitionTime.h>
#include <http/UndoScope.h>
#include <libundo/undo/Scope.h>
#include <libundo/undo/Transaction.h>
#include <playground.h>
#include <presets/EditBuffer.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/buttons.h>
#include <proxies/hwui/controls/Button.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/InvertedLabel.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/Caption.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/RandomizeAmountLabel.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/RandomizeAmountSlider.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/SingleSoundEditMenuLegacy.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/TransitionTimeLabel.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/TransitionTimeSlider.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/SingleSoundLayout.h>
#include <proxies/hwui/panel-unit/EditPanel.h>
#include <proxies/hwui/panel-unit/PanelUnit.h>
#include <proxies/hwui/UsageMode.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/LockedIndicator.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/AnyParameterLockedIndicator.h>

SingleSoundLayout::SingleSoundLayout(FocusAndMode focusAndMode)
    : super(Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled())
{
  addControl(new Caption("Single", Rect(0, 0, 64, 13)));

  m_initButton = addControl(new Button("Init", Buttons::BUTTON_A));

  m_randomizeLabel = addControl(new LabelRegular8("Amount", Rect(67, 12, 58, 11)));
  m_randomizeSlider = addControl(new RandomizeAmountSlider(Rect(67, 28, 58, 4)));
  m_randomizeAmount = addControl(new RandomizeAmountLabel(Rect(67, 36, 58, 12)));

  m_randomButton = addControl(new Button("Randomize", Buttons::BUTTON_B));

  m_transitionTimeLabel = addControl(new LabelRegular8("Time", Rect(131, 12, 58, 11)));
  m_transitionTimeSlider = addControl(new TransitionTimeSlider(Rect(131, 28, 58, 4)));
  m_transitionTimeAmount = addControl(new TransitionTimeLabel(Rect(131, 36, 58, 12)));
  m_transitionTimeButton = addControl(new Button("Transition", Buttons::BUTTON_C));

  toggleEditMenu(false);

  m_paramLocked = addControl(new AnyParameterLockedIndicator(Rect(244, 2, 10, 11)));
  m_edit = addControl(new InvertedLabel("Edit", Rect(8, 26, 48, 12)));
  m_edit->setHighlight(true);
  m_edit->setVisible(false);
  m_paramLocked = addControl(new AnyParameterLockedIndicator(Rect(244, 2, 10, 11)));

  m_initMenu = addControl(new SingleSoundEditMenuLegacy(Rect(195, 1, 58, 63)));
  m_initMenu->setVisible(false);
}

SingleSoundLayout::~SingleSoundLayout()
{
}

bool SingleSoundLayout::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  if(down)
  {
    if(m_selectedColumn != Column::Edit)
    {
      if(i == Buttons::BUTTON_A)
      {
        toggleColumn(Column::Init);
        return true;
      }
      else if(i == Buttons::BUTTON_B)
      {
        toggleColumn(Column::Randomize);
        return true;
      }
      else if(i == Buttons::BUTTON_C)
      {
        toggleColumn(Column::TranstionTime);
        return true;
      }
    }

    if(i == Buttons::BUTTON_D)
    {
      if(m_selectedColumn == Column::Edit)
        if(m_initMenu != nullptr)
          m_initMenu->toggle();
      return true;
    }
    else if(i == Buttons::BUTTON_ENTER)
    {
      action();
      if(m_selectedColumn != Column::Randomize)
        toggleColumn(m_selectedColumn);
      return true;
    }
    else if(i == Buttons::BUTTON_DEFAULT)
    {
      setDefault();
      return true;
    }
    else if(i == Buttons::BUTTON_EDIT)
    {
      toggleColumn(Column::Edit);
      return true;
    }
    else if(i == Buttons::BUTTON_PRESET)
    {
      Application::get().getHWUI()->setFocusAndMode({ UIFocus::Presets, UIMode::Select });
      return true;
    }
    else if(i == Buttons::BUTTON_STORE)
    {
      Application::get().getHWUI()->setFocusAndMode({ UIFocus::Presets, UIMode::Store });
      return true;
    }
    else if(i == Buttons::BUTTON_SOUND)
    {
      Application::get().getHWUI()->setFocusAndMode({ UIFocus::Parameters, UIMode::Select });
      return true;
    }
  }

  return super::onButton(i, down, modifiers);
}

void SingleSoundLayout::toggleColumn(Column c)
{
  if(m_selectedColumn == c)
    m_selectedColumn = Column::None;
  else
    m_selectedColumn = c;

  setup();
}

void SingleSoundLayout::toggleEditMenu(bool inEdit)
{
  remove(m_initMenu);
  m_initMenu = nullptr;
  remove(m_emptyButton);
  m_emptyButton = nullptr;

  if(inEdit)
  {
    m_initMenu = addControl(new SingleSoundEditMenuLegacy(Rect(195, 0, 58, 64)));
    m_initMenu->setVisible(true);
  }
  else
  {
    m_emptyButton = addControl(new Button("", Buttons::BUTTON_D));
    m_emptyButton->setVisible(true);
  }
}

void SingleSoundLayout::setup()
{
  setHighlight(false);

  bool inEditMode = m_selectedColumn == Column::Edit;

  toggleEditMenu(inEditMode);

  m_edit->setVisible(inEditMode);

  m_initButton->blind(inEditMode);
  m_randomButton->blind(inEditMode);
  m_transitionTimeButton->blind(inEditMode);

  switch(m_selectedColumn)
  {
    case Column::None:
      break;

    case Column::Init:
      m_initButton->setHighlight(true);
      break;

    case Column::Randomize:
      m_randomizeLabel->setHighlight(true);
      m_randomizeSlider->setHighlight(true);
      m_randomizeAmount->setHighlight(true);
      m_randomButton->setHighlight(true);
      break;

    case Column::TranstionTime:
      m_transitionTimeLabel->setHighlight(true);
      m_transitionTimeSlider->setHighlight(true);
      m_transitionTimeAmount->setHighlight(true);
      m_transitionTimeButton->setHighlight(true);
      break;

    case Column::Edit:
      if(m_initMenu != nullptr)
      {
        m_initMenu->setHighlight(true);
        m_initMenu->selectButton(0);
      }
      break;
  }
}

void SingleSoundLayout::setDefault()
{
  if(m_selectedColumn == Column::Randomize)
    Application::get().getSettings()->getSetting<RandomizeAmount>()->setDefault();
  else if(m_selectedColumn == Column::TranstionTime)
    Application::get().getSettings()->getSetting<TransitionTime>()->setDefault();
}

bool SingleSoundLayout::onRotary(int inc, ButtonModifiers modifiers)
{
  if(m_selectedColumn == Column::Randomize)
    Application::get().getSettings()->getSetting<RandomizeAmount>()->incDec(inc, modifiers);
  else if(m_selectedColumn == Column::TranstionTime)
    Application::get().getSettings()->getSetting<TransitionTime>()->incDec(inc, modifiers);

  return Layout::onRotary(inc, modifiers);
}

void SingleSoundLayout::action()
{
  switch(m_selectedColumn)
  {
    case Column::Init:
      initSound();
      break;

    case Column::Randomize:
      randomize();
      break;

    case Column::Edit:
      if(m_initMenu != nullptr)
        m_initMenu->doAction();
      break;
    case Column::None:
    case Column::TranstionTime:
      break;
  }
}

void SingleSoundLayout::initSound()
{
  UNDO::Scope::tTransactionScopePtr scope = Application::get().getUndoScope()->startTransaction("Init Sound");
  Application::get().getPresetManager()->getEditBuffer()->undoableInitSound(scope->getTransaction());
  Application::get().getHWUI()->getPanelUnit().getUsageMode()->bruteForceUpdateLeds();
}

void SingleSoundLayout::randomize()
{
  UNDO::Scope::tTransactionScopePtr scope = Application::get().getUndoScope()->startTransaction("Randomize Sound");
  auto editBuffer = Application::get().getPresetManager()->getEditBuffer();
  editBuffer->undoableRandomize(scope->getTransaction(), Initiator::EXPLICIT_HWUI);
  Application::get().getHWUI()->getPanelUnit().getUsageMode()->bruteForceUpdateLeds();
}
