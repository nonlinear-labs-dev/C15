#include <parameters/scale-converters/LinearHerzScaleConverter.h>
#include <proxies/audio-engine/AudioEngineProxy.h>
#include <nltools/messaging/Message.h>
#include <Application.h>
#include "xml/Writer.h"
#include "TuneReference.h"

TuneReference::TuneReference(UpdateDocumentContributor& parent)
    : Setting(parent)
    , m_value(nullptr, ScaleConverter::get<LinearHerzScaleConverter>(), 0.5, 80, 800)
{
}

void TuneReference::load(const Glib::ustring& text, Initiator initiator)
{
  if(m_value.setRawValue(initiator, std::stod(text)))
  {
    notify();
  }
}

Glib::ustring TuneReference::save() const
{
  return std::to_string(m_value.getRawValue());
}

Glib::ustring TuneReference::getDisplayString() const
{
  return m_value.getDisplayString();
}

void TuneReference::incDec(int inc, ButtonModifiers mod)
{
  if(inc > 0)
  {
    while(inc-- > 0)
      m_value.inc(Initiator::EXPLICIT_OTHER, mod);
  }
  else
  {
    while(inc++ < 0)
      m_value.dec(Initiator::EXPLICIT_OTHER, mod);
  }

  notify();
}

void TuneReference::writeDocument(Writer& writer, tUpdateID knownRevision) const
{
  Setting::writeDocument(writer, knownRevision);

  if(knownRevision == 0)
  {
    writer.writeTextElement("default", to_string(m_value.getDefaultValue()));
    writer.writeTextElement("scaling", m_value.getScaleConverter()->controlPositionToDisplayJS());
    writer.writeTextElement("bipolar", to_string(m_value.isBiPolar()));
    writer.writeTextElement("coarse-denominator", to_string(m_value.getCoarseDenominator()));
    writer.writeTextElement("fine-denominator", to_string(m_value.getFineDenominator()));
  }
}

void TuneReference::syncExternals(SendReason reason) const
{
  nltools::msg::Setting::TuneReference msg { m_value.getRawValue() };
  Application::get().getAudioEngineProxy()->sendSettingMessage<nltools::msg::Setting::TuneReference>(msg);
}

Glib::ustring TuneReference::getName() const
{
  return "Tune Reference";
}
