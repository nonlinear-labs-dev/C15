#include <third-party/include/catch.hpp>
#include <testing/TestHelper.h>

#include <presets/EditBuffer.h>
#include <parameters/ModulateableParameter.h>
#include <parameters/RibbonParameter.h>
#include <proxies/audio-engine/AudioEngineProxy.h>
#include <nltools/Types.h>
#include <presets/Bank.h>
#include <presets/Preset.h>
#include <presets/PresetParameter.h>
#include <testing/unit-tests/mock/MockPresetStorage.h>

TEST_CASE("Load Single into Single", "[EditBuffer][Loading]")
{
  MockPresetStorage presets;
  auto preset = presets.getSinglePreset();
  auto globalValue = 0.25;

  for(auto val : std::vector<double> { 0.5 })
  {
    WHEN("Test with value: " + std::to_string(val))
    {
      auto eb = TestHelper::getEditBuffer();
      auto scope = TestHelper::createTestScope();
      TestHelper::initSingleEditBuffer(scope->getTransaction());

      preset->forEachParameter([&](PresetParameter *p) { p->setValue(scope->getTransaction(), val); });

      preset->forEachParameter<VoiceGroup::Global>(
          [&](PresetParameter *p) { p->setValue(scope->getTransaction(), globalValue); });

      eb->undoableLoad(scope->getTransaction(), preset);

      REQUIRE_FALSE(eb->findAnyParameterChanged());

      eb->forEachParameter<VoiceGroup::Global>([&](Parameter *p) {
        if(dynamic_cast<PhysicalControlParameter *>(p) && !dynamic_cast<RibbonParameter *>(p))
          return;

        CHECK_PARAMETER_CP_EQUALS_FICTION(p, globalValue)
      });

      eb->forEachParameter<VoiceGroup::I>([&](Parameter *p) { CHECK_PARAMETER_CP_EQUALS_FICTION(p, val) });
    }
  }
}
