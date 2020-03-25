#include <Application.h>
#include <device-settings/DebugLevel.h>
#include <http/UndoScope.h>
#include <libundo/undo/SwapData.h>
#include <libundo/undo/Transaction.h>
#include <libundo/undo/TransactionCreationScope.h>
#include <math.h>
#include <parameters/PhysicalControlParameter.h>
#include <parameters/value/QuantizedValue.h>
#include <presets/EditBuffer.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/base-unit/PlayPanel.h>
#include <proxies/hwui/base-unit/soled/SOLED.h>
#include <proxies/hwui/buttons.h>
#include <proxies/hwui/debug-oled/DebugOLED.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/Oleds.h>
#include <proxies/hwui/panel-unit/boled/BOLED.h>
#include <proxies/hwui/panel-unit/EditPanel.h>
#include <proxies/hwui/panel-unit/RotaryEncoder.h>
#include <proxies/lpc/LPCProxy.h>
#include <tools/Signal.h>
#include <xml/FileOutStream.h>
#include <groups/HardwareSourcesGroup.h>
#include <tools/PerformanceTimer.h>
#include <proxies/hwui/descriptive-layouts/LayoutFolderMonitor.h>
#include <nltools/messaging/Message.h>
#include <device-settings/LayoutMode.h>
#include <xml/StandardOutStream.h>
#include <serialization/EditBufferSerializer.h>
#include <iostream>
#include <giomm.h>

HWUI::HWUI()
    : m_readersCancel(Gio::Cancellable::create())
    , m_buttonStates { false }
    , m_focusAndMode(UIFocus::Parameters, UIMode::Select)
    , m_blinkCount(0)
{
#ifdef _DEVELOPMENT_PC
  if(isatty(fileno(stdin)))
  {
    m_keyboardInput = Gio::DataInputStream::create(Gio::UnixInputStream::create(0, true));
    m_keyboardInput->read_line_async(mem_fun(this, &HWUI::onKeyboardLineRead), m_readersCancel);
  }
#endif

  nltools::msg::receive<nltools::msg::ButtonChangedMessage>(nltools::msg::EndPoint::Playground,
                                                            sigc::mem_fun(this, &HWUI::onButtonMessage));
}

HWUI::~HWUI()
{
  DebugLevel::warning(__PRETTY_FUNCTION__, __LINE__);
  m_readersCancel->cancel();
}

void HWUI::deInit()
{
  Oleds::get().deInit();
}

void HWUI::onButtonMessage(const nltools::msg::ButtonChangedMessage &msg)
{
  onButtonPressed(static_cast<Buttons>(msg.buttonId), msg.pressed);
}

void HWUI::init()
{
  m_panelUnit.init();
  m_baseUnit.init();

  Oleds::get().syncRedraw();
}

void HWUI::indicateBlockingMainThread()
{
  m_switchOffBlockingMainThreadIndicator.setCallback([=]() { m_baseUnit.indicateBlockingMainThread(false); });

  m_switchOffBlockingMainThreadIndicator.refresh(std::chrono::seconds(5));
  m_baseUnit.indicateBlockingMainThread(true);
}

void HWUI::setupFocusAndMode()
{
  m_panelUnit.setupFocusAndMode(m_focusAndMode);
  m_baseUnit.setupFocusAndMode(m_focusAndMode);
}

