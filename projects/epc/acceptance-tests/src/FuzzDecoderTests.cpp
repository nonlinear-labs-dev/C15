#include <catch.hpp>
#include <synth/input/MIDIDecoder.h>
#include <synth/input/TCDDecoder.h>
#include <mock/MockDSPHosts.h>

TEST_CASE("Fuzz MIDIDecoder", "[FUZZ]")
{
  MockDSPHost host;
  MidiRuntimeOptions settings;
  MIDIDecoder decoder(&host, &settings);

  for(uint8_t d0 = std::numeric_limits<uint8_t>::min(); d0 < std::numeric_limits<uint8_t>::max(); d0++)
  {
    for(uint8_t d1 = std::numeric_limits<uint8_t>::min(); d1 < std::numeric_limits<uint8_t>::max(); d1++)
    {
      for(uint8_t d2 = std::numeric_limits<uint8_t>::min(); d2 < std::numeric_limits<uint8_t>::max(); d2++)
      {
        CHECK_NOTHROW(decoder.decode({ d0, d1, d2 }));
      }
    }
  }
}

TEST_CASE("Fuzz TCDDecoder", "[FUZZ]")
{
  MockDSPHost host;
  MidiRuntimeOptions settings;
  KeyShift kS;
  TCDDecoder decoder(&host, &settings, &kS);

  for(uint8_t d0 = std::numeric_limits<uint8_t>::min(); d0 < std::numeric_limits<uint8_t>::max(); d0++)
  {
    for(uint8_t d1 = std::numeric_limits<uint8_t>::min(); d1 < std::numeric_limits<uint8_t>::max(); d1++)
    {
      for(uint8_t d2 = std::numeric_limits<uint8_t>::min(); d2 < std::numeric_limits<uint8_t>::max(); d2++)
      {
        CHECK_NOTHROW(decoder.decode({ d0, d1, d2 }));
      }
    }
  }
}