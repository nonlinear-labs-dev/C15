#include "PresetPartSelection.h"
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <presets/Preset.h>
#include <presets/Bank.h>
#include <Application.h>

PresetPartSelection::PresetPartSelection(VoiceGroup focus)
    : m_focusedVoiceGroup { focus }
{
  auto pm = Application::get().getPresetManager();
  auto eb = pm->getEditBuffer();

  if(auto bank = pm->getSelectedBank())
  {
    m_bank = bank;
    if(auto preset = bank->getSelectedPreset())
    {
      m_preset = preset;
      if(m_preset->isDual())
      {
        auto src = eb->getPartOrigin(m_focusedVoiceGroup).sourceGroup;
        m_voiceGroup = src != VoiceGroup::Global ? src : VoiceGroup::I;
      }
      else
        m_voiceGroup = VoiceGroup::I;
    }
  }
}

void PresetPartSelection::selectNextBank()
{
  auto pm = Application::get().getPresetManager();
  if(m_bank)
  {
    if(auto newBank = pm->getBankAt(pm->getBankPosition(m_bank->getUuid()) + 1))
    {
      m_bank = newBank;
      m_preset = m_bank->getPresetAt(0);
      m_voiceGroup = VoiceGroup::I;
    }
  }
}

void PresetPartSelection::selectPreviousBank()
{
  auto pm = Application::get().getPresetManager();
  if(m_bank)
  {
    if(auto newBank = pm->getBankAt(pm->getBankPosition(m_bank->getUuid()) - 1))
    {
      m_bank = newBank;
      m_preset = m_bank->getPresetAt(0);
      m_voiceGroup = VoiceGroup::I;
    }
  }
}

void PresetPartSelection::selectNextPresetPart()
{
  if(m_bank)
  {
    if(m_preset)
    {
      if(m_preset->isDual())
      {
        if(m_voiceGroup == VoiceGroup::I)
        {
          m_voiceGroup = VoiceGroup::II;
        }
        else if(m_voiceGroup == VoiceGroup::II)
        {
          if(auto newPreset = m_bank->getPresetAt(m_bank->getPresetPosition(m_preset) + 1))
          {
            m_preset = newPreset;
            m_voiceGroup = VoiceGroup::I;
          }
        }
      }
      else
      {
        if(auto newPreset = m_bank->getPresetAt(m_bank->getPresetPosition(m_preset) + 1))
        {
          m_preset = newPreset;
          m_voiceGroup = VoiceGroup::I;
        }
      }
    }
  }
}

void PresetPartSelection::selectPreviousPresetPart()
{
  if(m_bank)
  {
    if(m_preset)
    {
      if(m_preset->isDual())
      {
        if(m_voiceGroup == VoiceGroup::II)
        {
          m_voiceGroup = VoiceGroup::I;
        }
        else if(m_voiceGroup == VoiceGroup::I)
        {
          if(auto newPreset = m_bank->getPresetAt(m_bank->getPresetPosition(m_preset) - 1))
          {
            m_preset = newPreset;
            m_voiceGroup = m_preset->isDual() ? VoiceGroup::II : VoiceGroup::I;
          }
        }
      }
      else
      {
        if(auto newPreset = m_bank->getPresetAt(m_bank->getPresetPosition(m_preset) - 1))
        {
          m_preset = newPreset;
          m_voiceGroup = m_preset->isDual() ? VoiceGroup::II : VoiceGroup::I;
        }
      }
    }
  }
}
