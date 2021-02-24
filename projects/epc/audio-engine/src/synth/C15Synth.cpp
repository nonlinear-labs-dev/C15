#include "C15Synth.h"
#include "AudioEngineOptions.h"
#include "c15-audio-engine/dsp_host_dual.h"
#include <glibmm/main.h>
#include <nltools/logging/Log.h>
#include <nltools/messaging/Message.h>

constexpr static u_int8_t MIDI_SYSEX_PATTERN = 0b11110000;
constexpr static u_int8_t MIDI_NOTE_OFF_PATTERN = 0b10000000;  //rechten bits ignorieren da channel
constexpr static u_int8_t MIDI_NOTE_ON_PATTERN = 0b10010000;
constexpr static u_int8_t MIDI_CHANNEL_MASK = 0b00001111;
constexpr static u_int8_t MIDI_CHANNEL_AFTERTOUCH_PATTERN = 0b11010000;
constexpr static u_int8_t MIDI_CONTROLCHANGE_PATTERN = 0b10110000;
constexpr static u_int8_t MIDI_PITCHBEND_PATTERN = 0b11100000;
constexpr static u_int8_t MIDI_PROGRAMCHANGE_PATTERN = 0b11000000;
constexpr static u_int8_t MIDI_CONTROL_CHANGE_HIGH_RES_VELOCITY = 0b01011000;
constexpr static u_int8_t MIDI_EVENT_TYPE_MASK = 0b11110000;
constexpr static u_int8_t MIDI_CHANNEL_OMNI = 16;
constexpr static auto TCD_PATTERN = 0b11100000;
constexpr static auto TCD_TYPE_MASK = 0b00001111;

C15Synth::C15Synth(AudioEngineOptions* options)
    : Synth(options)
    , m_dsp(std::make_unique<dsp_host_dual>())
    , m_options(options)
    , m_syncExternalsTask(std::async(std::launch::async, [this] { syncExternals(); }))
{
  m_hwSourceValues.fill(0);

  m_dsp->init(options->getSampleRate(), options->getPolyphony());

  using namespace nltools::msg;

  receive<SinglePresetMessage>(EndPoint::AudioEngine, sigc::mem_fun(this, &C15Synth::onSinglePresetMessage));
  receive<LayerPresetMessage>(EndPoint::AudioEngine, sigc::mem_fun(this, &C15Synth::onLayerPresetMessage));
  receive<SplitPresetMessage>(EndPoint::AudioEngine, sigc::mem_fun(this, &C15Synth::onSplitPresetMessage));

  receive<UnmodulateableParameterChangedMessage>(EndPoint::AudioEngine,
                                                 sigc::mem_fun(this, &C15Synth::onUnmodulateableParameterMessage));
  receive<ModulateableParameterChangedMessage>(EndPoint::AudioEngine,
                                               sigc::mem_fun(this, &C15Synth::onModulateableParameterMessage));
  receive<MacroControlChangedMessage>(EndPoint::AudioEngine,
                                      sigc::mem_fun(this, &C15Synth::onMacroControlParameterMessage));
  receive<HWSourceChangedMessage>(EndPoint::AudioEngine, sigc::mem_fun(this, &C15Synth::onHWSourceMessage));
  receive<HWAmountChangedMessage>(EndPoint::AudioEngine, sigc::mem_fun(this, &C15Synth::onHWAmountMessage));

  //Settings
  receive<Setting::NoteShiftMessage>(EndPoint::AudioEngine, sigc::mem_fun(this, &C15Synth::onNoteShiftMessage));
  receive<Setting::PresetGlitchMessage>(EndPoint::AudioEngine, sigc::mem_fun(this, &C15Synth::onPresetGlitchMessage));
  receive<Setting::TransitionTimeMessage>(EndPoint::AudioEngine,
                                          sigc::mem_fun(this, &C15Synth::onTransitionTimeMessage));
  receive<Setting::EditSmoothingTimeMessage>(EndPoint::AudioEngine,
                                             sigc::mem_fun(this, &C15Synth::onEditSmoothingTimeMessage));

  receive<Setting::TuneReference>(EndPoint::AudioEngine, sigc::mem_fun(this, &C15Synth::onTuneReferenceMessage));

  receive<Keyboard::NoteUp>(EndPoint::AudioEngine, [this](const Keyboard::NoteUp& noteUp) {
    m_dsp->onMidiMessage(0, static_cast<uint32_t>(noteUp.m_keyPos), 0);
    m_syncExternalsWaiter.notify_all();
  });

  receive<Keyboard::NoteDown>(EndPoint::AudioEngine, [this](const Keyboard::NoteDown& noteDown) {
    m_dsp->onMidiMessage(100, static_cast<uint32_t>(noteDown.m_keyPos), 0);
    m_syncExternalsWaiter.notify_all();
  });

  // receive program changes from playground and dispatch it to midi-over-ip
  receive<nltools::msg::Midi::ProgramChangeMessage>(EndPoint::AudioEngine, [this](const auto& pc) {
    const uint8_t newStatus = MIDI_PROGRAMCHANGE_PATTERN | m_midiOptions.getSendChannel();
    m_externalMidiOutBuffer.push(nltools::msg::Midi::SimpleMessage { newStatus, pc.program });
    m_syncExternalsWaiter.notify_all();
  });

  receive<nltools::msg::Midi::SimpleMessage>(EndPoint::ExternalMidiOverIPClient, [&](const auto& msg) {
    MidiEvent e;
    std::copy(msg.rawBytes.data(), msg.rawBytes.data() + msg.numBytesUsed, e.raw);

    if((e.raw[0] & 0xF0) == 0xC0 && m_midiOptions.shouldReceiveProgramChanges())
    {
      // receive program changes midi-over-ip and dispatch it to playground
      send(nltools::msg::EndPoint::Playground, nltools::msg::Midi::ProgramChangeMessage { e.raw[1] });
    }
    else
    {
      pushMidiEvent(e);
    }
  });

  receive<nltools::msg::Setting::MidiSettingsMessage>(EndPoint::AudioEngine,
                                                      sigc::mem_fun(this, &C15Synth::onMidiSettingsMessage));
}

