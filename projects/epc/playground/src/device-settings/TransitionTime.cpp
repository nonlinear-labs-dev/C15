#include <Application.h>
#include <device-settings/DebugLevel.h>
#include <device-settings/Settings.h>
#include <device-settings/TransitionTime.h>
#include <parameters/scale-converters/TransitionTimeMSScaleConverter.h>
#include <parameters/value/RawValue.h>
#include <proxies/playcontroller/PlaycontrollerProxy.h>
#include <xml/Writer.h>
#include <nltools/messaging/Message.h>
#include <proxies/audio-engine/AudioEngineProxy.h>

TransitionTime::TransitionTime(Settings &parent)
    : super(parent)
    , m_time(nullptr, ScaleConverter::get<TransitionTimeMSScaleConverter>(), 0, 100, 1000)
{
}

TransitionTime::~TransitionTime()
{
}

void TransitionTime::load(const Glib::ustring &text, Initiator initiator)
{
  try
  {
    set(std::stod(text));
  }
  catch(...)
  {
    set(0);
    DebugLevel::error("Could not read settings for transition time:", text);
  }
}

Glib::ustring TransitionTime::save() const
{
  return to_string(get());
}

void TransitionTime::set(tControlPositionValue time)
{
  if(m_time.setRawValue(Initiator::INDIRECT, std::max(0.0, std::min(time, 1.0))))
  {
    notify();
  }
}

void TransitionTime::setDefault()
{
  set(m_time.getDefaultValue());
}

void TransitionTime::syncExternals(SendReason reason) const
{
  nltools::msg::Setting::TransitionTimeMessage msg { static_cast<float>(m_time.getRawValue()) };
  Application::get().getAudioEngineProxy()->sendSettingMessage<nltools::msg::Setting::TransitionTimeMessage>(msg);
}

tControlPositionValue TransitionTime::get() const
{
  return m_time.getQuantizedClipped();
}

void TransitionTime::incDec(int incs, ButtonModifiers mods)
{
  while(incs > 0)
  {
    m_time.inc(Initiator::INDIRECT, mods);
    incs--;
  }

  while(incs < 0)
  {
    m_time.dec(Initiator::INDIRECT, mods);
    incs++;
  }

  syncExternals(SendReason::SettingChanged);
  notify();
}

Glib::ustring TransitionTime::getDisplayString() const
{
  return m_time.getDisplayString();
}

void TransitionTime::writeDocument(Writer &writer, tUpdateID knownRevision) const
{
  super::writeDocument(writer, knownRevision);

  if(knownRevision == 0)
  {
    writer.writeTextElement("default", to_string(m_time.getDefaultValue()));
    writer.writeTextElement("scaling", m_time.getScaleConverter()->controlPositionToDisplayJS());
    writer.writeTextElement("bipolar", to_string(m_time.isBiPolar()));
    writer.writeTextElement("coarse-denominator", to_string(m_time.getCoarseDenominator()));
    writer.writeTextElement("fine-denominator", to_string(m_time.getFineDenominator()));
  }
}
