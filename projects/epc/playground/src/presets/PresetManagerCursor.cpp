#include "PresetManagerCursor.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/Bank.h"
#include "presets/Preset.h"

PresetManagerCursor::PresetManagerCursor(std::function<bool(const Preset*)> matcher)
    : m_matcher(matcher)
    , m_preset(Uuid::none())
    , m_bank(Uuid::none())

{
  moveToSelected();
}

void PresetManagerCursor::moveToSelected()
{
  auto pm = Application::get().getPresetManager();
  m_bank = pm->getSelectedBankUuid();

  if(auto b = getBank())
  {
    m_preset = b->getSelectedPresetUuid();

    if(!m_matcher(getPreset()))
    {
      if(!previousPreset())
        nextPreset();
    }
  }
}

bool PresetManagerCursor::advanceBank(int i)
{
  auto pm = Application::get().getPresetManager();

  m_preset = Uuid::none();
  auto bank = std::exchange(m_bank, Uuid::none());

  while(auto b = pm->findBank(bank))
  {
    int pos = static_cast<int>(pm->getBankPosition(b->getUuid())) + i;

    if(pos >= 0 && pos < static_cast<int>(pm->getNumBanks()))
    {
      auto newBank = pm->getBankAt(static_cast<size_t>(pos));
      bank = newBank->getUuid();

      if(newBank->getNumPresets() == 0)
      {
        m_bank = bank;
        return true;
      }

      if(auto p = (i > 0) ? findFirstMatchingPreset(newBank) : findLastMatchingPreset(newBank))
      {
        m_bank = bank;
        m_preset = p->getUuid();
        return true;
      }
    }
    else
    {
      return false;
    }
  }
  return false;
}

bool PresetManagerCursor::advancePreset(int dir)
{
  auto pm = Application::get().getPresetManager();
  auto preset = std::exchange(m_preset, Uuid::none());

  if(auto p = pm->findPreset(preset))
  {
    auto b = static_cast<Bank*>(p->getParent());
    auto pos = static_cast<int>(b->getPresetPosition(p));
    auto num = static_cast<int>(b->getNumPresets());

    for(int i = pos + dir; i >= 0 && i < num; i += dir)
    {
      auto newP = b->getPresetAt(static_cast<size_t>(i));
      if(m_matcher(newP))
      {
        m_preset = newP->getUuid();
        return true;
      }
    }
  }
  return false;
}

bool PresetManagerCursor::nextBank()
{
  return advanceBank(1);
}

bool PresetManagerCursor::previousBank()
{
  return advanceBank(-1);
}

bool PresetManagerCursor::canPreviousBank() const
{
  auto cp = *this;
  return cp.previousBank();
}

bool PresetManagerCursor::canNextBank() const
{
  auto cp = *this;
  return cp.nextBank();
}

bool PresetManagerCursor::nextPreset()
{
  return advancePreset(1);
}

bool PresetManagerCursor::canNextPreset() const
{
  auto cp = *this;
  return cp.nextPreset();
}

bool PresetManagerCursor::previousPreset()
{
  return advancePreset(-1);
}

bool PresetManagerCursor::canPreviousPreset() const
{
  auto cp = *this;
  return cp.previousPreset();
}

Preset* PresetManagerCursor::getPreset() const
{
  auto pm = Application::get().getPresetManager();
  return pm->findPreset(m_preset);
}

Uuid PresetManagerCursor::getPresetUuid() const
{
  return m_preset;
}

Bank* PresetManagerCursor::getBank() const
{
  auto pm = Application::get().getPresetManager();
  return pm->findBank(m_bank);
}

Uuid PresetManagerCursor::getBankUuid() const
{
  return m_bank;
}

std::string PresetManagerCursor::getPresetNumberString() const
{
  if(auto p = getPreset())
  {
    auto b = static_cast<Bank*>(p->getParent());
    char txt[256];
    sprintf(txt, "%03zu", b->getPresetPosition(p) + 1);
    return txt;
  }
  return {};
}

std::string PresetManagerCursor::getPresetName() const
{
  if(auto p = getPreset())
    return p->getName();

  return {};
}

std::string PresetManagerCursor::getPresetType() const
{
  if(auto p = getPreset())
  {
    switch(p->getType())
    {
      case SoundType::Single:
        return "";
      case SoundType::Split:
        return "||";
      case SoundType::Layer:
        return "=";
      case SoundType::Invalid:
        return "X";
    }
  }
  g_assert_not_reached();
}

std::string PresetManagerCursor::getBankName() const
{
  if(auto b = getBank())
    return b->getName(true);

  return {};
}

Preset* PresetManagerCursor::findFirstMatchingPreset(Bank* bank) const
{
  auto numPresets = bank->getNumPresets();

  for(size_t i = 0; i < numPresets; i++)
  {
    auto p = bank->getPresetAt(i);
    if(m_matcher(p))
      return p;
  }
  return nullptr;
}

Preset* PresetManagerCursor::findLastMatchingPreset(Bank* bank) const
{
  auto numPresets = bank->getNumPresets();

  for(size_t i = numPresets; i > 0; i--)
  {
    auto p = bank->getPresetAt(i - 1);
    if(m_matcher(p))
      return p;
  }
  return nullptr;
}
