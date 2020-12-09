#include <Application.h>
#include <device-settings/DebugLevel.h>
#include <device-settings/EncoderAcceleration.h>
#include <device-settings/Settings.h>
#include <parameters/scale-converters/Linear100PercentScaleConverter.h>
#include <parameters/value/RawValue.h>
#include <parameters/ValueRange.h>
#include <proxies/playcontroller/PlaycontrollerProxy.h>
#include <xml/Writer.h>

EncoderAcceleration::EncoderAcceleration(Settings &parent)
    : super(parent)
    , m_acceleration(nullptr, ScaleConverter::get<Linear100PercentScaleConverter>(), 50, 100, 1000)
{
}

EncoderAcceleration::~EncoderAcceleration()
{
}

void EncoderAcceleration::load(const Glib::ustring &text, Initiator initiator)
{
  try
  {
    set(std::stod(text));
  }
  catch(...)
  {
    set(0);
    DebugLevel::error("Could not read settings for encoder acceleration:", text);
  }
}

Glib::ustring EncoderAcceleration::save() const
{
  return to_string(get());
}

void EncoderAcceleration::set(tControlPositionValue amount)
{
  amount = m_acceleration.getScaleConverter()->getControlPositionRange().clip(amount);

  if(m_acceleration.setRawValue(Initiator::INDIRECT, amount))
  {
    notify();
  }
}

void EncoderAcceleration::syncExternals(SendReason reason) const
{
}

void EncoderAcceleration::setDefault()
{
  set(m_acceleration.getDefaultValue());
}

tControlPositionValue EncoderAcceleration::get() const
{
  return m_acceleration.getQuantizedClipped();
}

void EncoderAcceleration::incDec(int incs, ButtonModifiers modifiers)
{
  while(incs > 0)
  {
    m_acceleration.inc(Initiator::INDIRECT, modifiers);
    incs--;
  }

  while(incs < 0)
  {
    m_acceleration.dec(Initiator::INDIRECT, modifiers);
    incs++;
  }

  notify();
  syncExternals(SendReason::SettingChanged);
}

Glib::ustring EncoderAcceleration::getDisplayString() const
{
  return m_acceleration.getDisplayString();
}

void EncoderAcceleration::writeDocument(Writer &writer, tUpdateID knownRevision) const
{
  super::writeDocument(writer, knownRevision);

  if(knownRevision == 0)
  {
    writer.writeTextElement("default", to_string(m_acceleration.getDefaultValue()));
    writer.writeTextElement("scaling", m_acceleration.getScaleConverter()->controlPositionToDisplayJS());
    writer.writeTextElement("bipolar", to_string(m_acceleration.isBiPolar()));
    writer.writeTextElement("coarse-denominator", to_string(m_acceleration.getCoarseDenominator()));
    writer.writeTextElement("fine-denominator", to_string(m_acceleration.getFineDenominator()));
  }
}