C15Synth::~C15Synth()
{
  {
    std::unique_lock<std::mutex> lock(m_syncExternalsMutex);
    m_quit = true;
    m_syncExternalsWaiter.notify_all();
  }
  m_syncExternalsTask.wait();
}

dsp_host_dual* C15Synth::getDsp() const
{
  return m_dsp.get();
}

void C15Synth::syncExternals()
{
  static_assert(std::tuple_size_v<dsp_host_dual::HWSourceValues> == std::tuple_size_v<decltype(m_hwSourceValues)>,
                "Types do not match!");

  std::unique_lock<std::mutex> lock(m_syncExternalsMutex);

  while(!m_quit)
  {
    m_syncExternalsWaiter.wait(lock);
    syncExternalMidiBridge();
    syncPlayground();
  }
}

void C15Synth::syncExternalMidiBridge()
{
  while(!m_externalMidiOutBuffer.empty())
  {
    auto msg = m_externalMidiOutBuffer.pop();
    auto copy = msg;
    if(filterMidiOutEvent(copy))
      send(nltools::msg::EndPoint::ExternalMidiOverIPBridge, copy);

    if(LOG_MIDI_OUT)
    {
      nltools::Log::info("midiOut(status: ", static_cast<uint16_t>(msg.rawBytes[0]),
                         ", data0: ", static_cast<uint16_t>(msg.rawBytes[1]),
                         ", data1: ", static_cast<uint16_t>(msg.rawBytes[2]), ")");
    }
  }
}

void C15Synth::syncPlayground()
{
  auto engineHWSourceValues = m_dsp->getHWSourceValues();
  for(size_t i = 0; i < std::tuple_size_v<dsp_host_dual::HWSourceValues>; i++)
  {
    using namespace nltools::msg;
    if(std::exchange(m_hwSourceValues[i], engineHWSourceValues[i]) != engineHWSourceValues[i])
    {
      send(EndPoint::Playground, HardwareSourceChangedNotification { i, static_cast<double>(engineHWSourceValues[i]) });
    }
  }
}

bool matchPattern(unsigned char data, uint8_t PATTERN, uint8_t MASK)
{
  return (data & MASK) == PATTERN;
}

bool isSysex(unsigned char data)
{
  return matchPattern(data, MIDI_SYSEX_PATTERN, MIDI_EVENT_TYPE_MASK);
}

