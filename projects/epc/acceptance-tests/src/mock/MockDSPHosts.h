#pragma once
#include <synth/C15Synth.h>
#include <synth/input/InputEventStage.h>

class MockDSPHost : public DSPInterface
{
 public:
  void onHWChanged(uint32_t id, float value) override;
  void onKeyDown(const int note, float velocity, InputEvent from) override;
  void onKeyUp(const int note, float velocity, InputEvent from) override;
  C15::Properties::HW_Return_Behavior getBehaviour(int id) override;
  SoundType getType() override;
  VoiceGroup getSplitPartForKey(int key) override;
  void onKeyDownSplit(const int note, float velocity, VoiceGroup part, InputEvent from) override;
  void onKeyUpSplit(const int note, float velocity, VoiceGroup part, InputEvent from) override;
  void onMidiSettingsReceived() override;
  void setType(SoundType type);

 protected:
  SoundType m_type = SoundType::Split;
};

class ConfigureableDSPHost : public MockDSPHost
{
 public:
  void onHWChanged(uint32_t id, float value) override;
  void onKeyDown(const int note, float velocity, InputEvent from) override;
  void onKeyUp(const int note, float velocity, InputEvent from) override;
  C15::Properties::HW_Return_Behavior getBehaviour(int id) override;
  VoiceGroup getSplitPartForKey(int key) override;
  void onKeyDownSplit(const int note, float velocity, VoiceGroup part, InputEvent from) override;
  void onKeyUpSplit(const int note, float velocity, VoiceGroup part, InputEvent from) override;

  void setOnHWChangedCB(std::function<void(uint32_t, float)>&& cb);
  void setOnKeyDownCB(std::function<void(int, float, InputEvent)>&& cb);
  void setOnKeyUpCB(std::function<void(int, float, InputEvent)>&& cb);
  void setGetBehaviourCB(std::function<C15::Properties::HW_Return_Behavior(int id)>&& cb);
  void setGetSplitPartForKeyCB(std::function<VoiceGroup(int)>&& cb);
  void setOnKeyDownSplitCB(std::function<void(int, float, VoiceGroup, InputEvent)>&& cb);
  void setOnKeyUpSplitCB(std::function<void(int, float, VoiceGroup, InputEvent)>&& cb);

 private:
  std::function<void(uint32_t, float)> m_onHWChanged;
  std::function<void(int, float, InputEvent)> m_onKeyDown;
  std::function<void(int, float, InputEvent)> m_onKeyUp;
  std::function<C15::Properties::HW_Return_Behavior(int id)> m_getBehaviour;
  std::function<VoiceGroup(int)> m_getSplitPartForKey;
  std::function<void(int, float, VoiceGroup, InputEvent)> m_onKeyDownSplit;
  std::function<void(int, float, VoiceGroup, InputEvent)> m_onKeyUpSplit;
};

class PassOnKeyDownHost : public MockDSPHost
{
 public:
  PassOnKeyDownHost(int expectedNote, float expectedVelo, VoiceGroup expectedPart);
  void onKeyDown(const int note, float velocity, InputEvent from) override;
  void onKeyDownSplit(const int note, float velocity, VoiceGroup part, InputEvent from) override;
  [[nodiscard]] bool didReceiveKeyDown() const;
  VoiceGroup getSplitPartForKey(int key) override;

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
  void onKeyUp(const int note, float velocity, InputEvent from) override;
  [[nodiscard]] bool didReceiveKeyUp() const;
  void onKeyUpSplit(const int note, float velocity, VoiceGroup part, InputEvent from) override;
  VoiceGroup getSplitPartForKey(int key) override;

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
  void onHWChanged(uint32_t id, float value) override;
  [[nodiscard]] bool didReceiveHW() const;

 private:
  bool m_receivedHW = false;
  const int m_id;
  const float m_value;
};