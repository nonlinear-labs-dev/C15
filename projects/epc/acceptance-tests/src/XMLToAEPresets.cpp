#include <catch.hpp>
#include <mock/XMLPresetLoader.h>
#include <memory>
#include <synth/C15Synth.h>
#include <AudioEngineOptions.h>
#include <Toolbox.h>
#include <testing/TestHelper.h>

TEST_CASE("Load XML Preset into AE")
{
  auto options = Tests::createEmptyAudioEngineOptions();
  auto synth = std::make_unique<C15Synth>(options.get());

  //Prepare Midi Settings
  {
    using tMSG = nltools::msg::Setting::MidiSettingsMessage;
    tMSG msg;
    msg.receiveChannel = MidiReceiveChannel::CH_1;
    TestHelper::updateMappingForHW(msg.routings,
                                   tMSG::RoutingIndex::Notes,
                                   tMSG::RoutingAspect::RECEIVE_PRIMARY,
                                   true);

    synth->onMidiSettingsMessage(msg);
  }

  WHEN("Init Preset is Loaded")
  {
    XMLPresetLoader::loadTestPresetFromBank(synth.get(), "xml-banks", "Init");
    THEN("Note Played produces no Sound")
    {
      synth->doMidi({ 0x90, 127, 127 });
      auto res = synth->measurePerformance(std::chrono::milliseconds(250));
      CHECK(Tests::getMaxLevel(std::get<0>(res)) == 0);
    }
  }

  WHEN("Init Preset with Mixer A up, is Loaded")
  {
    XMLPresetLoader::loadTestPresetFromBank(synth.get(), "xml-banks", "InitWithAMix");
    THEN("Note Played produces Sound")
    {
      synth->doMidi({ 0x90, 127, 127 });
      auto res = synth->measurePerformance(std::chrono::milliseconds(250));
      CHECK(Tests::getMaxLevel(std::get<0>(res)) > 0.2f);
    }
  }
}