bool C15Synth::filterMidiOutEvent(nltools::msg::Midi::SimpleMessage& event) const
{
  const auto statusByte = event.rawBytes[0];

  if(isSysex(statusByte))
    return false;

  const auto allowedChannel = m_midiOptions.getSendChannel();
  const auto channel = (statusByte | (MIDI_CHANNEL_MASK & allowedChannel));
  event.rawBytes[0] = channel;

  const auto isNoteEvent = matchPattern(statusByte, MIDI_NOTE_ON_PATTERN, MIDI_EVENT_TYPE_MASK)
      || matchPattern(statusByte, MIDI_NOTE_OFF_PATTERN, MIDI_EVENT_TYPE_MASK);
  const auto isAftertouchEvent = matchPattern(statusByte, MIDI_CHANNEL_AFTERTOUCH_PATTERN, MIDI_EVENT_TYPE_MASK);
  const auto isControlChangeEvent = matchPattern(statusByte, MIDI_CONTROLCHANGE_PATTERN, MIDI_EVENT_TYPE_MASK);
  const auto isPitchbendEvent = matchPattern(statusByte, MIDI_PITCHBEND_PATTERN, MIDI_EVENT_TYPE_MASK);
  const auto isProgramChangeEvent = matchPattern(statusByte, MIDI_PROGRAMCHANGE_PATTERN, MIDI_EVENT_TYPE_MASK);
  const auto sendChannelIsNone = m_midiOptions.getSendChannel() == -1;

  if(isNoteEvent && !sendChannelIsNone)
    return m_midiOptions.shouldSendNotes();

  if(isControlChangeEvent || isAftertouchEvent || isPitchbendEvent)
  {
    if(isControlChangeEvent && (int) event.rawBytes[1] == MIDI_CONTROL_CHANGE_HIGH_RES_VELOCITY)
    {
      return m_midiOptions.shouldSendNotes();
    }

    return m_midiOptions.shouldSendControllers();
  }

  if(isProgramChangeEvent)
    return m_midiOptions.shouldSendProgramChanges();

  return false;
}

bool C15Synth::filterMidiInEvent(const MidiEvent& event) const
{
  auto statusByte = event.raw[0];

  if(isSysex(statusByte))
    return false;

  const auto channel = (statusByte & MIDI_CHANNEL_MASK);
  const auto allowedChannel = m_midiOptions.getReceiveChannel();

  if(channel == allowedChannel || allowedChannel == MIDI_CHANNEL_OMNI)
  {
    const auto isNoteEvent = matchPattern(statusByte, MIDI_NOTE_ON_PATTERN, MIDI_EVENT_TYPE_MASK)
        || matchPattern(statusByte, MIDI_NOTE_OFF_PATTERN, MIDI_EVENT_TYPE_MASK);
    const auto isAftertouchEvent = matchPattern(statusByte, MIDI_CHANNEL_AFTERTOUCH_PATTERN, MIDI_EVENT_TYPE_MASK);
    const auto isControlChangeEvent = matchPattern(statusByte, MIDI_CONTROLCHANGE_PATTERN, MIDI_EVENT_TYPE_MASK);
    const auto isPitchBendEvent = matchPattern(statusByte, MIDI_PITCHBEND_PATTERN, MIDI_EVENT_TYPE_MASK);

    if(isAftertouchEvent || isControlChangeEvent || isPitchBendEvent)
    {
      return m_midiOptions.shouldReceiveControllers();
    }

    if(isNoteEvent)
    {
      return (channel == allowedChannel || allowedChannel == MIDI_CHANNEL_OMNI) && m_midiOptions.shouldReceiveNotes();
    }
  }
  return false;
}

void C15Synth::doMidi(const MidiEvent& event)
{
  if(filterMidiInEvent(event))
  {
    //[=](auto outgoingMidiMessage) { queueExternalMidiOut(outgoingMidiMessage);
    m_dsp->onMidiMessage(event.raw[0], event.raw[1], event.raw[2]);
    m_syncExternalsWaiter.notify_all();
  }
}

bool isValidTCDMessage(const MidiEvent& event)
{
  return matchPattern(event.raw[0], TCD_PATTERN, TCD_PATTERN);
}