void HWUI::onKeyboardLineRead(Glib::RefPtr<Gio::AsyncResult> &res)
{
  std::string line;

  if(m_keyboardInput->read_line_finish(res, line))
  {
    if(!line.empty())
    {
      if(line == "r")
      {
        LayoutFolderMonitor::get().bruteForce();
      }
      if(line == "t")
      {
        onButtonPressed(Buttons::BUTTON_SETUP, true);
      }
      else if(line == "!t")
      {
        onButtonPressed(Buttons::BUTTON_SETUP, false);
      }
      else if(line == "s")
      {
        onButtonPressed(Buttons::BUTTON_SHIFT, true);
      }
      else if(line == "!s")
      {
        onButtonPressed(Buttons::BUTTON_SHIFT, false);
      }
      else if(line == "a")
      {
        onButtonPressed(Buttons::BUTTON_A, true);
      }
      else if(line == "!a")
      {
        onButtonPressed(Buttons::BUTTON_A, false);
      }
      else if(line == "b")
      {
        onButtonPressed(Buttons::BUTTON_B, true);
      }
      else if(line == "!b")
      {
        onButtonPressed(Buttons::BUTTON_B, false);
      }
      else if(line == "c")
      {
        onButtonPressed(Buttons::BUTTON_C, true);
      }
      else if(line == "!c")
      {
        onButtonPressed(Buttons::BUTTON_C, false);
      }
      else if(line == "d")
      {
        onButtonPressed(Buttons::BUTTON_D, true);
      }
      else if(line == "!d")
      {
        onButtonPressed(Buttons::BUTTON_D, false);
      }
      else if(line == "e")
      {
        onButtonPressed(Buttons::BUTTON_ENTER, true);
      }
      else if(line == "!e")
      {
        onButtonPressed(Buttons::BUTTON_ENTER, false);
      }
      else if(line == "i")
      {
        onButtonPressed(Buttons::BUTTON_INFO, true);
        onButtonPressed(Buttons::BUTTON_INFO, false);
      }
      else if(line == "!666")
      {
        onButtonPressed(Buttons::BUTTON_UNDO, true);
        onButtonPressed(Buttons::BUTTON_REDO, true);
        onButtonPressed(Buttons::BUTTON_UNDO, false);
        onButtonPressed(Buttons::BUTTON_REDO, false);
      }
      else if(line == "stress-undo-s")
      {
        Application::get().getPresetManager()->stress(1000);
      }
      else if(line == "stress-undo-m")
      {
        Application::get().getPresetManager()->stress(10000);
      }
      else if(line == "stress-undo-l")
      {
        Application::get().getPresetManager()->stress(100000);
      }
      else if(line == "stress-change-all")
      {
        Application::get().getPresetManager()->stressAllParams(500);
      }
      else if(line == "stress-pm")
      {
        Application::get().getPresetManager()->stressLoad(1000);
      }
      else if(line == "dump-editbuffer")
      {
        auto eb = Application::get().getPresetManager()->getEditBuffer();
        auto wtf = std::shared_ptr<std::ostream>(&std::cout, [](void *) {});
        auto os = std::make_shared<StandardOutStream>(wtf);
        XmlWriter writer(os);
        eb->writeDocument(writer, 0);

        EditBufferSerializer ebs(eb);
        ebs.write(writer);
      }
      else if(line == "undo-performance-compare")
      {
        Application::get().stopWatchDog();

        for(int steps = 0; steps < 50; steps++)
        {
          unsigned long long totalTraverse = 0;
          long avgusTraverse = 0;
          for(int i = 1; i < 101; i++)
          {
            auto traverse = Application::get().getPresetManager()->getUndoScope().getRootTransaction()->traverseTree();
            totalTraverse += traverse;
          }
          avgusTraverse = static_cast<long>(totalTraverse / 100);
          DebugLevel::warning("Count: ~", steps * 1000, "Transactions Traverse avg:", avgusTraverse / 1000, "\bms");
          Application::get().getPresetManager()->stressBlocking(1000);
        }
        Application::get().runWatchDog();
      }
      else if(line == "inc-all-fine")
      {
        Application::get().getPresetManager()->incAllParamsFine();
      }
      else if(line.at(0) == '!')
      {
        onButtonPressed(Buttons::BUTTON_SHIFT, true);
        m_panelUnit.getEditPanel().getKnob().fake(1);
        onButtonPressed(Buttons::BUTTON_SHIFT, false);
      }
      else if(line.at(0) == '@')
      {
        onButtonPressed(Buttons::BUTTON_SHIFT, true);
        m_panelUnit.getEditPanel().getKnob().fake(-1);
        onButtonPressed(Buttons::BUTTON_SHIFT, false);
      }
      else if(line.at(0) == '+')
      {
        float f = line.size();
        float sign = f < 0 ? -1 : 1;
        f = powf(f, 1.5f) * sign;
        auto c = static_cast<signed char>(roundf(f));
        m_panelUnit.getEditPanel().getKnob().fake(c);
      }
      else if(line.at(0) == '-')
      {
        float f = line.size();
        f = -f;
        float sign = f < 0 ? -1 : 1;
        f = powf(fabsf(f), 1.5f) * sign;
        auto c = static_cast<signed char>(roundf(f));
        m_panelUnit.getEditPanel().getKnob().fake(c);
      }
      else if(line.find('z') == 0)
      {
        auto p = Application::get().getPresetManager()->getEditBuffer()->findParameterByID(
            HardwareSourcesGroup::getUpperRibbonParameterID());

        if(auto h = dynamic_cast<PhysicalControlParameter *>(p))
        {
          h->onChangeFromLpc(h->getControlPositionValue() + line.size() * 0.1);
        }
        else
        {
          auto changer = p->getValue().startUserEdit(Initiator::EXPLICIT_LPC);
          changer->changeBy(1.0 / p->getValue().getCoarseDenominator());
        }
      }
      else if(line.find('x') == 0)
      {
        auto p = Application::get().getPresetManager()->getEditBuffer()->findParameterByID(
            HardwareSourcesGroup::getUpperRibbonParameterID());

        if(auto h = dynamic_cast<PhysicalControlParameter *>(p))
        {
          h->onChangeFromLpc(h->getControlPositionValue() - line.size() * 0.1);
        }
        else
        {
          auto changer = p->getValue().startUserEdit(Initiator::EXPLICIT_LPC);
          changer->changeBy(-1.0 / p->getValue().getCoarseDenominator());
        }
      }
      else
      {
        try
        {
          auto i = (Buttons) std::stoi(line);
          if(i < Buttons::NUM_BUTTONS)
          {
            if(line.back() == 'u')
            {
              onButtonPressed(i, false);
            }
            else if(line.back() == 'd')
            {
              onButtonPressed(i, true);
            }
            else
            {
              onButtonPressed(i, true);
              onButtonPressed(i, false);
            }
          }
          else
          {
            DebugLevel::error("Number bigger than ", m_buttonStates.size(), __PRETTY_FUNCTION__, __LINE__);
          }
        }
        catch(...)
        {
          DebugLevel::error("Exception caught in", __PRETTY_FUNCTION__, __LINE__);
        }
      }
    }
  }

  m_keyboardInput->read_line_async(mem_fun(this, &HWUI::onKeyboardLineRead), m_readersCancel);
}

