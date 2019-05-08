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
  return "((cpValue == 1) ? '1' + (withUnit ? ' voice' : '') + ' (off)' : formatDimensionDigits(cpValue, 'voices', "
         "withUnit, 2))";
}

Glib::ustring VoicesDimension::stringize(const tDisplayValue &displayValue) const
{
  if(displayValue == 1)
    return "1 voice (off)";

  char txt[256];
  sprintf(txt, "%d voices", (int) displayValue);
  return txt;
}
