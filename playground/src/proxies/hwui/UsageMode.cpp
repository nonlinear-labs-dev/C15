#include "UsageMode.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "device-settings/DebugLevel.h"

UsageMode::UsageMode()
{
}

UsageMode::~UsageMode()
{
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
    m_actions[buttonID] = [action](auto b, auto m, auto d) -> bool {
      auto ret = action(b,m,d);
      if(d) {
        std::cerr << "Button " << toString(b) << " down at:\t" << std::chrono::high_resolution_clock::now().time_since_epoch().count() << std::endl;
        Application::get().setProfilingFlag(true);
      }
      return ret;
    };
  }
}

void UsageMode::setupFocusAndMode(FocusAndMode focusAndMode)
{
}

void UsageMode::bruteForceUpdateLeds()
{
}
