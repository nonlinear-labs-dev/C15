#include <catch.hpp>
#include <synth/input/InputEventStage.h>
#include <synth/C15Synth.h>
#include <mock/MockDSPHosts.h>
#include <testing/TestHelper.h>

MidiRuntimeOptions createMidiSettings()
{
  MidiRuntimeOptions options;
  nltools::msg::Setting::MidiSettingsMessage msg;
  msg.receiveNotes = true;
  msg.receiveProgramChange = true;
  msg.receiveChannel = MidiReceiveChannel::Omni;
  msg.receiveSplitChannel = MidiReceiveChannelSplit::Omni;

  msg.bendercc = BenderCC::Pitchbend;
  msg.aftertouchcc = AftertouchCC::ChannelPressure;
  msg.pedal1cc = PedalCC::CC20;
  msg.pedal2cc = PedalCC::CC21;
  msg.pedal3cc = PedalCC::CC22;
  msg.pedal4cc = PedalCC::CC23;
  msg.ribbon1cc = RibbonCC::CC24;
  msg.ribbon2cc = RibbonCC::CC25;

  msg.hwMappings = TestHelper::createFullMappings(true);

  options.update(msg);
  return options;
}

class MockMIDIDecoder : public MIDIDecoder
{
 public:
  using MIDIDecoder::MIDIDecoder;
  void setValue(float v)
  {
    value = v;
  }

  void setType(DecoderEventType type)
  {
    m_type = type;
  }

  void setKeyOrCtrl(int k)
  {
    keyOrControl = k;
  }

  void setChannel(MidiReceiveChannel c)
  {
    m_midiChannel = c;
  }
};

TEST_CASE("Midi Decoder Reset", "[MIDI]")
{
  MockDSPHost host;
  auto setting = createMidiSettings();
  MockMIDIDecoder decoder(&host, &setting);

  decoder.setChannel(MidiReceiveChannel::CH_1);
  decoder.setKeyOrCtrl(12);
  decoder.setType(DecoderEventType::KeyUp);
  decoder.setValue(187);

  decoder.reset();
  CHECK(decoder.getValue() == 0);
  CHECK(decoder.getEventType() == DecoderEventType::UNKNOWN);
  CHECK(decoder.getKeyOrControl() == -1);
  CHECK(decoder.getChannel() == MidiReceiveChannel::None);
}

TEST_CASE("MIDI input on Secondary channel is ignored if not in split", "[MIDI]")
{
  PassOnKeyDownHost dsp { 17, 1.0, VoiceGroup::I };
  auto settings = createMidiSettings();
  settings.setReceiveChannel(MidiReceiveChannel::CH_1);
  settings.setSplitReceiveChannel(MidiReceiveChannelSplit::CH_2);
  dsp.setType(SoundType::Single);

  InputEventStage eventStage(
      &dsp, &settings, [] {}, [](auto) { CHECK(false); });

  WHEN("Channel 1 KeyDown")
  {
    eventStage.onMIDIMessage({ 0x90, (uint8_t) 17, (uint8_t) 127 });
    CHECK(dsp.didReceiveKeyDown());
  }

  WHEN("Channel 2 KeyDown")
  {
    eventStage.onMIDIMessage({ 0x91, (uint8_t) 17, (uint8_t) 127 });
    CHECK(!dsp.didReceiveKeyDown());
  }
}

TEST_CASE("Input Event Mapping CC to HW", "[MIDI]")
{
  PassOnHWReceived dsp { 0, 1.0 };
  auto settings = createMidiSettings();
  InputEventStage eventStage(
      &dsp, &settings, [] {}, [](nltools::msg::Midi::SimpleMessage msg) { CHECK(false); });

  WHEN("Pedal CC 01")
  {
    settings.setPedal1(PedalCC::CC01);

    WHEN("Send 14 Bit as CC01 && CC33")
    {
      eventStage.onMIDIMessage({ 0xB0, 33, 127 });
      eventStage.onMIDIMessage({ 0xB0, 01, 127 });
      CHECK(dsp.didReceiveHW());
    }

    WHEN("Send 14 Bit as CC02 && CC34")
    {
      eventStage.onMIDIMessage({ 0xB0, 34, 127 });
      eventStage.onMIDIMessage({ 0xB0, 02, 127 });
      CHECK(!dsp.didReceiveHW());
    }

    WHEN("Send 7 Bit as CC01")
    {
      eventStage.onMIDIMessage({ 0xB0, 01, 127 });
      CHECK(dsp.didReceiveHW());
    }

    WHEN("Send 7 Bit as CC02")
    {
      eventStage.onMIDIMessage({ 0xB0, 02, 127 });
      CHECK(!dsp.didReceiveHW());
    }
  }
}

