#include "PresetListContent.h"
#include "PresetListEntry.h"
#include "Application.h"
#include "presets/PresetBank.h"
#include "presets/PresetManager.h"
#include "EmptyBankLabel.h"

PresetListContent::PresetListContent(const Rect &pos)
    : super(pos)
{
}

PresetListContent::~PresetListContent()
{
}

bool PresetListContent::animateSelectedPreset(function<void()> cb)
{
  if(m_secondPreset)
  {
    m_secondPreset->animate(cb);
    return true;
  }
  return false;
}

shared_ptr<Preset> PresetListContent::getPresetAtPosition(shared_ptr<PresetBank> bank, int pos) const
{
  if(pos >= 0 && pos < bank->getNumPresets())
    return bank->getPreset(pos);
  return nullptr;
}

void PresetListContent::setup(shared_ptr<PresetBank> bank, int focussedPresetPos)
{
  if(bank && bank->getNumPresets())
  {
    if(!m_firstPreset)
    {
      clear();

      m_emptyLabel = nullptr;
      m_firstPreset = addControl(new PresetListEntry(Rect(0, 0, 126, 16)));
      m_secondPreset = addControl(new PresetListEntry(Rect(0, 16, 126, 16)));
      m_thirdPreset = addControl(new PresetListEntry(Rect(0, 32, 126, 16)));
    }

    m_firstPreset->setPreset(getPresetAtPosition(bank, focussedPresetPos - 1), false);
    m_secondPreset->setPreset(getPresetAtPosition(bank, focussedPresetPos), true);
    m_thirdPreset->setPreset(getPresetAtPosition(bank, focussedPresetPos + 1), false);
  }
  else if(bank == nullptr || !m_emptyLabel)
  {
    clear();
    if(bank == nullptr)
      m_emptyLabel = addControl(new Label("no bank", Rect(0, 16, 126, 16)));
    else
      m_emptyLabel = addControl(new EmptyBankLabel(Rect(0, 16, 126, 16)));

    m_firstPreset = nullptr;
    m_secondPreset = nullptr;
    m_thirdPreset = nullptr;
  }
}
