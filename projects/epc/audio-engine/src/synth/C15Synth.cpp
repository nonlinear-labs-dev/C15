#include "C15Synth.h"
#include "AudioEngineOptions.h"
#include "c15-audio-engine/dsp_host_dual.h"
#include <glibmm/main.h>
#include <nltools/logging/Log.h>
#include <nltools/messaging/Message.h>

constexpr static u_int8_t MIDI_PROGRAMCHANGE_PATTERN = 0b11000000;

C15Synth::C15Synth(AudioEngineOptions* options)
    : Synth(options)
    , m_dsp(std::make_unique<dsp_host_dual>())
    , m_options(options)
    , m_externalMidiOutBuffer(2048)
    , m_syncExternalsTask(std::async(std::launch::async, [this] { syncExternalsLoop(); }))
    , m_syncPlaygroundTask(std::async(std::launch::async, [this] {syncPlaygroundLoop(); }))
    , m_inputEventStage { m_dsp.get(), &m_midiOptions, [this] {
                            m_syncPlaygroundWaiter.notify_all(); },
                          [this](auto msg) { queueExternalMidiOut(msg); } }
{
  m_playgroundHwSourceKnownValues.fill(0);

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
    m_inputEventStage.onMIDIMessage({ 0, static_cast<uint8_t>(noteUp.m_keyPos), 0 });
    m_syncExternalsWaiter.notify_all();
  });

  receive<Keyboard::NoteDown>(EndPoint::AudioEngine, [this](const Keyboard::NoteDown& noteDown) {
    m_inputEventStage.onMIDIMessage({ 100, static_cast<uint8_t>(noteDown.m_keyPos), 0 });
    m_syncExternalsWaiter.notify_all();
  });

  // receive program changes from playground and dispatch it to midi-over-ip
  receive<nltools::msg::Midi::ProgramChangeMessage>(EndPoint::AudioEngine, [this](const auto& pc) {
    const int sendChannel = m_midiOptions.channelEnumToInt(m_midiOptions.getSendChannel());
    if(sendChannel != -1 && m_midiOptions.shouldSendProgramChanges())
    {
      const uint8_t newStatus = MIDI_PROGRAMCHANGE_PATTERN | sendChannel;
      m_externalMidiOutBuffer.push(nltools::msg::Midi::SimpleMessage { newStatus, pc.program });
      m_syncExternalsWaiter.notify_all();
    }
  });

  receive<nltools::msg::Midi::SimpleMessage>(EndPoint::ExternalMidiOverIPClient, [&](const auto& msg) {
    MidiEvent e;
    std::copy(msg.rawBytes.data(), msg.rawBytes.data() + msg.numBytesUsed, e.raw);

    const auto isPC = (e.raw[0] & 0xF0) == 0xC0;
    if(isPC)
    {
      // Program Changes should be exclusively bound to Primary channel
      const auto receivedChannel = static_cast<int>(e.raw[0]) - 192;
      const auto isOmniReceive = m_midiOptions.getReceiveChannel() == MidiReceiveChannel::Omni;
      const auto receivedChannelMatches
          = m_midiOptions.channelEnumToInt(m_midiOptions.getReceiveChannel()) == receivedChannel;

      if(isOmniReceive || receivedChannelMatches)
      {
        if(m_midiOptions.shouldReceiveProgramChanges())
        {
          send(nltools::msg::EndPoint::Playground, nltools::msg::Midi::ProgramChangeMessage { e.raw[1] });
        }
      }
    }
    else
    {
      pushMidiEvent(e);
    }
  });

  receive<nltools::msg::Setting::MidiSettingsMessage>(EndPoint::AudioEngine,
                                                      sigc::mem_fun(this, &C15Synth::onMidiSettingsMessage));

  receive<nltools::msg::PanicAudioEngine>(EndPoint::AudioEngine,
                                          sigc::mem_fun(this, &C15Synth::onPanicNotificationReceived));
}

C15Synth::~C15Synth()
{
  {
    std::unique_lock<std::mutex> lock(m_syncExternalsMutex);
    std::unique_lock<std::mutex> lockPg(m_syncPlaygroundMutex);

    m_quit = true;

    m_syncExternalsWaiter.notify_all();
    m_syncPlaygroundWaiter.notify_all();
  }

  m_syncExternalsTask.wait();
  m_syncPlaygroundTask.wait();
}

dsp_host_dual* C15Synth::getDsp() const
{
  return m_dsp.get();
}

void C15Synth::syncExternalsLoop()
{
  static_assert(
      std::tuple_size_v<dsp_host_dual::HWSourceValues> == std::tuple_size_v<decltype(m_playgroundHwSourceKnownValues)>,
      "Types do not match!");

  std::unique_lock<std::mutex> lock(m_syncExternalsMutex);

  while(!m_quit)
  {
    m_syncExternalsWaiter.wait(lock);
    syncExternalMidiBridge();
  }
}

void C15Synth::syncPlaygroundLoop()
{
  std::unique_lock<std::mutex> lock(m_syncPlaygroundMutex);

  while(!m_quit)
  {
    m_syncPlaygroundWaiter.wait(lock);
    syncPlayground();
  }
}

void C15Synth::syncExternalMidiBridge()
{
  while(!m_externalMidiOutBuffer.empty())
  {
    auto msg = m_externalMidiOutBuffer.pop();
    auto copy = msg;
    send(nltools::msg::EndPoint::ExternalMidiOverIPBridge, copy);
  }
}

void C15Synth::syncPlayground()
{
  auto engineHWSourceValues = m_dsp->getHWSourceValues();
  for(size_t i = 0; i < std::tuple_size_v<dsp_host_dual::HWSourceValues>; i++)
  {
    using namespace nltools::msg;
    if(std::exchange(m_playgroundHwSourceKnownValues[i], engineHWSourceValues[i]) != engineHWSourceValues[i])
    {
      send(EndPoint::Playground, HardwareSourceChangedNotification { i, static_cast<double>(engineHWSourceValues[i]) });
    }
  }
}

void C15Synth::doMidi(const MidiEvent& event)
{
  m_inputEventStage.onMIDIMessage(event);
}

void C15Synth::doTcd(const MidiEvent& event)
{
  m_inputEventStage.onTCDMessage(event);
}

unsigned int C15Synth::getRenderedSamples()
{
  return m_dsp->m_sample_counter;
}

void C15Synth::doAudio(SampleFrame* target, size_t numFrames)
{
#ifdef __arm__
#else
  // to avoid denormals, we exploit flush_to_zero within this code-block: every denormal float will be set to zero
  _mm_setcsr(_mm_getcsr() | (1 << 15) | (1 << 6));
  // then, the samples are rendered as before
#endif

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
  auto element = m_dsp->getParameter(msg.parameterId);
  auto latchIndex = InputEventStage::parameterIDToHWID(msg.parameterId);

  if(element.m_param.m_type == C15::Descriptors::ParameterType::Hardware_Source && latchIndex != HWID::INVALID) {
    auto didBehaviourChange = m_dsp->updateBehaviour(element, msg.returnMode);
    m_playgroundHwSourceKnownValues[latchIndex] = static_cast<float>(msg.controlPosition);
    m_inputEventStage.onUIHWSourceMessage(msg, didBehaviourChange);
  }
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
  m_inputEventStage.setNoteShift(msg.m_shift);
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
  m_dsp->onMidiSettingsReceived();
}

void C15Synth::onPanicNotificationReceived(const nltools::msg::PanicAudioEngine&)
{
  resetDSP();
}
