#include <third-party/include/catch.hpp>
#include <proxies/hwui/HWUI.h>
#include <Application.h>
#include <proxies/hwui/descriptive-layouts/GenericLayout.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/SingleSoundLayout.h>

TEST_CASE("Sound Focus Traversal leads to correct Sound Screens")
{
  auto hwui = Application::get().getHWUI();
  forEachValue<UIMode>([&](auto mode) {
      hwui->setFocusAndMode({UIFocus::Sound, mode, UIDetail::Any});
      auto layout = hwui->getPanelUnit().getEditPanel().getBoled().getLayout();
      auto genLayout = dynamic_cast<DescriptiveLayouts::GenericLayout*>(layout.get());
      REQUIRE(genLayout);
      REQUIRE_FALSE(dynamic_cast<SingleSoundLayout*>(layout.get())); //Old Sound Screen
  });
}