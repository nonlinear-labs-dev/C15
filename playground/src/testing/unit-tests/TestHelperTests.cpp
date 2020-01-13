#include <testing/unit-tests/mock/MockPresetStorage.h>
#include "testing/TestHelper.h"
#include <presets/Preset.h>
#include <presets/PresetParameter.h>

TEST_CASE("PARAMETER VALUE ASSERT")
{
  auto eb = TestHelper::getEditBuffer();
  auto scope = TestHelper::createTestScope();
  auto transaction = scope->getTransaction();

  THEN("Stetting rough Value leds to correct Assert")
  {

    for(auto v : { 0.2, 0.0, 0.1, 0.17635456, 0.1235425, 0.1234525643, 0.8563, 0.99, 1.0, -1.0 })
      eb->forEachParameter([&](Parameter* p) {
        p->setCPFromHwui(transaction, v);
        CHECK_PARAMETER_CP_EQUALS_FICTION(p, v);
      });
  }
}
