#include <testing/TestHelper.h>

TEST_CASE("Default value for Fade From is different in I and II")
{
  auto eb = TestHelper::getEditBuffer();
  CHECK(TestHelper::floating::differs(
      eb->findParameterByID({ 396, VoiceGroup::I })->getValue().getFactoryDefaultValue(),
      eb->findParameterByID({ 396, VoiceGroup::II })->getValue().getFactoryDefaultValue()));
}

TEST_CASE("Fade From gets properly initialized on convert to Layer")
{
  auto scope = TestHelper::createTestScope();
  TestHelper::initSingleEditBuffer(scope->getTransaction());

  auto eb = TestHelper::getEditBuffer();
  eb->undoableConvertToDual(scope->getTransaction(), SoundType::Layer, VoiceGroup::II);

  CHECK(eb->findParameterByID({ 396, VoiceGroup::I })->getDisplayString() == "C6");
  CHECK(eb->findParameterByID({ 396, VoiceGroup::II })->getDisplayString() == "C1");
}