bool C15Synth::filterTcdIn(const MidiEvent& event) const
{
  if(isValidTCDMessage(event))
  {
    const auto typeId = (event.raw[0] & TCD_TYPE_MASK);
    const auto isNoteEvent = typeId >= 13 && typeId <= 15;
    const auto isControlEvent = typeId >= 0 && typeId <= 11;

    if(isNoteEvent)
    {
      return m_midiOptions.shouldReceiveLocalNotes();
    }

    if(isControlEvent)
    {
      return m_midiOptions.shouldReceiveLocalControllers();
    }
  }
  return false;
}

void C15Synth::doTcd(const MidiEvent& event)
{
  if(filterTcdIn(event))
  {
    m_dsp->onTcdMessage(event.raw[0], event.raw[1], event.raw[2],
                        [=](auto outgoingMidiMessage) { queueExternalMidiOut(outgoingMidiMessage); });
    m_syncExternalsWaiter.notify_all();
  }
}

unsigned int C15Synth::getRenderedSamples()
{
  return m_dsp->m_sample_counter;
}

void C15Synth::doAudio(SampleFrame* target, size_t numFrames)
{
  // to avoid denormals, we exploit flush_to_zero within this code-block: every denormal float will be set to zero
  _mm_setcsr(_mm_getcsr() | (1 << 15) | (1 << 6));
  // then, the samples are rendered as before

  for(size_t i = 0; i < numFrames; i++)
  {
    m_dsp->render();
    target[i].left = m_dsp->m_mainOut_L;
    target[i].right = m_dsp->m_mainOut_R;
  }
}

void C15Synth::logStatus()
{
  m_dsp->logStatus();
}

void C15Synth::resetDSP()
{
  m_dsp->reset();
}

void C15Synth::toggleTestTone()
{
  switch(m_dsp->onSettingToneToggle())
  {
    case 0:
      nltools::Log::info("toggle TestTone: C15 only");
      break;
    case 1:
      nltools::Log::info("toggle TestTone: Tone only");
      break;
    case 2:
      nltools::Log::info("toggle TestTone: C15 and Tone");
      break;
  }
  //nltools::Log::info("TestTone is implemented before soft clipper and switches via Fadepoint now");
}

void C15Synth::selectTestToneFrequency()
{
  nltools::Log::info("currently disabled");
}

void C15Synth::selectTestToneAmplitude()
{
  nltools::Log::info("currently disabled");
}

void C15Synth::increase()
{
  changeSelectedValueBy(1);
}

void C15Synth::decrease()
{
  changeSelectedValueBy(-1);
}

void C15Synth::changeSelectedValueBy(int i)
{
  nltools::Log::info("currently disabled");
}

void C15Synth::onModulateableParameterMessage(const nltools::msg::ModulateableParameterChangedMessage& msg)
{
  // (fail-safe) dispatch by ParameterList
  auto element = m_dsp->getParameter(msg.parameterId);
  switch(element.m_param.m_type)
  {
    case C15::Descriptors::ParameterType::Global_Modulateable:
      m_dsp->globalParChg(element.m_param.m_index, msg);
      return;
    case C15::Descriptors::ParameterType::Local_Modulateable:
      m_dsp->localParChg(element.m_param.m_index, msg);
      return;
  }
#if LOG_FAIL
  nltools::Log::warning("invalid Modulateable_Parameter ID:", msg.parameterId);
#endif
}

void C15Synth::onUnmodulateableParameterMessage(const nltools::msg::UnmodulateableParameterChangedMessage& msg)
{
  // (fail-safe) dispatch by ParameterList
  auto element = m_dsp->getParameter(msg.parameterId);
  // further (subtype) distinction
  switch(element.m_param.m_type)
  {
    case C15::Descriptors::ParameterType::Global_Unmodulateable:
      m_dsp->globalParChg(element.m_param.m_index, msg);
      return;
    case C15::Descriptors::ParameterType::Macro_Time:
      m_dsp->globalTimeChg(element.m_param.m_index, msg);
      return;
    case C15::Descriptors::ParameterType::Local_Unmodulateable:
      // poly detection
      switch(static_cast<C15::Parameters::Local_Unmodulateables>(element.m_param.m_index))
      {
        case C15::Parameters::Local_Unmodulateables::Unison_Voices:
          m_dsp->localUnisonVoicesChg(msg);
          break;
        case C15::Parameters::Local_Unmodulateables::Mono_Grp_Enable:
          m_dsp->localMonoEnableChg(msg);
          break;
        case C15::Parameters::Local_Unmodulateables::Mono_Grp_Prio:
          m_dsp->localMonoPriorityChg(msg);
          break;
        case C15::Parameters::Local_Unmodulateables::Mono_Grp_Legato:
          m_dsp->localMonoLegatoChg(msg);
          break;
        default:
          m_dsp->localParChg(element.m_param.m_index, msg);
          break;
      }
      return;
    default:
      break;
  }
#if LOG_FAIL
  nltools::Log::warning("invalid Unmodulateable_Parameter ID:", msg.parameterId);
#endif
}

