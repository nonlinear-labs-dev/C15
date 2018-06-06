#include <Application.h>
#include <presets/PresetBank.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/buttons.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetListContent.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetListHeader.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetListSelectStorePosition.h>
#include <algorithm>

PresetListSelectStorePosition::PresetListSelectStorePosition(const Rect &pos, bool showBankArrows) :
    super(pos, showBankArrows)
{
  initBankAndPreset();
}

PresetListSelectStorePosition::~PresetListSelectStorePosition()
{
}

void PresetListSelectStorePosition::initBankAndPreset()
{
  auto pm = Application::get().getPresetManager();

  if(auto bank = pm->getSelectedBank())
  {
    m_bankPosition = pm->calcBankIndex(bank.get());
    m_selectedPreset = bank->getSelectedPreset();
    m_presetPosition = bank->getPresetPosition(m_selectedPreset);
    onChange();
  }
}

void PresetListSelectStorePosition::onChange()
{
  m_bankConnection.disconnect();

  if(auto bank = Application::get().getPresetManager()->getBank(m_bankPosition))
  {
    m_bankConnection = bank->onBankChanged(mem_fun(this, &PresetListSelectStorePosition::onBankChanged));
  }
}

void PresetListSelectStorePosition::onBankChanged()
{
  if(auto bank = Application::get().getPresetManager()->getBank(m_bankPosition))
  {
    if(m_selectedPreset != bank->getSelectedPreset())
    {
      m_selectedPreset = bank->getSelectedPreset();
      m_presetPosition = bank->getPresetPosition(m_selectedPreset);
    }

    sanitizePresetPosition(bank);
    m_content->setup(bank, m_presetPosition);
    m_header->setup(bank);
  }
}

std::pair<int, int> PresetListSelectStorePosition::getSelectedPosition() const
{
  return
  { m_bankPosition, m_presetPosition};
}

bool PresetListSelectStorePosition::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  auto focusAndMode = Application::get().getHWUI()->getFocusAndMode();

  if(down)
  {
    switch(i)
    {
      case Buttons::BUTTON_B:
        if(focusAndMode.focus == UIFocus::Banks)
        {
          movePresetSelection(-1);
        }
        else
        {
          moveBankSelection(-1);
        }
        return true;

      case Buttons::BUTTON_C:
        if(focusAndMode.focus == UIFocus::Banks)
        {
          movePresetSelection(1);
        }
        else
        {
          moveBankSelection(1);
        }
        return true;
    }
  }
  return false;
}

void PresetListSelectStorePosition::onRotary(int inc, ButtonModifiers modifiers)
{
  auto focusAndMode = Application::get().getHWUI()->getFocusAndMode();

  if(focusAndMode.focus == UIFocus::Presets)
    movePresetSelection(inc);
  else
    moveBankSelection(inc);
}

void PresetListSelectStorePosition::movePresetSelection(int moveBy)
{
  auto pm = Application::get().getPresetManager();

  sanitizeBankPosition(pm);

  if(auto bank = pm->getBank(m_bankPosition))
  {
    m_presetPosition += moveBy;
    sanitizePresetPosition(bank);
  }

  onChange();
  setDirty();
}

void PresetListSelectStorePosition::sanitizeBankPosition(std::shared_ptr<PresetManager> pm)
{
  if(auto numBanks = pm->getNumBanks())
  {
    m_bankPosition = std::min<int>(m_bankPosition, numBanks - 1);
    m_bankPosition = std::max<int>(m_bankPosition, 0);
    return;
  }

  m_bankPosition = invalidIndex;
}

void PresetListSelectStorePosition::sanitizePresetPosition(std::shared_ptr<PresetBank> bank)
{
  if(bank)
  {
    if(auto numPresets = bank->getNumPresets())
    {
      m_presetPosition = std::min<int>(m_presetPosition, numPresets - 1);
      m_presetPosition = std::max<int>(m_presetPosition, 0);
      return;
    }
  }

  m_presetPosition = invalidIndex;
}

void PresetListSelectStorePosition::moveBankSelection(int moveBy)
{
  auto pm = Application::get().getPresetManager();
  m_bankPosition += moveBy;

  sanitizeBankPosition(pm);
  sanitizePresetPosition(pm->getBank(m_bankPosition));

  onChange();
  setDirty();
}
