#include <catch.hpp>
#include <proxies/hwui/debug-oled/DebugLayout.h>
#include "testing/TestHelper.h"
#include "proxies/hwui/HWUI.h"
#include <device-settings/LayoutMode.h>

inline bool currentLayoutNotExceptionLayout()
{
  auto currentLayout = Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().getLayout();
  return dynamic_cast<DebugLayout *>(currentLayout.get()) == nullptr;
}

inline void traverseFocusAndMode()
{
  forEachValue<UIFocus>([](auto focus) {
    forEachValue<UIMode>([=](auto mode) {
      forEachValue<UIDetail>([=](auto detail) {
        Application::get().getHWUI()->setFocusAndMode({ focus, mode, detail });
        REQUIRE(currentLayoutNotExceptionLayout());
      });
    });
  });
}

TEST_CASE("Mixed - HWUI has no Exception Screen on FocusAndMode Traversal")
{
  auto layoutMode = Application::get().getSettings()->getSetting<LayoutMode>();
  layoutMode->set(LayoutVersionMode::Mixed);
  traverseFocusAndMode();
}

TEST_CASE("Old - HWUI does not crash on FocusAndMode Traversal")
{
  auto layoutMode = Application::get().getSettings()->getSetting<LayoutMode>();
  layoutMode->set(LayoutVersionMode::Old);
  traverseFocusAndMode();
}