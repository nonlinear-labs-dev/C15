#include "C15Synth.h"
#include "c15-audio-engine/dsp_host_dual.h"
#include "main.h"
#include "Options.h"

#include <nltools/messaging/Message.h>
#include <nltools/logging/Log.h>

C15Synth::C15Synth()
    : m_dsp(std::make_unique<dsp_host_dual>())
{
  m_dsp->init(getOptions()->getSampleRate(), getOptions()->getPolyphony());

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

  receive<Keyboard::NoteUp>(EndPoint::AudioEngine, [this](const Keyboard::NoteUp &noteUp) {
    MidiEvent keyUp;
    keyUp.raw[0] = 0;
    keyUp.raw[1] = noteUp.m_keyPos;
    keyUp.raw[2] = 0;
    doMidi(keyUp);
  });

  receive<Keyboard::NoteDown>(EndPoint::AudioEngine, [this](const Keyboard::NoteDown &noteDown) {
    MidiEvent keyDown;
    keyDown.raw[0] = 1 << 4;
    keyDown.raw[1] = noteDown.m_keyPos;
    keyDown.raw[2] = 127;
    doMidi(keyDown);
  });
}

C15Synth::~C15Synth() = default;

void C15Synth::doMidi(const MidiEvent &event)
{
#if test_inputModeFlag
  m_dsp->onRawMidiMessage(event.raw[0], event.raw[1], event.raw[2]);
#else
  m_dsp->onMidiMessage(event.raw[0], event.raw[1], event.raw[2]);
#endif
}

void C15Synth::doAudio(SampleFrame *target, size_t numFrames)
{
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

double C15Synth::measurePerformance(std::chrono::seconds time)
{
  for(int i = 0; i < getOptions()->getPolyphony(); i++)
  {
    m_dsp->onMidiMessage(1, 0, 53);
    m_dsp->onMidiMessage(5, 62, 64);
    m_dsp->onMidiMessage(1, 0, 83);
    m_dsp->onMidiMessage(5, 62, 64);
  }
  return Synth::measurePerformance(time);
}

void C15Synth::changeSelectedValueBy(int i)
{
  nltools::Log::info("currently disabled");
}

void C15Synth::onModulateableParameterMessage(const nltools::msg::ModulateableParameterChangedMessage &msg)
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

void C15Synth::onUnmodulateableParameterMessage(const nltools::msg::UnmodulateableParameterChangedMessage &msg)
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

void C15Synth::onMacroControlParameterMessage(const nltools::msg::MacroControlChangedMessage &msg)
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

void C15Synth::onHWAmountMessage(const nltools::msg::HWAmountChangedMessage &msg)
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

void C15Synth::onHWSourceMessage(const nltools::msg::HWSourceChangedMessage &msg)
{
  // (fail-safe) dispatch by ParameterList
  auto element = m_dsp->getParameter(msg.parameterId);
  if(element.m_param.m_type == C15::Descriptors::ParameterType::Hardware_Source)
  {
    m_dsp->globalParChg(element.m_param.m_index, msg);
    return;
  }
#if LOG_FAIL
  nltools::Log::warning("invalid HW_Source ID:", msg.parameterId);
#endif
}

void C15Synth::onSplitPresetMessage(const nltools::msg::SplitPresetMessage &msg)
{
  m_dsp->onPresetMessage(msg);
}

void C15Synth::onSinglePresetMessage(const nltools::msg::SinglePresetMessage &msg)
{
  m_dsp->onPresetMessage(msg);
}

void C15Synth::onLayerPresetMessage(const nltools::msg::LayerPresetMessage &msg)
{
  m_dsp->onPresetMessage(msg);
}

void C15Synth::onNoteShiftMessage(const nltools::msg::Setting::NoteShiftMessage &msg)
{
  m_dsp->onSettingNoteShift(static_cast<float>(msg.m_shift));
}

void C15Synth::onPresetGlitchMessage(const nltools::msg::Setting::PresetGlitchMessage &msg)
{
  m_dsp->onSettingGlitchSuppr(msg.m_enabled);
}

void C15Synth::onTransitionTimeMessage(const nltools::msg::Setting::TransitionTimeMessage &msg)
{
  m_dsp->onSettingTransitionTime(msg.m_value);
}

void C15Synth::onEditSmoothingTimeMessage(const nltools::msg::Setting::EditSmoothingTimeMessage &msg)
{
  m_dsp->onSettingEditTime(msg.m_time);
}

void C15Synth::onTuneReferenceMessage(const nltools::msg::Setting::TuneReference &msg)
{
  m_dsp->onSettingTuneReference(static_cast<float>(msg.m_tuneReference));
}
