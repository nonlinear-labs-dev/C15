//#include <catch.hpp>
//#include "Toolbox.h"
//#include "AudioEngineOptions.h"
//#include <synth/C15Synth.h>
//#include <Options.h>
//
//#include <nltools/messaging/Messaging.h>
//#include <nltools/messaging/Message.h>
//#include <nltools/logging/Log.h>
//#include <Application.h>
//#include <presets/PresetManager.h>
//#include <presets/Bank.h>
//#include <sys/types.h>
//#include <unistd.h>
//#include <glibmm.h>
//
//namespace Tests
//{
//  using namespace nltools::msg;
//  using namespace nltools::msg::Midi;
//  using namespace std::chrono_literals;
//
//  TEST_CASE("MIDI PC sends only one message", "[MIDI]")
//  {
//    Configuration conf;
//    conf.offerEndpoints = { { EndPoint::AudioEngine },
//                            { EndPoint::ExternalMidiOverIPClient },
//                            { EndPoint::Playground },
//                            { EndPoint::ExternalMidiOverIPBridge } };
//
//    conf.useEndpoints = { { EndPoint::Playground, "localhost" },
//                          { EndPoint::ExternalMidiOverIPBridge, "localhost" },
//                          { EndPoint::ExternalMidiOverIPClient, "localhost" },
//
//                          EndPoint::Playcontroller,
//                          EndPoint::Oled,
//                          EndPoint::PanelLed,
//                          EndPoint::RibbonLed,
//                          EndPoint::AudioEngine,
//                          EndPoint::BeagleBone };
//
//    nltools::msg::init(conf);
//
//    // playground
//    Options::s_acceptanceTests = true;  // do not install messaging configuration in Application constructor
//    Application playgroundApp(0, nullptr);
//    PresetManagerUseCases uc(playgroundApp.getPresetManager());
//    uc.createBankAndStoreEditBuffer();
//    auto bank = playgroundApp.getPresetManager()->getBankAt(0);
//    uc.appendPreset(bank);
//    uc.appendPreset(bank);
//    uc.appendPreset(bank);
//    uc.selectMidiBank(bank);
//
//    // audio-engine
//    auto options = createEmptyAudioEngineOptions();
//    auto synth = std::make_unique<C15Synth>(options.get());
//
//    auto presetSelectionBeforePC = playgroundApp.getPresetManager()->getSelectedPreset();
//
//    int selectionCount = 0;
//
//    sigc::connection c1, c2, c3;
//
//    /*
//     * TODO This Test is not deterministic, it sometimes fails or passes for no apparent reason
//     * lets either increase the "settle-down" time or use an more sophisticated approach to check if the PG is ready/done
//     * this would also decrease test-runtime -> very nice
//     */
//
//    {
//      nltools::msg::Setting::MidiSettingsMessage msg;
//      msg.receiveChannel = MidiReceiveChannel::CH_1;
//      msg.receiveProgramChange = true;
//      synth->onMidiSettingsMessage(msg);
//    }
//
//    // wait one second for settle down, then send an PC from external midi
//    playgroundApp.getMainContext()->signal_timeout().connect_seconds(
//        [&]
//        {
//          c1 = playgroundApp.getPresetManager()->getSelectedBank()->onBankChanged(
//              [&]
//              {
//                if(playgroundApp.getPresetManager()->getSelectedPreset() != presetSelectionBeforePC)
//                {
//                  presetSelectionBeforePC = playgroundApp.getPresetManager()->getSelectedPreset();
//                  CHECK(selectionCount == 0);
//                  selectionCount++;
//                }
//              });
//
//          c2 = onConnectionEstablished(
//              EndPoint::ExternalMidiOverIPClient,
//              [] {
//                send<nltools::msg::Midi::SimpleMessage>(EndPoint::ExternalMidiOverIPClient, { 0xC0, 0 });
//              });
//
//          return false;
//        },
//        1);
//
//    playgroundApp.getMainContext()->signal_timeout().connect_seconds(
//        [&]
//        {
//          CHECK(selectionCount == 1);
//          playgroundApp.quit();
//          return false;
//        },
//        3);
//
//    playgroundApp.run();
//  }
//
//  TEST_CASE("MIDI Program change is not returned to sender", "[MIDI]")
//  {
//    Configuration conf;
//    conf.offerEndpoints = { { EndPoint::AudioEngine },
//                            { EndPoint::ExternalMidiOverIPClient },
//                            { EndPoint::Playground },
//                            { EndPoint::ExternalMidiOverIPBridge } };
//
//    conf.useEndpoints = { { EndPoint::Playground, "localhost" },
//                          { EndPoint::ExternalMidiOverIPBridge, "localhost" },
//                          { EndPoint::ExternalMidiOverIPClient, "localhost" },
//
//                          EndPoint::Playcontroller,
//                          EndPoint::Oled,
//                          EndPoint::PanelLed,
//                          EndPoint::RibbonLed,
//                          EndPoint::AudioEngine,
//                          EndPoint::BeagleBone };
//
//    nltools::msg::init(conf);
//
//    // playground
//    Options::s_acceptanceTests = true;  // do not install messaging configuration in Application constructor
//    Application playgroundApp(0, nullptr);
//    PresetManagerUseCases uc(playgroundApp.getPresetManager());
//    uc.createBankAndStoreEditBuffer();
//    auto bank = playgroundApp.getPresetManager()->getBankAt(0);
//    uc.appendPreset(bank);
//    uc.appendPreset(bank);
//    uc.appendPreset(bank);
//    uc.selectMidiBank(bank);
//
//    // audio-engine
//    auto options = createEmptyAudioEngineOptions();
//    auto synth = std::make_unique<C15Synth>(options.get());
//    sigc::connection c1, c2, c3;
//
//    {
//      nltools::msg::Setting::MidiSettingsMessage msg;
//      msg.receiveChannel = MidiReceiveChannel::CH_1;
//      msg.receiveProgramChange = true;
//      synth->onMidiSettingsMessage(msg);
//    }
//
//    // wait one second for settle down, then send an PC from external midi
//    playgroundApp.getMainContext()->signal_timeout().connect_seconds(
//        [&]
//        {
//          c1 = receive<ProgramChangeMessage>(EndPoint::AudioEngine, [&](auto msg) { CHECK((false)); });
//          c3 = receive<SimpleMessage>(EndPoint::ExternalMidiOverIPBridge, [](auto msg) { CHECK(false); });
//          c2 = onConnectionEstablished(
//              EndPoint::ExternalMidiOverIPClient,
//              [] {
//                send<nltools::msg::Midi::SimpleMessage>(EndPoint::ExternalMidiOverIPClient, { 0xC0, 0 });
//              });
//          return false;
//        },
//        1);
//
//    playgroundApp.getMainContext()->signal_timeout().connect_seconds(
//        [&]
//        {
//          playgroundApp.quit();
//          return false;
//        },
//        3);
//
//    playgroundApp.run();
//  }
//
//  TEST_CASE("MIDI Program change is forwarded to playground", "[MIDI]")
//  {
//    Configuration conf;
//    conf.offerEndpoints = { { EndPoint::AudioEngine },
//                            { EndPoint::ExternalMidiOverIPClient },
//                            { EndPoint::Playground },
//                            { EndPoint::ExternalMidiOverIPBridge } };
//
//    conf.useEndpoints = { { EndPoint::Playground, "localhost" },
//                          { EndPoint::ExternalMidiOverIPBridge, "localhost" },
//                          { EndPoint::ExternalMidiOverIPClient, "localhost" },
//
//                          EndPoint::Playcontroller,
//                          EndPoint::Oled,
//                          EndPoint::PanelLed,
//                          EndPoint::RibbonLed,
//                          EndPoint::AudioEngine,
//                          EndPoint::BeagleBone };
//
//    nltools::msg::init(conf);
//
//    // playground
//    Options::s_acceptanceTests = true;  // do not install messaging configuration in Application constructor
//    Application playgroundApp(0, nullptr);
//    PresetManagerUseCases uc(playgroundApp.getPresetManager());
//    uc.createBankAndStoreEditBuffer();
//    auto bank = playgroundApp.getPresetManager()->getBankAt(0);
//    uc.appendPreset(bank);
//    uc.appendPreset(bank);
//    uc.appendPreset(bank);
//    uc.selectMidiBank(bank);
//
//    // audio-engine
//    auto options = createEmptyAudioEngineOptions();
//    auto synth = std::make_unique<C15Synth>(options.get());
//
//    {
//      nltools::msg::Setting::MidiSettingsMessage msg;
//      msg.receiveChannel = MidiReceiveChannel::CH_1;
//      msg.receiveProgramChange = true;
//      synth->onMidiSettingsMessage(msg);
//    }
//
//    auto presetSelectionBeforePC = playgroundApp.getPresetManager()->getSelectedPreset();
//    auto c = bank->onBankChanged(
//        [&]
//        {
//          if(playgroundApp.getPresetManager()->getSelectedPreset() != presetSelectionBeforePC)
//          {
//            // preset selection changed by sending PC, success
//            presetSelectionBeforePC = playgroundApp.getPresetManager()->getSelectedPreset();
//            playgroundApp.quit();
//          }
//        });
//
//    sigc::connection c2 = onConnectionEstablished(
//        EndPoint::ExternalMidiOverIPClient,
//        [] {
//          send<nltools::msg::Midi::SimpleMessage>(EndPoint::ExternalMidiOverIPClient, { 0xC0, 0 });
//        });
//
//    playgroundApp.getMainContext()->signal_timeout().connect_seconds(
//        [&]
//        {
//          playgroundApp.quit();
//          return false;
//        },
//        3);
//    playgroundApp.run();
//  }
//}
