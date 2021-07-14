#include <catch.hpp>
#include <MidiRuntimeOptions.h>
#include <mock/MidiRuntimeOptionsTester.h>

TEST_CASE("PedalCC to int")
{
  auto getMSB = [](PedalCC c) { return MidiRuntimeOptionsTester::decodeEnumMSB(c); };

  auto getLSB = [](PedalCC c) { return MidiRuntimeOptionsTester::decodeEnumLSB(c); };

  CHECK(getMSB(PedalCC::None) == std::nullopt);
  CHECK(getMSB(PedalCC::CC01).value() == 1);
  CHECK(getMSB(PedalCC::CC20).value() == 20);
  CHECK(getMSB(PedalCC::CC68).value() == 68);
  CHECK(getMSB(PedalCC::CC30).value() == 30);
  CHECK(getMSB(PedalCC::CC64).value() == 64);

  CHECK(getLSB(PedalCC::None) == std::nullopt);
  CHECK(getLSB(PedalCC::CC01).value() == 33);
  CHECK(getLSB(PedalCC::CC20).value() == 52);
  CHECK(getLSB(PedalCC::CC30).value() == 62);
  CHECK(getLSB(PedalCC::CC64) == std::nullopt);
  CHECK(getLSB(PedalCC::CC68) == std::nullopt);
}

TEST_CASE("RibbonCC to int")
{
  auto getMSB = [](RibbonCC c) { return MidiRuntimeOptionsTester::decodeEnumMSB(c); };

  auto getLSB = [](RibbonCC c) { return MidiRuntimeOptionsTester::decodeEnumLSB(c); };

  CHECK(getMSB(RibbonCC::None) == std::nullopt);
  CHECK(getMSB(RibbonCC::CC01).value() == 1);
  CHECK(getMSB(RibbonCC::CC20).value() == 20);
  CHECK(getMSB(RibbonCC::CC31).value() == 31);

  CHECK(getLSB(RibbonCC::None) == std::nullopt);
  CHECK(getLSB(RibbonCC::CC01).value() == 33);
  CHECK(getLSB(RibbonCC::CC20).value() == 52);
  CHECK(getLSB(RibbonCC::CC31).value() == 63);
}

TEST_CASE("AftertouchCC to int")
{
  auto getMSB = [](AftertouchCC c) { return MidiRuntimeOptionsTester::decodeEnumMSB(c); };

  auto getLSB = [](AftertouchCC c) { return MidiRuntimeOptionsTester::decodeEnumLSB(c); };

  CHECK(getMSB(AftertouchCC::None) == std::nullopt);
  CHECK(getMSB(AftertouchCC::PitchbendDown) == std::nullopt);
  CHECK(getMSB(AftertouchCC::PitchbendUp) == std::nullopt);
  CHECK(getMSB(AftertouchCC::ChannelPressure) == std::nullopt);

  CHECK(getMSB(AftertouchCC::CC01).value() == 1);
  CHECK(getMSB(AftertouchCC::CC20).value() == 20);

  CHECK(getLSB(AftertouchCC::None) == std::nullopt);
  CHECK(getLSB(AftertouchCC::PitchbendDown) == std::nullopt);
  CHECK(getLSB(AftertouchCC::PitchbendUp) == std::nullopt);
  CHECK(getLSB(AftertouchCC::ChannelPressure) == std::nullopt);

  CHECK(getLSB(AftertouchCC::CC01).value() == 33);
  CHECK(getLSB(AftertouchCC::CC20).value() == 52);
}

TEST_CASE("BenderCC to int")
{
  auto getMSB = [](BenderCC c) { return MidiRuntimeOptionsTester::decodeEnumMSB(c); };

  auto getLSB = [](BenderCC c) { return MidiRuntimeOptionsTester::decodeEnumLSB(c); };

  CHECK(getMSB(BenderCC::None) == std::nullopt);
  CHECK(getMSB(BenderCC::Pitchbend) == std::nullopt);
  CHECK(getMSB(BenderCC::CC01).value() == 1);
  CHECK(getMSB(BenderCC::CC20).value() == 20);

  CHECK(getLSB(BenderCC::None) == std::nullopt);
  CHECK(getLSB(BenderCC::Pitchbend) == std::nullopt);
  CHECK(getLSB(BenderCC::CC01).value() == 33);
  CHECK(getLSB(BenderCC::CC20).value() == 52);
}