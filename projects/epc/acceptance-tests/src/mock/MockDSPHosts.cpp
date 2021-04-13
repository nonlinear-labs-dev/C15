#include "MockDSPHosts.h"
#include <catch.hpp>

void MockDSPHost::onHWChanged(const uint32_t id, float value)
{
}

void MockDSPHost::onKeyDown(const int note, float velocity, InputEvent from)
{
}

void MockDSPHost::onKeyUp(const int note, float velocity, InputEvent from)
{
}

void MockDSPHost::onMidiSettingsReceived()
{
}

C15::Properties::HW_Return_Behavior MockDSPHost::getBehaviour(int id)
{
  return C15::Properties::HW_Return_Behavior::Zero;
}

SoundType MockDSPHost::getType()
{
  return m_type;
}

VoiceGroup MockDSPHost::getSplitPartForKey(int key)
{
  return VoiceGroup::I;
}

void MockDSPHost::onKeyDownSplit(const int note, float velocity, VoiceGroup part, DSPInterface::InputEvent from)
{
}

void MockDSPHost::onKeyUpSplit(const int note, float velocity, VoiceGroup part, DSPInterface::InputEvent from)
{
}

void MockDSPHost::setType(SoundType type)
{
  m_type = type;
}

PassOnKeyDownHost::PassOnKeyDownHost(const int expectedNote, float expectedVelo, VoiceGroup expectedPart)
    : m_note { expectedNote }
    , m_vel { expectedVelo }
    , m_part { expectedPart }
{
}

void PassOnKeyDownHost::onKeyDown(const int note, float velocity, InputEvent from)
{
  CHECK(m_note == note);
  CHECK(m_vel == velocity);
  m_receivedKeyDown = true;
}

void PassOnKeyDownHost::onKeyDownSplit(const int note, float velocity, VoiceGroup part, DSPInterface::InputEvent from)
{
  CHECK(m_note == note);
  CHECK(m_vel == velocity);
  m_receivedKeyDown = true;
}

bool PassOnKeyDownHost::didReceiveKeyDown() const
{
  return m_receivedKeyDown;
}

VoiceGroup PassOnKeyDownHost::getSplitPartForKey(int key)
{
  return VoiceGroup::Global;
}

PassOnKeyUpHost::PassOnKeyUpHost(const int expectedNote, float expectedVelo, VoiceGroup expectedPart)
    : m_note { expectedNote }
    , m_vel { expectedVelo }
    , m_part { expectedPart }
{
}

VoiceGroup PassOnKeyUpHost::getSplitPartForKey(int key)
{
  return VoiceGroup::Global;
}

void PassOnKeyUpHost::onKeyUp(const int note, float velocity, InputEvent from)
{
  CHECK(m_note == note);
  CHECK(m_vel == velocity);
  m_receivedKeyUp = true;
}

void PassOnKeyUpHost::onKeyUpSplit(const int note, float velocity, VoiceGroup part, DSPInterface::InputEvent from)
{
  CHECK(m_note == note);
  CHECK(m_vel == velocity);
  m_receivedKeyUp = true;
}

bool PassOnKeyUpHost::didReceiveKeyUp() const
{
  return m_receivedKeyUp;
}

PassOnHWReceived::PassOnHWReceived(int expectedId, float expectedValue)
    : m_id { expectedId }
    , m_value { expectedValue }
{
}

void PassOnHWReceived::onHWChanged(const uint32_t id, float value)
{
  CHECK(m_id == id);
  CHECK(m_value == value);
  m_receivedHW = true;
}

bool PassOnHWReceived::didReceiveHW() const
{
  return m_receivedHW;
}

void ConfigureableDSPHost::onHWChanged(uint32_t id, float value)
{
  if(m_onHWChanged)
    m_onHWChanged(id, value);
}

void ConfigureableDSPHost::onKeyDown(const int note, float velocity, DSPInterface::InputEvent from)
{
  if(m_onKeyDown)
    m_onKeyDown(note, velocity, from);
}

void ConfigureableDSPHost::onKeyUp(const int note, float velocity, DSPInterface::InputEvent from)
{
  if(m_onKeyUp)
    m_onKeyUp(note, velocity, from);
}

C15::Properties::HW_Return_Behavior ConfigureableDSPHost::getBehaviour(int id)
{
  if(m_getBehaviour)
    return m_getBehaviour(id);
  return C15::Properties::HW_Return_Behavior::Stay;
}

VoiceGroup ConfigureableDSPHost::getSplitPartForKey(int key)
{
  if(m_getSplitPartForKey)
    return m_getSplitPartForKey(key);

  return VoiceGroup::I;
}

void ConfigureableDSPHost::onKeyDownSplit(const int note, float velocity, VoiceGroup part,
                                          DSPInterface::InputEvent from)
{
  if(m_onKeyDownSplit)
    m_onKeyDownSplit(note, velocity, part, from);
}
void ConfigureableDSPHost::onKeyUpSplit(const int note, float velocity, VoiceGroup part, DSPInterface::InputEvent from)
{
  if(m_onKeyUpSplit)
    m_onKeyUpSplit(note, velocity, part, from);
}

void ConfigureableDSPHost::setOnHWChangedCB(std::function<void(uint32_t, float)>&& cb)
{
  m_onHWChanged = cb;
}

void ConfigureableDSPHost::setGetSplitPartForKeyCB(std::function<VoiceGroup(int)>&& cb)
{
  m_getSplitPartForKey = cb;
}

void ConfigureableDSPHost::setOnKeyDownSplitCB(std::function<void(int, float, VoiceGroup, InputEvent)>&& cb)
{
  m_onKeyDownSplit = cb;
}

void ConfigureableDSPHost::setGetBehaviourCB(std::function<C15::Properties::HW_Return_Behavior(int)>&& cb)
{
  m_getBehaviour = cb;
}

void ConfigureableDSPHost::setOnKeyUpCB(std::function<void(int, float, InputEvent)>&& cb)
{
  m_onKeyUp = cb;
}

void ConfigureableDSPHost::setOnKeyDownCB(std::function<void(int, float, InputEvent)>&& cb)
{
  m_onKeyDown = cb;
}

void ConfigureableDSPHost::setOnKeyUpSplitCB(std::function<void(int, float, VoiceGroup, InputEvent)>&& cb)
{
  m_onKeyUpSplit = cb;
}
