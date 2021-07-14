#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/controls/Button.h>
#include <sigc++/adaptors/hide.h>
#include "BankButton.h"

auto getSoundType()
{
  return Application::get().getPresetManager()->getEditBuffer()->getType();
}

BankButton::BankButton(const Rect& pos, bool bankFocus)
    : ControlWithChildren(pos)
    , m_bankFocus(bankFocus)
{
  m_soundTypeChanged = Application::get().getPresetManager()->getEditBuffer()->onSoundTypeChanged(
      sigc::hide(sigc::mem_fun(this, &BankButton::bruteForce)));
}

BankButton::~BankButton()
{
  m_soundTypeChanged.disconnect();
}

bool BankButton::onButton(Buttons b, bool down, ButtonModifiers modifiers)
{
  if(m_buttonAHandler && b == Buttons::BUTTON_A)
  {
    m_buttonAHandler->onButtonEvent(down);
    return true;
  }
  return false;
}

void BankButton::bruteForce()
{
  clear();

  auto mode = Application::get().getHWUI()->getFocusAndMode().mode;

  switch(getSoundType())
  {
    case SoundType::Single:
      installSingle();
      break;
    case SoundType::Layer:
    case SoundType::Split:
      if(mode == UIMode::Select)
        installDual();
      else
        installSingle();
      break;
  }
}

void BankButton::installSingle()
{
  auto focusAndMode = Application::get().getHWUI()->getFocusAndMode();
  auto bankbutton = addControl(new Button("Bank", { 0, 15, 58, 11 }));
  bankbutton->setHighlight(focusAndMode.focus == UIFocus::Banks);

  auto toggleBankFocus = [this] {
    if(m_bankFocus)
      Application::get().getHWUI()->setFocusAndMode({ UIFocus::Presets, UIMode::Unchanged, UIDetail::Unchanged });
    else
      Application::get().getHWUI()->setFocusAndMode({ UIFocus::Banks, UIMode::Unchanged, UIDetail::Unchanged });
  };

  m_buttonAHandler = std::make_unique<ShortVsLongPress>(toggleBankFocus, toggleBankFocus);
}

void BankButton::installDual()
{
  auto focusAndMode = Application::get().getHWUI()->getFocusAndMode();
  auto bankbutton = addControl(new Button("Bank", { 4, 0, 50, 11 }));
  bankbutton->setHighlight(focusAndMode.focus == UIFocus::Banks);

  addControl(new Button("I / II", { 0, 15, 58, 11 }));

  auto toggleBankFocus = [this] {
    if(m_bankFocus)
      Application::get().getHWUI()->setFocusAndMode({ UIFocus::Presets, UIMode::Unchanged, UIDetail::Unchanged });
    else
      Application::get().getHWUI()->setFocusAndMode({ UIFocus::Banks, UIMode::Unchanged, UIDetail::Unchanged });
  };

  m_buttonAHandler = std::make_unique<ShortVsLongPress>(
      [this] { Application::get().getHWUI()->toggleCurrentVoiceGroup(); }, toggleBankFocus);
}
