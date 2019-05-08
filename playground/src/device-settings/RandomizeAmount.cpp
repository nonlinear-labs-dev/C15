#include <device-settings/DebugLevel.h>
#include <device-settings/RandomizeAmount.h>
#include <device-settings/Settings.h>
#include <parameters/scale-converters/Linear100PercentScaleConverter.h>
#include <xml/Writer.h>

RandomizeAmount::RandomizeAmount(Settings &parent)
    : super(parent)
    , m_amount(nullptr, ScaleConverter::get<Linear100PercentScaleConverter>(), 0.1, 100, 1000)
{
}

RandomizeAmount::~RandomizeAmount()
{
}

void RandomizeAmount::load(const Glib::ustring &text)
{
  try
  {
    set(stof(text));
  }
  catch(...)
  {
    set(0);
    DebugLevel::error("Could not read settings for randomize amount:", text);
  }
}

Glib::ustring RandomizeAmount::save() const
{
  return to_string(get());
}

void RandomizeAmount::set(tControlPositionValue amount)
{
  if(m_amount.setRawValue(Initiator::INDIRECT, std::max(0.0, std::min(amount, 100.0))))
  {
    notify();
  }
}

void RandomizeAmount::setDefault()
{
  set(m_amount.getDefaultValue());
}

tControlPositionValue RandomizeAmount::get() const
{
  return m_amount.getQuantizedClipped();
}

void RandomizeAmount::incDec(int incs, ButtonModifiers mods)
{
  while(incs > 0)
  {
    m_amount.inc(Initiator::INDIRECT, mods);
    incs--;
  }

  while(incs < 0)
  {
    m_amount.dec(Initiator::INDIRECT, mods);
    incs++;
  }

  notify();
}

ustring RandomizeAmount::getDisplayString() const
{
  return m_amount.getDisplayString();
}

void RandomizeAmount::writeDocument(Writer &writer, tUpdateID knownRevision) const
{
  super::writeDocument(writer, knownRevision);

  if(knownRevision == 0)
  {
    writer.writeTextElement("default", to_string(m_amount.getDefaultValue()));
    writer.writeTextElement("scaling", m_amount.getScaleConverter()->controlPositionToDisplayJS());
    writer.writeTextElement("bipolar", to_string(m_amount.isBiPolar()));
    writer.writeTextElement("coarse-denominator", to_string(m_amount.getCoarseDenominator()));
    writer.writeTextElement("fine-denominator", to_string(m_amount.getFineDenominator()));
  }
}
