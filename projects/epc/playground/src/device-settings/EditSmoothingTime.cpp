#include <Application.h>
#include <device-settings/DebugLevel.h>
#include <device-settings/Settings.h>
#include <device-settings/EditSmoothingTime.h>
#include <parameters/scale-converters/EditSmoothingTimeMSScaleConverter.h>
#include <parameters/value/RawValue.h>
#include <proxies/playcontroller/PlaycontrollerProxy.h>
#include <xml/Writer.h>
#include <nltools/messaging/Message.h>
#include <proxies/audio-engine/AudioEngineProxy.h>

EditSmoothingTime::EditSmoothingTime(Settings &parent)
    : super(parent)
    , m_time(nullptr, ScaleConverter::get<EditSmoothingTimeMSScaleConverter>(), 0, 100, 1000)
{
}

EditSmoothingTime::~EditSmoothingTime()
{
}

void EditSmoothingTime::load(const Glib::ustring &text, Initiator initiator)
{
  try
  {
    set(std::stod(text));
  }
  catch(...)
  {
    set(0);
    DebugLevel::error("Could not read settings for edit smoothing time:", text);
  }
}

Glib::ustring EditSmoothingTime::save() const
{
  return to_string(get());
}

void EditSmoothingTime::set(tControlPositionValue time)
{
  if(m_time.setRawValue(Initiator::INDIRECT, std::max(0.0, std::min(time, 1.0))))
  {
    notify();
  }
}

void EditSmoothingTime::setDefault()
{
  set(m_time.getDefaultValue());
}

void EditSmoothingTime::syncExternals(SendReason reason) const
{
  nltools::msg::Setting::EditSmoothingTimeMessage msg { static_cast<float>(m_time.getRawValue()) };
  Application::get().getAudioEngineProxy()->sendSettingMessage<nltools::msg::Setting::EditSmoothingTimeMessage>(msg);
}

tControlPositionValue EditSmoothingTime::get() const
{
  return m_time.getQuantizedClipped();
}

void EditSmoothingTime::incDec(int incs, ButtonModifiers modifiers)
{
  while(incs > 0)
  {
    m_time.inc(Initiator::INDIRECT, modifiers);
    incs--;
  }

  while(incs < 0)
  {
    m_time.dec(Initiator::INDIRECT, modifiers);
    incs++;
  }

  syncExternals(SendReason::SettingChanged);
  notify();
}

Glib::ustring EditSmoothingTime::getDisplayString() const
{
  return m_time.getDisplayString();
}

void EditSmoothingTime::writeDocument(Writer &writer, tUpdateID knownRevision) const
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
