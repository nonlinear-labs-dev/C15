#pragma once
#include <nltools/Types.h>
class MidiRuntimeOptions;

class MidiRuntimeOptionsTester
{
 public:
  static std::optional<int> decodeEnumMSB(PedalCC cc);
  static std::optional<int> decodeEnumLSB(PedalCC cc);

  static std::optional<int> decodeEnumMSB(RibbonCC);
  static std::optional<int> decodeEnumLSB(RibbonCC);

  static std::optional<int> decodeEnumMSB(AftertouchCC);
  static std::optional<int> decodeEnumLSB(AftertouchCC);
  static std::optional<int> decodeEnumMSB(BenderCC);
  static std::optional<int> decodeEnumLSB(BenderCC);
};
