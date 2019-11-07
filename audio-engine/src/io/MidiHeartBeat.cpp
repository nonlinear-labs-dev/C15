#include "MidiHeartBeat.h"
#include <nltools/logging/Log.h>

MidiHeartBeat::MidiHeartBeat(const std::string &device)
{
  if(device.empty())
    nltools::Log::warning("No Heartbeat device - please use midi-out command line option.");
  else if(m_output = std::make_unique<AlsaMidiOutput>(device))
    send();
}

void encode(uint8_t *msg, int firstBit, int numBits, uint64_t counter)
{
  uint16_t *data = reinterpret_cast<uint16_t *>(msg + 1);
  *data |= (counter >> firstBit) & 0x3FFF;
}

void MidiHeartBeat::send()
{
  using namespace std::chrono_literals;
  // 11110100 0ttttttt 0ttttttt 11110101 0ttttttt 0ttttttt 11111001 0ttttttt 0ttttttt

  uint8_t message1[] = { 0xF4, 0x00, 0x00 };  // F4 = 1st of three messages => bits 0  .. 13
  uint8_t message2[] = { 0xF5, 0x00, 0x00 };  // F5 = 2nd of three messages => bits 14 .. 27
  uint8_t message3[] = { 0xF9, 0x00, 0x00 };  // F9 = 3rd of three messages => bits 28 .. 41

  encode(message1, 0, 14, m_counter);
  encode(message2, 14, 14, m_counter);
  encode(message3, 28, 14, m_counter);

  m_output->send(message1, 3);
  m_output->send(message2, 3);
  m_output->send(message3, 3);

  m_expiration.setCallback([=] { send(); });
  m_expiration.refresh(250ms);

  m_counter++;
}