TEST_CASE("Input Event Stage MIDI In KeyDown", "[MIDI]")
{
  PassOnKeyDownHost dsp { 17, 1, VoiceGroup::I };

  auto settings = createMidiSettings();
  InputEventStage eventStage(
      &dsp, &settings, [] {}, [](nltools::msg::Midi::SimpleMessage msg) { CHECK(false); });

  WHEN("Send 14 Bit")
  {
    eventStage.onMIDIMessage({ 0xB0, (uint8_t) 88, (uint8_t) 127 });
    eventStage.onMIDIMessage({ 0x90, (uint8_t) 17, (uint8_t) 127 });
    CHECK(dsp.didReceiveKeyDown());
  }

  WHEN("Send 7 Bit")
  {
    eventStage.onMIDIMessage({ 0x90, (uint8_t) 17, (uint8_t) 127 });
    CHECK(dsp.didReceiveKeyDown());
  }
}

TEST_CASE("Input Event Stage MIDI In KeyUp", "[MIDI]")
{
  PassOnKeyUpHost dsp { 17, 1, VoiceGroup::I };

  auto settings = createMidiSettings();
  InputEventStage eventStage(
      &dsp, &settings, [] {}, [](nltools::msg::Midi::SimpleMessage msg) { CHECK(false); });

  WHEN("W/o Velo")
  {
    eventStage.onMIDIMessage({ 0x80, (uint8_t) 17, (uint8_t) 127 });

    CHECK(dsp.didReceiveKeyUp());
  }

  WHEN("With Velo")
  {
    eventStage.onMIDIMessage({ 0xB0, 88, 127 });
    eventStage.onMIDIMessage({ 0x80, (uint8_t) 17, (uint8_t) 127 });

    CHECK(dsp.didReceiveKeyUp());
  }
}

TEST_CASE("Input Event Stage MIDI In HWSource -> Pedal1 100%", "[MIDI]")
{
  PassOnHWReceived dsp { 0, 1 };

  auto settings = createMidiSettings();
  InputEventStage eventStage(
      &dsp, &settings, [] {}, [](nltools::msg::Midi::SimpleMessage msg) { CHECK(false); });

  WHEN("Send 14 Bit with enabled 14 bit support")
  {
    settings.set14BitSupportEnabled(true);
    eventStage.onMIDIMessage({ 0xB0, 52, 127 });
    eventStage.onMIDIMessage({ 0xB0, 20, 127 });
    CHECK(dsp.didReceiveHW());
  }

  WHEN("Send 14 Bit with disbled 14 bit support")
  {
    settings.set14BitSupportEnabled(false);
    eventStage.onMIDIMessage({ 0xB0, 52, 127 });
    eventStage.onMIDIMessage({ 0xB0, 20, 127 });
    CHECK(dsp.didReceiveHW());
  }

  WHEN("Send 7 Bit")
  {
    eventStage.onMIDIMessage({ 0xB0, 20, 127 });
    CHECK(dsp.didReceiveHW());
  }
}

TEST_CASE("MIDI in of PitchBender as Channel Pitchbend", "[MIDI]")
{
  PassOnHWReceived dsp { 4, 1 };
  auto settings = createMidiSettings();
  InputEventStage eventStage(
      &dsp, &settings, [] {}, [](auto msg) { CHECK(false); });
  eventStage.onMIDIMessage({ 0b11100000, 127, 127 });
  CHECK(dsp.didReceiveHW());
}

TEST_CASE("MIDI in of PitchBender as Control Change", "[MIDI]")
{
  PassOnHWReceived dsp { 4, 1 };
  auto settings = createMidiSettings();
  settings.setBenderCC(BenderCC::CC01);

  InputEventStage eventStage(
      &dsp, &settings, [] {}, [](auto) { CHECK(false); });

  WHEN("Send 14 Bit")
  {
    eventStage.onMIDIMessage({ 0xB0, 33, 127 });
    eventStage.onMIDIMessage({ 0xB0, 1, 127 });
    CHECK(dsp.didReceiveHW());
  }

  WHEN("Send 7 Bit")
  {
    eventStage.onMIDIMessage({ 0xB0, 1, 127 });
    CHECK(dsp.didReceiveHW());
  }
}

TEST_CASE("MIDI in of Aftertouch as Channel Pressure", "[MIDI]")
{
  PassOnHWReceived dsp { 5, 1 };
  auto settings = createMidiSettings();
  settings.setAftertouchCC(AftertouchCC::ChannelPressure);

  InputEventStage eventStage(
      &dsp, &settings, [] {}, [](auto) { CHECK(false); });
  eventStage.onMIDIMessage({ 0b11010000, 127, 0 });
  CHECK(dsp.didReceiveHW());
}

TEST_CASE("MIDI in of Aftertouch as Control Change", "[MIDI]")
{
  PassOnHWReceived dsp { 5, 1 };
  auto settings = createMidiSettings();
  settings.setAftertouchCC(AftertouchCC::CC01);

  InputEventStage eventStage(
      &dsp, &settings, [] {}, [](auto) { CHECK(false); });

  WHEN("Send 14 Bit")
  {
    eventStage.onMIDIMessage({ 0xB0, 33, 127 });
    eventStage.onMIDIMessage({ 0xB0, 1, 127 });
    CHECK(dsp.didReceiveHW());
  }

  WHEN("Send 7 Bit")
  {
    eventStage.onMIDIMessage({ 0xB0, 1, 127 });
    CHECK(dsp.didReceiveHW());
  }
}