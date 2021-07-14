#include "MidiRuntimeOptionsTester.h"
#include <MidiRuntimeOptions.h>

std::optional<int> MidiRuntimeOptionsTester::decodeEnumMSB(PedalCC cc)
{
  return MidiRuntimeOptions::decodeEnumMSB(cc);
}

std::optional<int> MidiRuntimeOptionsTester::decodeEnumLSB(PedalCC cc)
{
  return MidiRuntimeOptions::decodeEnumLSB(cc);
}

std::optional<int> MidiRuntimeOptionsTester::decodeEnumMSB(RibbonCC cc)
{
  return MidiRuntimeOptions::decodeEnumMSB(cc);
}

std::optional<int> MidiRuntimeOptionsTester::decodeEnumLSB(RibbonCC cc)
{
  return MidiRuntimeOptions::decodeEnumLSB(cc);
}

std::optional<int> MidiRuntimeOptionsTester::decodeEnumMSB(AftertouchCC cc)
{
  return MidiRuntimeOptions::decodeEnumMSB(cc);
}

std::optional<int> MidiRuntimeOptionsTester::decodeEnumLSB(AftertouchCC cc)
{
  return MidiRuntimeOptions::decodeEnumLSB(cc);
}

std::optional<int> MidiRuntimeOptionsTester::decodeEnumMSB(BenderCC cc)
{
  return MidiRuntimeOptions::decodeEnumMSB(cc);
}

std::optional<int> MidiRuntimeOptionsTester::decodeEnumLSB(BenderCC cc)
{
  return MidiRuntimeOptions::decodeEnumLSB(cc);
}
