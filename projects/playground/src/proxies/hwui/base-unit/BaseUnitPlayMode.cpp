#include "BaseUnitPlayMode.h"
#include "Application.h"
#include "device-settings/Settings.h"
#include "device-settings/BaseUnitUIMode.h"
#include "proxies/lpc/LPCProxy.h"
#include "groups/HardwareSourcesGroup.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include <parameters/RibbonParameter.h>
#include <proxies/hwui/buttons.h>
#include <http/UndoScope.h>

BaseUnitPlayMode::BaseUnitPlayMode()
    : m_modeButtonHandler(std::bind(&BaseUnitPlayMode::modeButtonShortPress, this),
                          std::bind(&BaseUnitPlayMode::modeButtonLongPress, this))
{
}

void BaseUnitPlayMode::setup()
{
  setupBaseUnitUIModeButton();
  setupBaseUnitMinusButton();
  setupBaseUnitPlusButton();

  setupButtonConnection(Buttons::BUTTON_FUNCTION, [=](auto, auto, auto state) {
    if(state)
      toggleTouchBehaviour();

    return true;
  });
}

void BaseUnitPlayMode::toggleTouchBehaviour()
{
  if(auto pm = Application::get().getPresetManager())
  {
    auto trans = pm->getUndoScope().startTransaction("Set ribbon mode");
    auto id = Application::get().getLPCProxy()->getLastTouchedRibbonParameterID();

    if(auto ribbonParam
       = dynamic_cast<RibbonParameter*>(pm->getEditBuffer()->findParameterByID({ id, VoiceGroup::Global })))
      ribbonParam->undoableIncRibbonTouchBehaviour(trans->getTransaction());
  }
}

void BaseUnitPlayMode::setupBaseUnitUIModeButton()
{
  setupButtonConnection(Buttons::BUTTON_MODE, [=](auto, auto, auto state) {
    m_modeButtonHandler.onButtonEvent(state);
    return true;
  });
}

void BaseUnitPlayMode::modeButtonShortPress()
{
  auto s = Application::get().getSettings()->getSetting<BaseUnitUIMode>();

  if(s->get() == BaseUnitUIModes::Play)
    s->set(BaseUnitUIModes::ParameterEdit);
  else
    s->set(BaseUnitUIModes::Play);
}

void BaseUnitPlayMode::modeButtonLongPress()
{
  Application::get().getSettings()->getSetting<BaseUnitUIMode>()->set(BaseUnitUIModes::Presets);
}

void BaseUnitPlayMode::setupBaseUnitMinusButton()
{
  setupButtonConnection(Buttons::BUTTON_MINUS, [=](auto, auto, auto state) {
    if(state)
      m_noteShiftState.traverse(NoteShiftEvents::NOTE_SHIFT_EVENT_MINUS_PRESSED);
    else
      m_noteShiftState.traverse(NoteShiftEvents::NOTE_SHIFT_EVENT_MINUS_RELEASED);

    return true;
  });
}

void BaseUnitPlayMode::setupBaseUnitPlusButton()
{
  setupButtonConnection(Buttons::BUTTON_PLUS, [=](auto, auto, auto state) {
    if(state)
      m_noteShiftState.traverse(NoteShiftEvents::NOTE_SHIFT_EVENT_PLUS_PRESSED);
    else
      m_noteShiftState.traverse(NoteShiftEvents::NOTE_SHIFT_EVENT_PLUS_RELEASED);

    return true;
  });
}
