#include <glib.h>
#include "EditPanel.h"

EditPanel::EditPanel()
{
  getKnob().onRotaryChanged(sigc::mem_fun(&m_oled, &BOLED::onRotary));
}

EditPanel::~EditPanel()
{
}

void EditPanel::init()
{
  getBoled().init();
}

void EditPanel::setupFocusAndMode(FocusAndMode focusAndMode)
{
  m_oled.setupFocusAndMode(focusAndMode);
}

void EditPanel::onTimeout()
{
  m_oled.redraw();
}

RotaryEncoder &EditPanel::getKnob()
{
  return m_knob;
}

BOLED &EditPanel::getBoled()
{
  return m_oled;
}

const BOLED &EditPanel::getBoled() const
{
  return m_oled;
}

bool EditPanel::onButtonPressed(Buttons buttonID, ButtonModifiers modifiers, bool state)
{
  return m_oled.onButtonPressed(buttonID, modifiers, state);
}

UndoButtonStateMachine &EditPanel::getUndoStateMachine()
{
  return m_statemachine;
}
