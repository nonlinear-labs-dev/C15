#include "LoadToPartPresetList.h"
#include <Application.h>
#include <presets/PresetManager.h>
#include <libundo/undo/Scope.h>
#include <libundo/undo/TransactionCreationScope.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>
#include <device-settings/DirectLoadSetting.h>
#include <device-settings/Settings.h>
#include <presets/Bank.h>
#include <presets/Preset.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetListHeader.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetListContent.h>
#include <presets/PresetPartSelection.h>

LoadToPartPresetList::LoadToPartPresetList(const Rect& pos, bool showBankArrows, PresetPartSelection* partSelectionI,
                                           PresetPartSelection* partSelectionII)
    : PresetListBase(pos, showBankArrows)
    , m_selections { partSelectionI, partSelectionII }
{
  m_voiceGroupConnection = Application::get().getHWUI()->onCurrentVoiceGroupChanged(
      sigc::hide(sigc::mem_fun(this, &LoadToPartPresetList::onVoiceGroupChanged)));
}

LoadToPartPresetList::~LoadToPartPresetList()
{
  m_voiceGroupConnection.disconnect();
}

bool LoadToPartPresetList::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  if(down)
  {
    auto focusAndMode = Application::get().getHWUI()->getFocusAndMode();
    auto pm = Application::get().getPresetManager();
    auto selection = getCurrentSelection();

    switch(i)
    {
      case Buttons::BUTTON_B:
        if(focusAndMode.focus == UIFocus::Presets)
        {
          selectPreviousBank(pm);
        }
        else
        {
          if(selection && selection->m_bank)
            selectPrevious();
        }
        return true;

      case Buttons::BUTTON_C:
        if(focusAndMode.focus == UIFocus::Presets)
        {
          selectNextBank(pm);
        }
        else
        {
          if(selection && selection->m_bank)
            selectNext();
        }
        return true;

      case Buttons::BUTTON_ENTER:
        onEnterButtonPressed();
        return true;
    }
  }

  return false;
}

std::pair<size_t, size_t> LoadToPartPresetList::getSelectedPosition() const
{
  auto pm = Application::get().getPresetManager();
  if(auto selection = getCurrentSelection())
  {

    return std::pair<size_t, size_t>(pm->getBankPosition(selection->m_bank->getUuid()),
                                     selection->m_bank->getPresetPosition(selection->m_preset));
  }

  return { -1, -1 };
}

void LoadToPartPresetList::onRotary(int inc, ButtonModifiers modifiers)
{
  auto inBankMode = Application::get().getHWUI()->getFocusAndMode().focus == UIFocus::Banks;
  auto pm = Application::get().getPresetManager();

  if(inBankMode)
  {
    while(inc > 0)
    {
      selectNextBank(pm);
      inc--;
    }

    while(inc < 0)
    {
      selectPreviousBank(pm);
      inc++;
    }
  }
  else
  {
    while(inc > 0)
    {
      selectNext();
      inc--;
    }

    while(inc < 0)
    {
      selectPrevious();
      inc++;
    }
  }
}

void LoadToPartPresetList::update(const PresetManager* pm)
{
  if(auto selection = getCurrentSelection())
  {
    if(selection->m_bank)
    {
      if(auto selectedBank = pm->findBank(selection->m_bank->getUuid()))
      {
        if(auto selectedPreset = selection->m_preset)
        {
          m_content->setup(selectedBank, selectedBank->getPresetPosition(selectedPreset));
        }
        m_header->setup(selectedBank);
      }
    }
  }
}

void LoadToPartPresetList::selectNext()
{
  if(auto selection = getCurrentSelection())
  {
    selection->selectNextPresetPart();
    onSelectionChanged(*selection);
    update(Application::get().getPresetManager());
  }
}

void LoadToPartPresetList::selectPrevious()
{
  if(auto selection = getCurrentSelection())
  {
    selection->selectPreviousPresetPart();
    onSelectionChanged(*selection);
    update(Application::get().getPresetManager());
  }
}

void LoadToPartPresetList::selectPreviousBank(PresetManager* pm)
{
  if(auto selection = getCurrentSelection())
  {
    selection->selectPreviousBank();
    onSelectionChanged(*selection);
    update(Application::get().getPresetManager());
  }
}

void LoadToPartPresetList::selectNextBank(PresetManager* pm)
{
  if(auto selection = getCurrentSelection())
  {
    selection->selectNextBank();
    onSelectionChanged(*selection);
    update(Application::get().getPresetManager());
  }
}

void LoadToPartPresetList::onSelectionChanged(const PresetPartSelection& selection)
{
  if(Application::get().getSettings()->getSetting<DirectLoadSetting>()->get())
  {
    auto currentVg = Application::get().getHWUI()->getCurrentVoiceGroup();
    EditBufferUseCases useCase(Application::get().getPresetManager()->getEditBuffer());
    useCase.undoableLoadToPart(selection.m_preset, selection.m_voiceGroup, currentVg);
  }
}

void LoadToPartPresetList::onEnterButtonPressed()
{
  if(const auto selection = getCurrentSelection())
  {
    EditBufferUseCases ebUseCases(Application::get().getPresetManager()->getEditBuffer());
    auto eb = Application::get().getPresetManager()->getEditBuffer();
    const auto currentVG = Application::get().getHWUI()->getCurrentVoiceGroup();
    auto oldPartInGroup = eb->getPartOrigin(currentVG);
    ebUseCases.undoableLoadToPart(selection->m_preset, selection->m_voiceGroup, currentVG);

    if(oldPartInGroup.presetUUID == selection->m_preset->getUuid())
      animateSelectedPreset([=] {});
  }
}

void LoadToPartPresetList::onVoiceGroupChanged()
{
  auto pm = Application::get().getPresetManager();
  update(pm);
}

PresetPartSelection* LoadToPartPresetList::getCurrentSelection()
{
  auto vg = Application::get().getHWUI()->getCurrentVoiceGroup();
  return m_selections[static_cast<int>(vg)];
}

const PresetPartSelection* LoadToPartPresetList::getCurrentSelection() const
{
  auto vg = Application::get().getHWUI()->getCurrentVoiceGroup();
  return m_selections[static_cast<int>(vg)];
}

void LoadToPartPresetList::doEnterAction()
{
  onEnterButtonPressed();
}

auto getEditBuffer()
{
  return Application::get().getPresetManager()->getEditBuffer();
}
