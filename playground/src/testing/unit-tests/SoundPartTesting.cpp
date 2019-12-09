#include "third-party/include/catch.hpp"
#include "testing/TestHelper.h"
#include "MockPresetStorage.h"
#include <presets/Bank.h>
#include <presets/Preset.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>

TEST_CASE("HWUI Voice Group Selection")
{
  MockPresetStorage presets;

  auto hwui = Application::get().getHWUI();

  auto eb = TestHelper::getEditBuffer();
  {
    auto scope = TestHelper::createTestScope();
    eb->undoableLoad(scope->getTransaction(), presets.getLayerPreset());
    hwui->setCurrentVoiceGroup(VoiceGroup::II);
    REQUIRE(hwui->getCurrentVoiceGroup() == VoiceGroup::II);
  }

  SECTION("Reset Voice Group Selection to I on Preset Load")
  {
    {
      auto scope = TestHelper::createTestScope();
      REQUIRE(hwui->getCurrentVoiceGroup() == VoiceGroup::II);
      eb->undoableLoad(scope->getTransaction(), presets.getSplitPreset());
    }

    REQUIRE(hwui->getCurrentVoiceGroup() == VoiceGroup::I);
  }
}