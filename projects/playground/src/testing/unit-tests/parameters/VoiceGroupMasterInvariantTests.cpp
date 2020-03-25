#include <testing/TestHelper.h>

TEST_CASE("Default value for Fade From is different in I and II")
{
  TestHelper::initDualEditBuffer<SoundType::Layer>();

  auto eb = TestHelper::getEditBuffer();
  CHECK(eb->findParameterByID({ 396, VoiceGroup::I })->getDisplayString() == "C6");
  CHECK(eb->findParameterByID({ 396, VoiceGroup::II })->getDisplayString() == "C1");
}

TEST_CASE("Fade From gets properly initialized on convert to Layer")
{
  auto scope = TestHelper::createTestScope();
  TestHelper::initSingleEditBuffer(scope->getTransaction());

  auto eb = TestHelper::getEditBuffer();
  eb->undoableConvertToDual(scope->getTransaction(), SoundType::Layer);

  CHECK(eb->findParameterByID({ 396, VoiceGroup::I })->getDisplayString() == "C6");
  CHECK(eb->findParameterByID({ 396, VoiceGroup::II })->getDisplayString() == "C1");
}