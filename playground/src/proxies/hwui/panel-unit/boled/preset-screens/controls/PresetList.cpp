#include <Application.h>
#include <presets/Bank.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/buttons.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetList.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetListContent.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetListHeader.h>
#include <memory>

PresetList::PresetList(const Rect &pos, bool showBankArrows)
    : super(pos, showBankArrows)
{
  Application::get().getPresetManager()->onBankSelection(mem_fun(this, &PresetList::onBankSelectionChanged));
  Application::get().getPresetManager()->onRestoreHappened(mem_fun(this, &PresetList::onBankChanged));
}

PresetList::~PresetList()
{
}

void PresetList::onBankSelectionChanged()
{
  m_bankChangedConnection.disconnect();

  if(auto bank = Application::get().getPresetManager()->getSelectedBank())
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

    if(auto p = bank->getSelectedPreset())
      m_content->setup(bank, bank->getPresetPosition(p));
    else
      m_content->setup(bank, size_t(-1));
  }
  else
  {
    m_header->setup(nullptr);
    m_content->setup(nullptr, size_t(-1));
  }
}

bool PresetList::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  if(down)
  {
    auto focusAndMode = Application::get().getHWUI()->getFocusAndMode();
    auto pm = Application::get().getPresetManager();

    switch(i)
    {
      case Buttons::BUTTON_B:
        if(focusAndMode.focus == UIFocus::Banks)
        {
          if(auto bank = pm->getSelectedBank())
            bank->selectPreviousPreset();
        }
        else
        {
          pm->selectPreviousBank();
        }
        return true;

      case Buttons::BUTTON_C:
        if(focusAndMode.focus == UIFocus::Banks)
        {
          if(auto bank = pm->getSelectedBank())
            bank->selectNextPreset();
        }
        else
        {
          pm->selectNextBank();
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
    auto scope = pm->getUndoScope().startTransaction("Select Bank");

    if(modifiers[SHIFT] && pm->getNumBanks() > 0)
    {
      if(inc < 0)
        pm->selectBank(scope->getTransaction(), pm->getBanks().front()->getUuid());
      else
        pm->selectBank(scope->getTransaction(), pm->getBanks().back()->getUuid());
    }
    else
    {
      while(inc < 0)
      {
        pm->selectPreviousBank(scope->getTransaction());
        inc++;
      }

      while(inc > 0)
      {
        pm->selectNextBank(scope->getTransaction());
        inc--;
      }
    }
  }
  else if(auto bank = pm->getSelectedBank())
  {
    auto scope = pm->getUndoScope().startTransaction("Select Preset");
    while(inc < 0)
    {
      bank->selectPreviousPreset(scope->getTransaction());
      inc++;
    }

    while(inc > 0)
    {
      bank->selectNextPreset(scope->getTransaction());
      inc--;
    }
  }
}

std::pair<size_t, size_t> PresetList::getSelectedPosition() const
{
  auto pm = Application::get().getPresetManager();

  if(auto b = pm->getSelectedBank())
  {
    auto bankPos = pm->getBankPosition(b->getUuid());
    auto presetPos = b->getPresetPosition(b->getSelectedPreset());
    return std::make_pair(bankPos, presetPos);
  }
  return { -1, -1 };
}
