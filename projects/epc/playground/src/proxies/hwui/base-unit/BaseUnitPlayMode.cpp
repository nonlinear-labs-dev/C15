#include "BaseUnitPlayMode.h"
#include "Application.h"
#include "device-settings/Settings.h"
#include "device-settings/BaseUnitUIMode.h"
#include "proxies/playcontroller/PlaycontrollerProxy.h"
#include "groups/HardwareSourcesGroup.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include <parameters/RibbonParameter.h>
#include <proxies/hwui/buttons.h>
#include <http/UndoScope.h>
#include <use-cases/RibbonParameterUseCases.h>
#include <use-cases/SettingsUseCases.h>

BaseUnitPlayMode::BaseUnitPlayMode()
    : m_modeButtonHandler(std::bind(&BaseUnitPlayMode::modeButtonShortPress, this),
                          std::bind(&BaseUnitPlayMode::modeButtonLongPress, this))
{
  for(auto b : { Buttons::BUTTON_MINUS, Buttons::BUTTON_PLUS, Buttons::BUTTON_MODE, Buttons::BUTTON_FUNCTION })
    m_buttonStates.emplace(b, false);
}

void BaseUnitPlayMode::setup()
{
  setupBaseUnitUIModeButton();
  setupBaseUnitMinusButton();
  setupBaseUnitPlusButton();

  setupButtonConnection(Buttons::BUTTON_FUNCTION, [=](auto, auto, auto state) {
    if(checkPanicAffenGriff(Buttons::BUTTON_FUNCTION, state))
      return true;

    if(state)
      toggleTouchBehaviour();

    return true;
  });
}

void BaseUnitPlayMode::toggleTouchBehaviour()
{
  if(auto pm = Application::get().getPresetManager())
  {
    auto eb = pm->getEditBuffer();
    auto id = Application::get().getPlaycontrollerProxy()->getLastTouchedRibbonParameterID();

    if(auto ribbonParam = eb->findAndCastParameterByID<RibbonParameter>({ id, VoiceGroup::Global }))
    {
      RibbonParameterUseCases useCase(ribbonParam);
      useCase.incTouchBehaviour();
    }
  }
}

void BaseUnitPlayMode::setupBaseUnitUIModeButton()
{
  setupButtonConnection(Buttons::BUTTON_MODE, [=](auto, auto, auto state) {
    if(checkPanicAffenGriff(Buttons::BUTTON_MODE, state))
      return true;

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
    if(checkPanicAffenGriff(Buttons::BUTTON_MINUS, state))
      return true;

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
    if(checkPanicAffenGriff(Buttons::BUTTON_PLUS, state))
      return true;

    if(state)
      m_noteShiftState.traverse(NoteShiftEvents::NOTE_SHIFT_EVENT_PLUS_PRESSED);
    else
      m_noteShiftState.traverse(NoteShiftEvents::NOTE_SHIFT_EVENT_PLUS_RELEASED);

    return true;
  });
}

bool BaseUnitPlayMode::checkPanicAffenGriff(Buttons b, bool state)
{
  m_buttonStates[b] = state;
  if(std::all_of(m_buttonStates.cbegin(), m_buttonStates.cend(), [](auto x) { return x.second == true; }))
  {
    SettingsUseCases useCase(Application::get().getSettings());
    useCase.panicAudioEngine();
    return true;
  }
  return false;
}