void C15Synth::onMacroControlParameterMessage(const nltools::msg::MacroControlChangedMessage& msg)
{
  // (fail-safe) dispatch by ParameterList
  auto element = m_dsp->getParameter(msg.parameterId);
  if(element.m_param.m_type == C15::Descriptors::ParameterType::Macro_Control)
  {
    m_dsp->globalParChg(element.m_param.m_index, msg);
    return;
  }
#if LOG_FAIL
  nltools::Log::warning("invalid Macro_Control ID:", msg.parameterId);
#endif
}

void C15Synth::onHWAmountMessage(const nltools::msg::HWAmountChangedMessage& msg)
{
  // (fail-safe) dispatch by ParameterList
  auto element = m_dsp->getParameter(msg.parameterId);
  if(element.m_param.m_type == C15::Descriptors::ParameterType::Hardware_Amount)
  {
    m_dsp->globalParChg(element.m_param.m_index, msg);
    return;
  }
#if LOG_FAIL
  nltools::Log::warning("invalid HW_Amount ID:", msg.parameterId);
#endif
}

void C15Synth::onHWSourceMessage(const nltools::msg::HWSourceChangedMessage& msg)
{
  // (fail-safe) dispatch by ParameterList
  auto element = m_dsp->getParameter(msg.parameterId);
  if(element.m_param.m_type == C15::Descriptors::ParameterType::Hardware_Source)
  {
    m_dsp->globalParChg(element.m_param.m_index, msg);
    m_dsp->hwSourceToMidi(element.m_param.m_index, msg.controlPosition,
                          [this](const auto& msg) { queueExternalMidiOut(msg); });

    return;
  }
#if LOG_FAIL
  nltools::Log::warning("invalid HW_Source ID:", msg.parameterId);
#endif
}

void C15Synth::queueExternalMidiOut(const dsp_host_dual::SimpleRawMidiMessage& m)
{
  m_externalMidiOutBuffer.push(m);
  m_syncExternalsWaiter.notify_all();
}

void C15Synth::onSplitPresetMessage(const nltools::msg::SplitPresetMessage& msg)
{
  m_dsp->onPresetMessage(msg);
}

void C15Synth::onSinglePresetMessage(const nltools::msg::SinglePresetMessage& msg)
{
  m_dsp->onPresetMessage(msg);
}

void C15Synth::onLayerPresetMessage(const nltools::msg::LayerPresetMessage& msg)
{
  m_dsp->onPresetMessage(msg);
}

void C15Synth::onNoteShiftMessage(const nltools::msg::Setting::NoteShiftMessage& msg)
{
  m_dsp->onSettingNoteShift(msg.m_shift);
}

void C15Synth::onPresetGlitchMessage(const nltools::msg::Setting::PresetGlitchMessage& msg)
{
  m_dsp->onSettingGlitchSuppr(msg.m_enabled);
}

void C15Synth::onTransitionTimeMessage(const nltools::msg::Setting::TransitionTimeMessage& msg)
{
  m_dsp->onSettingTransitionTime(msg.m_value);
}

void C15Synth::onEditSmoothingTimeMessage(const nltools::msg::Setting::EditSmoothingTimeMessage& msg)
{
  m_dsp->onSettingEditTime(msg.m_time);
}

void C15Synth::onTuneReferenceMessage(const nltools::msg::Setting::TuneReference& msg)
{
  m_dsp->onSettingTuneReference(static_cast<float>(msg.m_tuneReference));
}

void C15Synth::onMidiSettingsMessage(const nltools::msg::Setting::MidiSettingsMessage& msg)
{
  m_midiOptions.update(msg);
}