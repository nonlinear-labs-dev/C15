#include "PresetListContent.h"
#include "PresetListEntry.h"
#include "Application.h"
#include <presets/Bank.h>

#include <utility>
#include "presets/PresetManager.h"
#include "EmptyBankLabel.h"

PresetListContent::PresetListContent(const Rect &pos)
    : super(pos)
{
}

PresetListContent::~PresetListContent() = default;

bool PresetListContent::animateSelectedPreset(std::function<void()> cb)
{
  if(m_secondPreset)
  {
    m_secondPreset->animate(std::move(cb));
    return true;
  }
  return false;
}

bool PresetListContent::isTransparent() const
{
  return true;
}

Preset *PresetListContent::getPresetAtPosition(Bank *bank, int pos) const
{
  if(pos >= 0 && pos < bank->getNumPresets())
    return bank->getPresetAt(pos);
  return nullptr;
}

void PresetListContent::setup(Bank *bank, size_t focussedPresetPos)
{
  auto hasBank = bank != nullptr;
  auto hasPresets = bank && bank->getNumPresets();
  auto hasEmptyBankLabel = dynamic_cast<EmptyBankLabel *>(m_emptyLabel);
  auto hasNoBankLabel = m_emptyLabel && m_emptyLabel->getText().text == "no bank";

  if(hasBank && hasPresets)
  {
    if(!m_firstPreset)
    {
      clear();

      m_emptyLabel = nullptr;
      m_firstPreset = addControl(new PresetListEntry(Rect(0, 0, 128, 16)));
      m_secondPreset = addControl(new PresetListEntry(Rect(0, 16, 128, 16)));
      m_thirdPreset = addControl(new PresetListEntry(Rect(0, 32, 128, 16)));
    }

    m_firstPreset->setPreset(getPresetAtPosition(bank, static_cast<int>(focussedPresetPos - 1)), false);
    m_secondPreset->setPreset(getPresetAtPosition(bank, static_cast<int>(focussedPresetPos)), true);
    m_thirdPreset->setPreset(getPresetAtPosition(bank, static_cast<int>(focussedPresetPos + 1)), false);
  }
  else if((hasBank && !hasEmptyBankLabel) || (!hasBank && !hasNoBankLabel))
  {
    clear();
    m_emptyLabel = addControl(new EmptyBankLabel(Rect(0, 16, 128, 16)));

    m_firstPreset = nullptr;
    m_secondPreset = nullptr;
    m_thirdPreset = nullptr;
  }
}
