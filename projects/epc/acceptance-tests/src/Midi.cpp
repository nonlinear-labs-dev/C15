#include <catch.hpp>
#include "Toolbox.h"
#include "AudioEngineOptions.h"
#include <synth/C15Synth.h>
#include <Options.h>

#include <nltools/messaging/Messaging.h>
#include <nltools/messaging/Message.h>
#include <nltools/logging/Log.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/Bank.h>
#include <sys/types.h>
#include <unistd.h>
#include <glibmm.h>

namespace Tests
{
  using namespace nltools::msg;
  using namespace nltools::msg::Midi;
  using namespace std::chrono_literals;

  TEST_CASE("MIDI PC sends only one message")
  {
    Configuration conf;
    conf.offerEndpoints = { { EndPoint::AudioEngine },
                            { EndPoint::ExternalMidiOverIPClient },
                            { EndPoint::Playground },
                            { EndPoint::ExternalMidiOverIPBridge } };

    conf.useEndpoints = { { EndPoint::Playground, "localhost" },
                          { EndPoint::ExternalMidiOverIPBridge, "localhost" },
                          { EndPoint::ExternalMidiOverIPClient, "localhost" },

                          EndPoint::Playcontroller,
                          EndPoint::Oled,
                          EndPoint::PanelLed,
                          EndPoint::RibbonLed,
                          EndPoint::AudioEngine,
                          EndPoint::BeagleBone };

    nltools::msg::init(conf);

    // playground
    Options::s_acceptanceTests = true;  // do not install messaging configuration in Application constructor
    Application playgroundApp(0, nullptr);
    playgroundApp.getPresetManagerUseCases()->createBankAndStoreEditBuffer();
    playgroundApp.getPresetManagerUseCases()->appendPreset(playgroundApp.getPresetManager()->getBankAt(0));
    playgroundApp.getPresetManagerUseCases()->appendPreset(playgroundApp.getPresetManager()->getBankAt(0));
    playgroundApp.getPresetManagerUseCases()->appendPreset(playgroundApp.getPresetManager()->getBankAt(0));

    // audio-engine
    auto options = createEmptyAudioEngineOptions();
    auto synth = std::make_unique<C15Synth>(options.get());

    auto presetSelectionBeforePC = playgroundApp.getPresetManager()->getSelectedPreset();

    int selectionCount = 0;

    sigc::connection c1, c2;

    // wait one second for settle down, then send an PC from external midi
    playgroundApp.getMainContext()->signal_timeout().connect_seconds(
        [&] {
          c1 = playgroundApp.getPresetManager()->getSelectedBank()->onBankChanged([&] {
            if(playgroundApp.getPresetManager()->getSelectedPreset() != presetSelectionBeforePC)
            {
              presetSelectionBeforePC = playgroundApp.getPresetManager()->getSelectedPreset();
              REQUIRE(selectionCount == 0);
              selectionCount++;
            }
          });

          c2 = onConnectionEstablished(EndPoint::ExternalMidiOverIPClient, [] {
            send<nltools::msg::Midi::SimpleMessage>(EndPoint::ExternalMidiOverIPClient, { 0xC0, 0 });
          });
          return false;
        },
        1);

    // no error for three seconds = success
    playgroundApp.getMainContext()->signal_timeout().connect_seconds(
        [&] {
          REQUIRE(selectionCount == 1);
          playgroundApp.quit();
          return false;
        },
        3);

    playgroundApp.run();
  }

