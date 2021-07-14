#include <catch.hpp>
#include <synth/c15-audio-engine/midi_handle.h>

TEST_CASE("Encode Decode", "[MIDI][TCD]")
{
  using CC_Range_7_Bit = Midi::FullCCRange<Midi::Formats::_7_Bits_>;
  using CC_Range_14_Bit = Midi::clipped14BitCCRange;
  using CC_Range_Bender = Midi::FullCCRange<Midi::Formats::_14_Bits_>;
  using CC_Range_Vel = Midi::clipped14BitVelRange;

  // pitch bend
  CHECK(CC_Range_Bender::encodeBipolarMidiValue(-1.5f) == 0x0000);
  CHECK(CC_Range_Bender::encodeBipolarMidiValue(-1.0f) == 0x0000);
  CHECK(CC_Range_Bender::encodeBipolarMidiValue(0.0f) == 0x2000);
  CHECK(CC_Range_Bender::encodeBipolarMidiValue(1.0f) == 0x3FFF);
  CHECK(CC_Range_Bender::encodeBipolarMidiValue(1.5f) == 0x3FFF);

  CHECK(CC_Range_Bender::decodeBipolarMidiValue(0x0000) == -1.0f);
  CHECK(CC_Range_Bender::decodeBipolarMidiValue(0x2000) == 0.0f);
  CHECK(CC_Range_Bender::decodeBipolarMidiValue(0x3FFF) == 1.0f);

  // 14 bit bipolar controllers
  CHECK(CC_Range_14_Bit::encodeBipolarMidiValue(-1.5f) == 0x0000);
  CHECK(CC_Range_14_Bit::encodeBipolarMidiValue(-1.0f) == 0x0000);
  CHECK(CC_Range_14_Bit::encodeBipolarMidiValue(0.0f) == 0x2000);
  CHECK(CC_Range_14_Bit::encodeBipolarMidiValue(1.0f) == 0x3F80);
  CHECK(CC_Range_14_Bit::encodeBipolarMidiValue(1.5f) == 0x3F80);

  CHECK(CC_Range_14_Bit::decodeBipolarMidiValue(0x0000) == -1.0f);
  CHECK(CC_Range_14_Bit::decodeBipolarMidiValue(0x2000) == 0.0f);
  CHECK(CC_Range_14_Bit::decodeBipolarMidiValue(0x3F80) == 1.0f);

  // 14 bit unipolar controllers
  CHECK(CC_Range_14_Bit::encodeUnipolarMidiValue(-0.5f) == 0x0000);
  CHECK(CC_Range_14_Bit::encodeUnipolarMidiValue(0.0f) == 0x0000);
  CHECK(CC_Range_14_Bit::encodeUnipolarMidiValue(0.5f) == 0x2000);
  CHECK(CC_Range_14_Bit::encodeUnipolarMidiValue(1.0f) == 0x3F80);
  CHECK(CC_Range_14_Bit::encodeUnipolarMidiValue(1.5f) == 0x3F80);

  CHECK(CC_Range_14_Bit::decodeUnipolarMidiValue(0x0000) == 0.0f);
  CHECK(CC_Range_14_Bit::decodeUnipolarMidiValue(0x2000) == 0.5f);
  CHECK(CC_Range_14_Bit::decodeUnipolarMidiValue(0x3F80) == 1.0f);

  // 7 bit unipolar controllers
  CHECK(CC_Range_7_Bit::encodeUnipolarMidiValue(-0.5f) == 0x00);
  CHECK(CC_Range_7_Bit::encodeUnipolarMidiValue(0.0f) == 0x00);
  CHECK(CC_Range_7_Bit::encodeUnipolarMidiValue(0.5f) == 0x40);
  CHECK(CC_Range_7_Bit::encodeUnipolarMidiValue(1.0f) == 0x7F);
  CHECK(CC_Range_7_Bit::encodeUnipolarMidiValue(1.5f) == 0x7F);

  CHECK(CC_Range_7_Bit::decodeUnipolarMidiValue(0x00) == 0.0f);
  CHECK(CC_Range_7_Bit::decodeUnipolarMidiValue(0x40) == 0.5f);
  CHECK(CC_Range_7_Bit::decodeUnipolarMidiValue(0x7F) == 1.0f);

  // velocity control
  CHECK(CC_Range_Vel::encodeUnipolarMidiValue(-0.5f) == 0x0000);
  CHECK(CC_Range_Vel::encodeUnipolarMidiValue(0.0f) == 0x0080);
  CHECK(CC_Range_Vel::encodeUnipolarMidiValue(0.5f) == 0x2000);
  CHECK(CC_Range_Vel::encodeUnipolarMidiValue(1.0f) == 0x3F80);
  CHECK(CC_Range_Vel::encodeUnipolarMidiValue(1.5f) == 0x3F80);

  CHECK(CC_Range_Vel::decodeUnipolarMidiValue(0x0000) == 0.0f);
  CHECK(CC_Range_Vel::decodeUnipolarMidiValue(0x0080) == 0.0f);
  CHECK(CC_Range_Vel::decodeUnipolarMidiValue(0x2000) == 0.5f);
  CHECK(CC_Range_Vel::decodeUnipolarMidiValue(0x3F80) == 1.0f);
  CHECK(CC_Range_Vel::decodeUnipolarMidiValue(0x3FFF) == 1.0f);
}