#include "SelectVoiceGroupLayout.h"
#include <Application.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/panel-unit/boled/BOLED.h>
#include <proxies/hwui/controls/Label.h>
#include <proxies/hwui/controls/Button.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include "PresetManagerLayout.h"

SelectVoiceGroupLayout::SelectVoiceGroupLayout(PresetManagerLayout *parent)
    : Layout(Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled())
    , m_parent { parent }
{
  addControl(new Label("Load Part I or II from Preset?", { 0, 0, 256, 64 }));
  addControl(new Button("Cancel", Buttons::BUTTON_A));
  m_I = addControl(new Button("I", Buttons::BUTTON_B));
  m_II = addControl(new Button("II", Buttons::BUTTON_C));
  addControl(new Button("Load", Buttons::BUTTON_D));

  m_I->setHighlight(true);
}

bool SelectVoiceGroupLayout::onButton(Buttons i, bool down, ::ButtonModifiers modifiers)
{
  if(down)
  {
    switch(i)
    {
      case Buttons::BUTTON_A:
        cancel();
        return true;
      case Buttons::BUTTON_B:
      case Buttons::BUTTON_C:
        toggleSelection();
        return true;
      case Buttons::BUTTON_D:
        load();
        return true;
    }
  }

  return Layout::onButton(i, down, modifiers);
}

void SelectVoiceGroupLayout::toggleSelection()
{
  if(m_loadFrom == VoiceGroup::I)
    m_loadFrom = VoiceGroup::II;
  else
    m_loadFrom = VoiceGroup::I;

  m_I->setHighlight(m_loadFrom == VoiceGroup::I);
  m_II->setHighlight(m_loadFrom == VoiceGroup::II);
}

void SelectVoiceGroupLayout::load()
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  auto hwui = Application::get().getHWUI();
  auto currentVG = Application::get().getHWUI()->getCurrentVoiceGroup();
  auto from = m_loadFrom;

  eb->undoableLoadSelectedToPart(from, currentVG);

  m_parent->animateSelectedPresetIfInLoadPartMode([hwui]() {
    hwui->setFocusAndMode({ UIFocus::Presets, UIMode::Select, UIDetail::Init });
  });

  cancel();
}

void SelectVoiceGroupLayout::cancel()
{
  Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().resetOverlay();
}