  TEST_CASE("MIDI Program change is not returned to sender")
  {
    Configuration conf;
    conf.offerEndpoints = { { EndPoint::AudioEngine },
                            { EndPoint::ExternalMidiOverIPClient },
                            { EndPoint::Playground },
                            { EndPoint::ExternalMidiOverIPBridge } };

    conf.useEndpoints = { { EndPoint::Playground, "localhost" },
                          { EndPoint::ExternalMidiOverIPBridge, "localhost" },
                          { EndPoint::ExternalMidiOverIPClient, "localhost" },

                          EndPoint::Playcontroller,
                          EndPoint::Oled,
                          EndPoint::PanelLed,
                          EndPoint::RibbonLed,
                          EndPoint::AudioEngine,
                          EndPoint::BeagleBone };

    nltools::msg::init(conf);

    // playground
    Options::s_acceptanceTests = true;  // do not install messaging configuration in Application constructor
    Application playgroundApp(0, nullptr);
    playgroundApp.getPresetManagerUseCases()->createBankAndStoreEditBuffer();
    playgroundApp.getPresetManagerUseCases()->appendPreset(playgroundApp.getPresetManager()->getBankAt(0));
    playgroundApp.getPresetManagerUseCases()->appendPreset(playgroundApp.getPresetManager()->getBankAt(0));
    playgroundApp.getPresetManagerUseCases()->appendPreset(playgroundApp.getPresetManager()->getBankAt(0));

    // audio-engine
    auto options = createEmptyAudioEngineOptions();
    auto synth = std::make_unique<C15Synth>(options.get());
    sigc::connection c1;
    sigc::connection c2;

    // wait one second for settle down, then send an PC from external midi
    playgroundApp.getMainContext()->signal_timeout().connect_seconds(
        [&] {
          c1 = receive<ProgramChangeMessage>(EndPoint::AudioEngine, [&](auto msg) { REQUIRE((false)); });
          c2 = onConnectionEstablished(EndPoint::ExternalMidiOverIPClient, [] {
            send<nltools::msg::Midi::SimpleMessage>(EndPoint::ExternalMidiOverIPClient, { 0xC0, 0 });
          });
          return false;
        },
        1);

    // if no REQUIRE((false)) was triggered for three seconds, we are successfully done, so quit the test
    playgroundApp.getMainContext()->signal_timeout().connect_seconds(
        [&] {
          playgroundApp.quit();
          return false;
        },
        3);

    playgroundApp.run();
  }

  TEST_CASE("MIDI Program change is forwarded to playground")
  {
    Configuration conf;
    conf.offerEndpoints = { { EndPoint::AudioEngine },
                            { EndPoint::ExternalMidiOverIPClient },
                            { EndPoint::Playground },
                            { EndPoint::ExternalMidiOverIPBridge } };

    conf.useEndpoints = { { EndPoint::Playground, "localhost" },
                          { EndPoint::ExternalMidiOverIPBridge, "localhost" },
                          { EndPoint::ExternalMidiOverIPClient, "localhost" },

                          EndPoint::Playcontroller,
                          EndPoint::Oled,
                          EndPoint::PanelLed,
                          EndPoint::RibbonLed,
                          EndPoint::AudioEngine,
                          EndPoint::BeagleBone };

    nltools::msg::init(conf);

    // playground
    Options::s_acceptanceTests = true;  // do not install messaging configuration in Application constructor
    Application playgroundApp(0, nullptr);
    playgroundApp.getPresetManagerUseCases()->createBankAndStoreEditBuffer();
    playgroundApp.getPresetManagerUseCases()->appendPreset(playgroundApp.getPresetManager()->getBankAt(0));
    playgroundApp.getPresetManagerUseCases()->appendPreset(playgroundApp.getPresetManager()->getBankAt(0));
    playgroundApp.getPresetManagerUseCases()->appendPreset(playgroundApp.getPresetManager()->getBankAt(0));

    // audio-engine
    auto options = createEmptyAudioEngineOptions();
    auto synth = std::make_unique<C15Synth>(options.get());

    auto presetSelectionBeforePC = playgroundApp.getPresetManager()->getSelectedPreset();
    sigc::connection c;
    c = playgroundApp.getPresetManager()->getSelectedBank()->onBankChanged([&] {
      if(playgroundApp.getPresetManager()->getSelectedPreset() != presetSelectionBeforePC)
      {
        // preset selection changed by sending PC, success
        presetSelectionBeforePC = playgroundApp.getPresetManager()->getSelectedPreset();
        playgroundApp.quit();
      }
    });

    sigc::connection c2 = onConnectionEstablished(EndPoint::ExternalMidiOverIPClient, [] {
      send<nltools::msg::Midi::SimpleMessage>(EndPoint::ExternalMidiOverIPClient, { 0xC0, 0 });
    });

    playgroundApp.run();
  }
}