PanelUnit &HWUI::getPanelUnit()
{
  return m_panelUnit;
}

const PanelUnit &HWUI::getPanelUnit() const
{
  return m_panelUnit;
}

BaseUnit &HWUI::getBaseUnit()
{
  return m_baseUnit;
}

void HWUI::onButtonPressed(Buttons buttonID, bool state)
{
  m_buttonStates[(int) buttonID] = state;

  setModifiers(buttonID, state);

  if(!detectAffengriff(buttonID, state))
  {
    if(!m_baseUnit.onButtonPressed(buttonID, m_modifiers, state))
    {
      if(!m_panelUnit.onButtonPressed(buttonID, m_modifiers, state))
      {
        if(buttonID == Buttons::BUTTON_SETUP && state)
        {
          if(m_focusAndMode.focus == UIFocus::Setup)
          {
            undoableSetFocusAndMode(UIFocus::Parameters);
          }
          else
          {
            undoableSetFocusAndMode({ UIFocus::Setup, UIMode::Select });
          }
        }
      }
    }
  }
}

void HWUI::setModifiers(Buttons buttonID, bool state)
{
  if(buttonID == Buttons::BUTTON_SHIFT)
  {
    if(state)
    {
      addModifier(ButtonModifier::SHIFT);
    }
    else
    {
      removeModifier(ButtonModifier::SHIFT);
    }
  }

  if(buttonID == Buttons::BUTTON_FINE)
  {
    if(isFineAllowed())
    {
      m_fineButton.setShiftedWhilePressDown(isModifierSet(ButtonModifier::SHIFT));
      m_fineButton.setButtonState(state);
    }
  }
  if(m_fineButton.getModifierState())
  {
    addModifier(ButtonModifier::FINE);
  }
  else
  {
    removeModifier(ButtonModifier::FINE);
  }
}

bool HWUI::isFineAllowed()
{
  auto uiFocus = getFocusAndMode().focus;
  return uiFocus == UIFocus::Parameters || uiFocus == UIFocus::Sound;
}

bool HWUI::detectAffengriff(Buttons buttonID, bool state)
{
  if(!state)
  {
    m_affengriffState = 0;
    return false;
  }

  if(m_affengriffState == 0 && (buttonID == Buttons::BUTTON_SHIFT))
  {
    m_affengriffState = 1;
  }
  else if(m_affengriffState == 1 && (buttonID == Buttons::BUTTON_SETUP))
  {
    m_affengriffState = 2;
  }
  else
  {
    m_affengriffState = 0;
  }
  return false;
}

void HWUI::unsetFineMode()
{
  if(m_fineButton.onlyTemporary())
  {
    m_fineButton.setState(TOGGLED_OFF);
    removeModifier(ButtonModifier::FINE);
  }
}

