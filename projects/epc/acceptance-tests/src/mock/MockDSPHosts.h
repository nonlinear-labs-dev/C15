#pragma once
#include <synth/C15Synth.h>
#include <synth/input/InputEventStage.h>

class MockDSPHost : public DSPInterface
{
 public:
  void onHWChanged(uint32_t id, float value, bool didBehaviourChange) override;
  void onKeyDown(const int note, float velocity, InputEventSource from) override;
  void onKeyUp(const int note, float velocity, InputEventSource from) override;
  C15::Properties::HW_Return_Behavior getBehaviour(int id) override;
  SoundType getType() override;
  VoiceGroup getSplitPartForKeyDown(int key) override;
  VoiceGroup getSplitPartForKeyUp(int key, InputEventSource from) override;
  void onKeyDownSplit(const int note, float velocity, VoiceGroup part, InputEventSource from) override;
  void onKeyUpSplit(const int note, float velocity, VoiceGroup part, InputEventSource from) override;
  void onMidiSettingsReceived() override;
  void setType(SoundType type);

 protected:
  SoundType m_type = SoundType::Split;
};

class ConfigureableDSPHost : public MockDSPHost
{
 public:
  void onHWChanged(uint32_t id, float value, bool didChange) override;
  void onKeyDown(const int note, float velocity, InputEventSource from) override;
  void onKeyUp(const int note, float velocity, InputEventSource from) override;
  C15::Properties::HW_Return_Behavior getBehaviour(int id) override;
  VoiceGroup getSplitPartForKeyDown(int key) override;
  VoiceGroup getSplitPartForKeyUp(int key, InputEventSource from) override;
  void onKeyDownSplit(const int note, float velocity, VoiceGroup part, InputEventSource from) override;
  void onKeyUpSplit(const int note, float velocity, VoiceGroup part, InputEventSource from) override;

  void setOnHWChangedCB(std::function<void(uint32_t, float, bool)>&& cb);
  void setOnKeyDownCB(std::function<void(int, float, InputEventSource)>&& cb);
  void setOnKeyUpCB(std::function<void(int, float, InputEventSource)>&& cb);
  void setGetBehaviourCB(std::function<C15::Properties::HW_Return_Behavior(int id)>&& cb);
  void setGetSplitPartForKeyCB(std::function<VoiceGroup(int)>&& cb);
  void setOnKeyDownSplitCB(std::function<void(int, float, VoiceGroup, InputEventSource)>&& cb);
  void setOnKeyUpSplitCB(std::function<void(int, float, VoiceGroup, InputEventSource)>&& cb);

 private:
  std::function<void(uint32_t, float, bool)> m_onHWChanged;
  std::function<void(int, float, InputEventSource)> m_onKeyDown;
  std::function<void(int, float, InputEventSource)> m_onKeyUp;
  std::function<C15::Properties::HW_Return_Behavior(int id)> m_getBehaviour;
  std::function<VoiceGroup(int)> m_getSplitPartForKey;
  std::function<void(int, float, VoiceGroup, InputEventSource)> m_onKeyDownSplit;
  std::function<void(int, float, VoiceGroup, InputEventSource)> m_onKeyUpSplit;
};

class PassOnKeyDownHost : public MockDSPHost
{
 public:
  PassOnKeyDownHost(int expectedNote, float expectedVelo, VoiceGroup expectedPart);
  void onKeyDown(const int note, float velocity, InputEventSource from) override;
  void onKeyDownSplit(const int note, float velocity, VoiceGroup part, InputEventSource from) override;
  [[nodiscard]] bool didReceiveKeyDown() const;
  VoiceGroup getSplitPartForKeyDown(int key) override;
  VoiceGroup getSplitPartForKeyUp(int key, InputEventSource from) override;

 protected:
  bool m_receivedKeyDown = false;
  int m_note;
  float m_vel;
  VoiceGroup m_part;
};

class PassOnKeyUpHost : public MockDSPHost
{
 public:
  PassOnKeyUpHost(int expectedNote, float expectedVelo, VoiceGroup expectedPart);
  void onKeyUp(const int note, float velocity, InputEventSource from) override;
  [[nodiscard]] bool didReceiveKeyUp() const;
  void onKeyUpSplit(const int note, float velocity, VoiceGroup part, InputEventSource from) override;
  VoiceGroup getSplitPartForKeyDown(int key) override;
  VoiceGroup getSplitPartForKeyUp(int key, InputEventSource from) override;

 private:
  bool m_receivedKeyUp = false;
  const int m_note;
  const float m_vel;
  const VoiceGroup m_part;
};

class PassOnHWReceived : public MockDSPHost
{
 public:
  PassOnHWReceived(int expectedId, float expectedValue);
  void onHWChanged(uint32_t id, float value, bool) override;
  [[nodiscard]] bool didReceiveHW() const;
  void setExpectedHW(int hw);

 private:
  bool m_receivedHW = false;
  int m_id;
  const float m_value;
};
