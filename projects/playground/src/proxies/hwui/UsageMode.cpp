#include <sigc++/adaptors/hide.h>
#include "UsageMode.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "device-settings/DebugLevel.h"
#include "proxies/hwui/HWUI.h"

UsageMode::UsageMode()
{
  m_voiceGroupChangedSignal = Application::get().getHWUI()->onCurrentVoiceGroupChanged(
      sigc::hide<0>(sigc::mem_fun(this, &UsageMode::bruteForceUpdateLeds)));
}

UsageMode::~UsageMode()
{
  m_voiceGroupChangedSignal.disconnect();
}

bool UsageMode::onButtonPressed(Buttons buttonID, ButtonModifiers modifiers, bool state)
{
  DebugLevel::info("button id:", (int) buttonID);

  const auto &it = m_actions.find(buttonID);

  if(it != m_actions.end())
    return it->second(buttonID, modifiers, state);

  return false;
}

void UsageMode::setupButtonConnection(Buttons buttonID, tAction action)
{
  if(buttonID < Buttons::NUM_BUTTONS)
  {
    g_assert(!m_actions[buttonID]);
    m_actions[buttonID] = action;
  }
}

void UsageMode::removeButtonConnection(Buttons buttonID)
{
  try
  {
    m_actions.erase(buttonID);
  }
  catch(...)
  {
    nltools::Log::error("Could not remove button sigc::connection for:", buttonID,
                        nltools::handle_eptr(std::current_exception()));
  }
}

void UsageMode::setupFocusAndMode(FocusAndMode focusAndMode)
{
}

void UsageMode::bruteForceUpdateLeds()
{
}