ButtonModifiers HWUI::getButtonModifiers() const
{
  return m_modifiers;
}

bool HWUI::isModifierSet(ButtonModifier m) const
{
  return m_modifiers[m];
}

bool HWUI::isResolutionFine() const
{
  return isModifierSet(ButtonModifier::FINE);
}

void HWUI::setModifiers(ButtonModifiers m)
{
  if(m != m_modifiers)
  {
    m_modifiers = m;
    m_modifersChanged.send(m_modifiers);
  }
}

void HWUI::addModifier(ButtonModifier i)
{
  auto cp = m_modifiers;
  cp.set(i);
  setModifiers(cp);
}

void HWUI::removeModifier(ButtonModifier i)
{
  auto cp = m_modifiers;
  cp.reset(i);
  setModifiers(cp);
}

sigc::connection HWUI::onModifiersChanged(const sigc::slot<void, ButtonModifiers> &cb)
{
  return m_modifersChanged.connectAndInit(cb, m_modifiers);
}

sigc::connection HWUI::connectToBlinkTimer(const sigc::slot<void, int> &cb)
{
  if(m_blinkTimer.empty())
  {
    m_blinkTimerConnection.disconnect();

    m_blinkTimerConnection = Application::get().getMainContext()->signal_timeout().connect(
        sigc::mem_fun(this, &HWUI::onBlinkTimeout), 500);
    m_blinkCount = 1;
  }

  return m_blinkTimer.connectAndInit(cb, m_blinkCount);
}

bool HWUI::onBlinkTimeout()
{
  m_blinkTimer.send(++m_blinkCount);
  return true;
}

void HWUI::undoableSetFocusAndMode(UNDO::Transaction *transaction, FocusAndMode focusAndMode)
{
  if(Application::get().getPresetManager()->isLoading())
    return;

  if(m_focusAndModeFrozen)
    return;

  if(focusAndMode.focus == UIFocus::Unchanged)
    focusAndMode.focus = m_focusAndMode.focus;

  if(focusAndMode.mode == UIMode::Unchanged)
    focusAndMode.mode = m_focusAndMode.mode;

  if(focusAndMode.focus != m_focusAndMode.focus)
    if(focusAndMode.mode == UIMode::Unchanged)
      focusAndMode.mode = UIMode::Select;

  auto swapData = UNDO::createSwapData(restrictFocusAndMode(focusAndMode));
  auto oldSwap = UNDO::createSwapData(m_focusAndMode);

  transaction->addSimpleCommand([=](UNDO::Command::State) {
    swapData->swapWith(m_focusAndMode);
    oldSwap->swapWith(m_oldFocusAndMode);
    setupFocusAndMode();
  });
}

FocusAndMode HWUI::getFocusAndMode() const
{
  return m_focusAndMode;
}

FocusAndMode HWUI::getOldFocusAndMode() const
{
  return m_oldFocusAndMode;
}

VoiceGroup HWUI::getCurrentVoiceGroup() const
{
  return m_currentVoiceGroup;
}

void HWUI::setCurrentVoiceGroup(VoiceGroup v)
{
  if(v == VoiceGroup::I || v == VoiceGroup::II)
    if(std::exchange(m_currentVoiceGroup, v) != v)
      m_voiceGoupSignal.deferedSend(m_currentVoiceGroup);
}

void HWUI::setCurrentVoiceGroupAndUpdateParameterSelection(UNDO::Transaction *transaction, VoiceGroup v)
{
  setCurrentVoiceGroup(v);
  undoableUpdateParameterSelection(transaction);
}

void HWUI::undoableUpdateParameterSelection(UNDO::Transaction *transaction)
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  auto selected = eb->getSelected();
  auto id = selected->getID();

  if(id.getVoiceGroup() != VoiceGroup::Global)
  {
    eb->undoableSelectParameter(transaction, { id.getNumber(), m_currentVoiceGroup });
  }
}

void HWUI::toggleCurrentVoiceGroupAndUpdateParameterSelection(UNDO::Transaction *transaction)
{
  if(Application::get().getPresetManager()->getEditBuffer()->getType() == SoundType::Single)
    return;

  if(m_currentVoiceGroup == VoiceGroup::I)
    setCurrentVoiceGroupAndUpdateParameterSelection(transaction, VoiceGroup::II);
  else if(m_currentVoiceGroup == VoiceGroup::II)
    setCurrentVoiceGroupAndUpdateParameterSelection(transaction, VoiceGroup::I);
}

