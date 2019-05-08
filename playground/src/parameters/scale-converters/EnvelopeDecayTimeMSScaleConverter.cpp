#include "EnvelopeDecayTimeMSScaleConverter.h"
#include "dimension/TimeDimension.h"

EnvelopeDecayTimeMSScaleConverter::EnvelopeDecayTimeMSScaleConverter()
    : ScaleConverter(TimeDimension::get())
    , m_tcdRange(0, 16000)
    , m_displayRange(0.0, 16000.0)
{
}

EnvelopeDecayTimeMSScaleConverter::~EnvelopeDecayTimeMSScaleConverter()
{
}

tDisplayValue EnvelopeDecayTimeMSScaleConverter::controlPositionToDisplay(const tControlPositionValue &cpValue) const
{
  if(cpValue < 0.01)
    return 11.3 * cpValue;

  return 0.1 * exp(log(16000) - log(0.1) * cpValue);
}

tTcdValue EnvelopeDecayTimeMSScaleConverter::controlPositionToTcd(const tControlPositionValue &cpValue) const
{
  return m_tcdRange.scaleValueToRange(cpValue, getControlPositionRange(), true);
}

tControlPositionValue EnvelopeDecayTimeMSScaleConverter::tcdToControlPosition(tTcdValue v) const
{
  return getControlPositionRange().scaleValueToRange(v, m_tcdRange, true);
}

Glib::ustring EnvelopeDecayTimeMSScaleConverter::controlPositionToDisplayJS() const
{
  stringstream s;
  s << "var e = 0;";
  s << "if(cpValue < 0.01) e = 11.3 * cpValue;";
  s << "else e = 0.1 * Math.exp(Math.log(16000) - Math.log(0.1) * cpValue);";
  s << "return $wnd.formatDimension(e, \"" << getDimension().getStingizerJS() << "\", withUnit);";
  return s.str();
}

bool EnvelopeDecayTimeMSScaleConverter::isBiPolar() const
{
  return false;
}
