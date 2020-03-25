#include <testing/TestHelper.h>

TEST_CASE("Convert Layer to Split Sound Resets Split", "[EditBuffer][Convert]")
{
  auto eb = TestHelper::getEditBuffer();
  auto splitPoint = eb->findParameterByID({ 356, VoiceGroup::Global });

  REQUIRE(splitPoint != nullptr);
  TestHelper::initDualEditBuffer<SoundType::Layer>();

  //Prepare changed splitPoint
  {
    auto scope = TestHelper::createTestScope();
    splitPoint->setCPFromHwui(scope->getTransaction(), 0.0);
  }

  //Convert to Split
  {
    auto scope = TestHelper::createTestScope();
    eb->undoableConvertToDual(scope->getTransaction(), SoundType::Split);
  }

  CHECK_PARAMETER_CP_EQUALS_FICTION(splitPoint, 0.5);
}

TEST_CASE("Convert Layer to Split works for parameter Groups", "[EditBuffer][Convert]")
{
  auto eb = TestHelper::getEditBuffer();

  auto paramI = eb->findParameterByID({ 0, VoiceGroup::I });
  auto paramII = eb->findParameterByID({ 0, VoiceGroup::II });

  WHEN("Parameters I / II have different values")
  {
    auto scope = TestHelper::createTestScope();
    TestHelper::initDualEditBuffer<SoundType::Layer>();
    paramI->setCPFromHwui(scope->getTransaction(), 0);
    paramII->setCPFromHwui(scope->getTransaction(), 1);

    THEN("Converted to Split Sound the Values still differ")
    {
      eb->undoableConvertToDual(scope->getTransaction(), SoundType::Split);
      REQUIRE(eb->getType() == SoundType::Split);
      REQUIRE(paramI->getValue().getRawValue() != paramII->getValue().getRawValue());
    }
  }
}

TEST_CASE("Converting Layer to Split/Single resets Mute Parameter", "[EditBuffer][Convert]")
{
  auto eb = TestHelper::getEditBuffer();
  auto muteI = eb->findParameterByID({ 395, VoiceGroup::I });
  auto muteII = eb->findParameterByID({ 395, VoiceGroup::II });

  WHEN("Mute is enabled in Layer")
  {
    TestHelper::initDualEditBuffer<SoundType::Layer>();
    auto scope = TestHelper::createTestScope();
    muteI->setCPFromHwui(scope->getTransaction(), 1);
    muteII->setCPFromHwui(scope->getTransaction(), 1);

    CHECK_PARAMETER_CP_EQUALS_FICTION(muteI, 1);
    CHECK_PARAMETER_CP_EQUALS_FICTION(muteII, 1);

    WHEN("Converted to Split")
    {
      eb->undoableConvertToDual(scope->getTransaction(), SoundType::Split);

      THEN("Mute is disabled!")
      {
        CHECK_PARAMETER_CP_EQUALS_FICTION(muteI, 0);
        CHECK_PARAMETER_CP_EQUALS_FICTION(muteII, 0);
      }
    }
  }
}