#include <cmath>
#include "VoicesDimension.h"

VoicesDimension::VoicesDimension()
{
}

VoicesDimension::~VoicesDimension()
{
}

const VoicesDimension &VoicesDimension::get()
{
  static VoicesDimension s;
  return s;
}

Glib::ustring VoicesDimension::getStingizerJS() const
{
  return "((cpValue == 0) ? '1' + (withUnit ? ' voice' : '') + ' (off)' : formatDimensionDigits(cpValue, 'voices', "
         "withUnit, 2))";
}

Glib::ustring VoicesDimension::stringize(const tDisplayValue &displayValue) const
{
  if(displayValue == 0)
    return "1 voice (off)";

  auto numVoices = displayValue + 1;
  char txt[256];
  sprintf(txt, "%ld voices", std::lround(numVoices));
  return txt;
}