void HWUI::toggleCurrentVoiceGroup()
{
  if(Application::get().getPresetManager()->getEditBuffer()->getType() == SoundType::Single)
    return;

  if(m_currentVoiceGroup == VoiceGroup::I)
    setCurrentVoiceGroup(VoiceGroup::II);
  else if(m_currentVoiceGroup == VoiceGroup::II)
    setCurrentVoiceGroup(VoiceGroup::I);
}

sigc::connection HWUI::onCurrentVoiceGroupChanged(const sigc::slot<void, VoiceGroup> &cb)
{
  return m_voiceGoupSignal.connectAndInit(cb, m_currentVoiceGroup);
}

void HWUI::setFocusAndMode(const UIDetail &detail)
{
  setFocusAndMode({ m_focusAndMode.focus, m_focusAndMode.mode, detail });
}

void HWUI::setFocusAndMode(FocusAndMode focusAndMode)
{
  if(m_focusAndModeFrozen)
    return;

  focusAndMode.fixUnchanged(m_focusAndMode);
  m_oldFocusAndMode = m_focusAndMode;
  m_focusAndMode = restrictFocusAndMode(focusAndMode);
  setupFocusAndMode();
}

void HWUI::undoableSetFocusAndMode(FocusAndMode focusAndMode)
{
  auto scope = Application::get().getUndoScope()->startCuckooTransaction();
  undoableSetFocusAndMode(scope->getTransaction(), focusAndMode);
}

void HWUI::setUiModeDetail(UIDetail detail)
{
  m_focusAndMode.detail = detail;

  if(Application::get().getSettings()->getSetting<LayoutMode>()->get() != LayoutVersionMode::Old)
  {
    setupFocusAndMode();
  }
}

void HWUI::freezeFocusAndMode()
{
  m_focusAndModeFrozen = true;
}

void HWUI::thawFocusAndMode()
{
  m_focusAndModeFrozen = false;
}

FocusAndMode HWUI::fixFocusAndModeWithAnys(FocusAndMode in)
{
  if(in.focus == UIFocus::Any)
    in.focus = UIFocus::Parameters;

  if(in.mode == UIMode::Any)
    in.mode = UIMode::Select;

  if(in.detail == UIDetail::Any)
    in.detail = UIDetail::Init;

  return in;
}

FocusAndMode HWUI::removeInvalidsFromSound(FocusAndMode in)
{
  if(in.focus == UIFocus::Sound)
  {
    if(in.mode == UIMode::Store || in.mode == UIMode::Info)
    {
      in.mode = UIMode::Select;
    }
  }

  return in;
}

FocusAndMode HWUI::removeEditOnFocusChange(FocusAndMode in) const
{
  const bool isDesiredParameter = (in.focus == UIFocus::Parameters);
  const bool isDesiredPresetManager = (in.focus == UIFocus::Banks) || (in.focus == UIFocus::Presets);
  const bool isDesiredSound = in.focus == UIFocus::Sound;

  const bool isCurrentPresetManager
      = (m_focusAndMode.focus == UIFocus::Banks) || (m_focusAndMode.focus == UIFocus::Presets);
  const bool isCurrentParameter = m_focusAndMode.focus == UIFocus::Parameters;
  const bool isCurrentSound = m_focusAndMode.focus == UIFocus::Sound;

  const bool switchFromPresetManagerToParameter = (isCurrentPresetManager && isDesiredParameter);
  const bool switchFromPresetManagerToSound = (isCurrentPresetManager && isDesiredSound);

  const bool switchFromParameterToPresetManager = (isCurrentParameter && isDesiredPresetManager);
  const bool switchFromParameterToSound = (isCurrentParameter && isDesiredSound);

  const bool switchFromSoundToPresetManager = (isCurrentSound && isDesiredPresetManager);
  const bool switchFromSoundToParameter = (isCurrentSound && isDesiredParameter);

  if(switchFromPresetManagerToParameter || switchFromParameterToPresetManager || switchFromSoundToPresetManager
     || switchFromSoundToParameter || switchFromPresetManagerToSound || switchFromParameterToSound)
  {
    if(m_focusAndMode.mode == UIMode::Edit)
    {
      in.mode = UIMode::Select;
    }
  }

  return in;
}

FocusAndMode HWUI::restrictFocusAndMode(FocusAndMode in) const
{
  in = fixFocusAndModeWithAnys(in);
  in = removeEditOnFocusChange(in);
  in = removeInvalidsFromSound(in);
  return in;
}
