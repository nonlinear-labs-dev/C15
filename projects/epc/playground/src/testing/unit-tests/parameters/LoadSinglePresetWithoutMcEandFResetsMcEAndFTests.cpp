#include <testing/unit-tests/mock/PreDualModePresetBank.h>
#include "testing/TestHelper.h"
#include "testing/unit-tests/mock/MockPresetStorage.h"
#include "presets/Preset.h"
#include "presets/PresetParameter.h"
#include "parameters/MacroControlParameter.h"
#include "parameters/ModulateableParameter.h"

TEST_CASE("Load Single Preset Without MacroControl E and F defaults MC E and F")
{
  auto mockPreset = PreDualModePresetBank::createMockPreset();

  auto eb = TestHelper::getEditBuffer();
  auto scope = TestHelper::createTestScope();
  TestHelper::initSingleEditBuffer(scope->getTransaction());

  auto mcE = dynamic_cast<MacroControlParameter*>(eb->findParameterByID({ 369, VoiceGroup::Global }));
  auto mcF = dynamic_cast<MacroControlParameter*>(eb->findParameterByID({ 371, VoiceGroup::Global }));

  auto partVolume = dynamic_cast<ModulateableParameter*>(eb->findParameterByID({ 358, VoiceGroup::I }));
  auto partTune = dynamic_cast<ModulateableParameter*>(eb->findParameterByID({ 360, VoiceGroup::I }));

  partVolume->setModulationSource(scope->getTransaction(), MacroControls::MC5);
  partVolume->setModulationAmount(scope->getTransaction(), 0.2);
  partTune->setModulationSource(scope->getTransaction(), MacroControls::MC6);
  partTune->setModulationAmount(scope->getTransaction(), 0.4);

  mcE->undoableSetGivenName(scope->getTransaction(), "EE");
  mcF->undoableSetGivenName(scope->getTransaction(), "FF");
  mcE->setCPFromHwui(scope->getTransaction(), 0.75);
  mcF->setCPFromHwui(scope->getTransaction(), 0.25);

  THEN("Loading old Preset resets non existing parameters")
  {
    eb->undoableLoad(scope->getTransaction(), mockPreset.get(), true);
    REQUIRE(mcE->getGivenName() == "");
    REQUIRE(mcF->getGivenName() == "");
    REQUIRE(partTune->getModulationSource() == MacroControls::NONE);
    REQUIRE(partVolume->getModulationSource() == MacroControls::NONE);
    REQUIRE(partTune->getModulationAmount() == Approx(0.0));
    REQUIRE(partVolume->getModulationAmount() == Approx(0.0));
  }
}
