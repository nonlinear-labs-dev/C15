#include <Application.h>
#include <presets/PresetBank.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/buttons.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetList.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetListContent.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetListHeader.h>
#include <memory>

PresetList::PresetList(const Rect &pos, bool showBankArrows) :
    super(pos, showBankArrows)
{
  Application::get().getPresetManager()->onBankSelection(mem_fun(this, &PresetList::onBankSelectionChanged));
  Application::get().getPresetManager()->onRestored(mem_fun(this, &PresetList::onRestore));
}

PresetList::~PresetList()
{
}

void PresetList::onBankSelectionChanged(shared_ptr<PresetBank> bank)
{
  m_bankChangedConnection.disconnect();

  if(bank)
  {
    m_bankChangedConnection = bank->onBankChanged(mem_fun(this, &PresetList::onBankChanged));
  }
  else
  {
    onBankChanged();
  }
}

void PresetList::onBankChanged()
{
  if(auto bank = Application::get().getPresetManager()->getSelectedBank())
  {
    m_header->setup(bank);
    m_content->setup(bank, bank->getPresetPosition(bank->getSelectedPreset()));
  }
  else
  {
    m_header->setup(nullptr);
    m_content->setup(nullptr, -1);
  }
}

bool PresetList::onButton(int i, bool down, ButtonModifiers modifiers)
{
  if(down)
  {
    auto focusAndMode = Application::get().getHWUI()->getFocusAndMode();
    auto pm = Application::get().getPresetManager();

    switch(i)
    {
      case BUTTON_B:
        if(focusAndMode.focus == UIFocus::Banks)
        {
          if(auto bank = pm->getSelectedBank())
            bank->undoableIncPresetSelection(-1, ButtonModifiers());
        }
        else if(auto bank = pm->getSelectedBank())
        {
          pm->undoableSelectPrevious();
        }
        return true;

      case BUTTON_C:
        if(focusAndMode.focus == UIFocus::Banks)
        {
          if(auto bank = pm->getSelectedBank())
            bank->undoableIncPresetSelection(1, ButtonModifiers());
        }
        else
        {
          pm->undoableSelectNext();
        }
        return true;
    }
  }

  return false;

}

void PresetList::onRotary(int inc, ButtonModifiers modifiers)
{
  auto focusAndMode = Application::get().getHWUI()->getFocusAndMode();
  auto pm = Application::get().getPresetManager();

  if(focusAndMode.focus == UIFocus::Banks)
  {
    if(modifiers[SHIFT]) {
      if(inc < 0) {
        pm->undoableSelectFirstBank();
      } else {
        pm->undoableSelectLastBank();
      }
    } else {
      while(inc < 0)
      {
        pm->undoableSelectPrevious();
        inc++;
      }

      while(inc > 0)
      {
        pm->undoableSelectNext();
        inc--;
      }
    }

  }
  else if(auto bank = pm->getSelectedBank())
  {
    bank->undoableIncPresetSelection(inc, modifiers);
  }
}

std::pair<int, int> PresetList::getSelectedPosition() const
{
  auto pm = Application::get().getPresetManager();

  if(auto b = pm->getSelectedBank())
  {
    auto bankPos = pm->calcBankIndex(b.get());
    auto presetPos = b->getPresetPosition(b->getSelectedPreset());
    return make_pair(bankPos, presetPos);

  }
  return
  { -1, -1};
}

void PresetList::onRestore() {
  onBankChanged();
}
