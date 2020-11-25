#pragma once

#include "Synth.h"
#include "c15-audio-engine/dsp_host_dual.h"
#include <nltools/messaging/Message.h>
#include <sigc++/sigc++.h>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <future>
#include <nltools/threading/BackgroundThreadWaiter.h>

namespace nltools
{
  namespace msg
  {
    struct ParameterChangedMessage;
    struct SetPresetMessage;
  }
}

class dsp_host_dual;
class AudioEngineOptions;

class C15Synth : public Synth, public sigc::trackable
{
 public:
  C15Synth(const AudioEngineOptions* options);
  ~C15Synth() override;

  void doMidi(const MidiEvent& event) override;
  void doTcd(const MidiEvent& event) override;
  void doAudio(SampleFrame* target, size_t numFrames) override;

  void resetDSP() override;
  void logStatus();
  void toggleTestTone();
  void selectTestToneFrequency();
  void selectTestToneAmplitude();
  void increase();
  void decrease();

  void changeSelectedValueBy(int i);
  void onLayerPresetMessage(const nltools::msg::LayerPresetMessage& msg);
  void onSplitPresetMessage(const nltools::msg::SplitPresetMessage& msg);
  void onSinglePresetMessage(const nltools::msg::SinglePresetMessage& msg);

  void onModulateableParameterMessage(const nltools::msg::ModulateableParameterChangedMessage& msg);
  void onUnmodulateableParameterMessage(const nltools::msg::UnmodulateableParameterChangedMessage& msg);
  void onMacroControlParameterMessage(const nltools::msg::MacroControlChangedMessage& msg);
  void onHWAmountMessage(const nltools::msg::HWAmountChangedMessage& msg);
  void onHWSourceMessage(const nltools::msg::HWSourceChangedMessage& msg);

  void onNoteShiftMessage(const nltools::msg::Setting::NoteShiftMessage& msg);
  void onPresetGlitchMessage(const nltools::msg::Setting::PresetGlitchMessage& msg);
  void onTransitionTimeMessage(const nltools::msg::Setting::TransitionTimeMessage& msg);
  void onEditSmoothingTimeMessage(const nltools::msg::Setting::EditSmoothingTimeMessage& msg);
  void onTuneReferenceMessage(const nltools::msg::Setting::TuneReference& msg);

  void simulateKeyDown(int key);
  void simulateKeyUp(int key);
  unsigned int getRenderedSamples();

  dsp_host_dual* getDsp();

 private:
  bool doIdle();
  void queueExternalMidiOut(const dsp_host_dual::SimpleRawMidiMessage& m);
  void sendExternalMidiOut();

  std::unique_ptr<dsp_host_dual> m_dsp;
  std::array<float, 8> m_hwSourceValues;
  const AudioEngineOptions* m_options;

  RingBuffer<nltools::msg::Midi::SimpleMessage, 2048> m_externalMidiOutBuffer;

  std::mutex m_externalMidiOutMutex;
  std::condition_variable m_externalMidiOutThreadWaiter;
  std::atomic<bool> m_quit { false };
  std::future<void> m_externalMidiOutThread;
};
