#pragma once
#include <synth/input/InputEventStage.h>
#include <synth/C15Synth.h>

class MockDSPHost : public DSPInterface
{
 public:
  void onHWChanged(uint32_t id, float value) override;
  void onKeyDown(int note, float velocity, VoiceGroup part) override;
  void onKeyUp(int note, float velocity, VoiceGroup part) override;
  C15::Properties::HW_Return_Behavior getBehaviour(int id) override;
};

class PassOnKeyDownHost : public MockDSPHost
{
 public:
  PassOnKeyDownHost(int expectedNote, float expectedVelo, VoiceGroup expectedPart);
  void onKeyDown(int note, float velocity, VoiceGroup part) override;
  [[nodiscard]] bool didReceiveKeyDown() const;

 private:
  bool m_receivedKeyDown = false;
  const int m_note;
  const float m_vel;
  const VoiceGroup m_part;
};

class PassOnKeyUpHost : public MockDSPHost
{
 public:
  PassOnKeyUpHost(int expectedNote, float expectedVelo, VoiceGroup expectedPart);
  void onKeyUp(int note, float velocity, VoiceGroup part) override;
  [[nodiscard]] bool didReceiveKeyUp() const;

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