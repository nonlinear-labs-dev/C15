#include <catch.hpp>
#include "testing/TestHelper.h"
#include "testing/unit-tests/mock/MockPresetStorage.h"

TEST_CASE("Init Part Tests")
{
  auto editBuffer = TestHelper::getEditBuffer();
  auto tuneI = editBuffer->findParameterByID({ 358, VoiceGroup::I });
  auto tuneII = editBuffer->findParameterByID({ 358, VoiceGroup::II });
  auto masterTune = editBuffer->findParameterByID({ 247, VoiceGroup::Global });

  {
    auto scope = TestHelper::createTestScope();
    for(auto p : { tuneI, tuneII, masterTune })
    {
      TestHelper::forceParameterChange(scope->getTransaction(), p);
      REQUIRE(p->isChangedFromLoaded());
    }
  }

  WHEN("Init Part I")
  {
    auto scope = TestHelper::createTestScope();
    editBuffer->undoableInitPart(scope->getTransaction(), VoiceGroup::I);

    THEN("Tune I init | Tune II && Master untouched")
    {
      REQUIRE_FALSE(tuneI->isChangedFromLoaded());
      REQUIRE(tuneII->isChangedFromLoaded());
      REQUIRE(masterTune->isChangedFromLoaded());
    }
  }

  WHEN("Init Part II")
  {
    auto scope = TestHelper::createTestScope();
    editBuffer->undoableInitPart(scope->getTransaction(), VoiceGroup::II);

    THEN("Tune II init | Tune I && Master untouched")
    {
      REQUIRE_FALSE(tuneII->isChangedFromLoaded());
      REQUIRE(tuneI->isChangedFromLoaded());
      REQUIRE(masterTune->isChangedFromLoaded());
    }
  }
}
