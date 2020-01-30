#include "MidiHeartBeat.h"
#include <nltools/logging/Log.h>

MidiHeartBeat::MidiHeartBeat(const std::string &device)
{
  if(device.empty())
  {
    nltools::Log::warning("No Heartbeat device - please use '--heartbeat=device' command line option.");
  }
  else if((m_output = std::make_unique<AlsaMidiOutput>(device)))
  {
    m_timerConnection
        = Glib::MainContext::get_default()->signal_timeout().connect(sigc::mem_fun(this, &MidiHeartBeat::send), 100);
  }
}

MidiHeartBeat::~MidiHeartBeat()
{
  m_timerConnection.disconnect();
}

void encode(uint8_t *msg, int firstBit, uint64_t counter)
{
  auto lsb = msg + 1;
  auto msb = msg + 2;
  *msb = (counter >> firstBit) & 0x7F;
  *lsb = (counter >> (firstBit + 7)) & 0x7F;
}

uint64_t testDecoding(uint8_t msg[])
{
  static uint64_t readCounter = 0;
  unsigned long firstBit = (msg[0] & 0x03) * 14;

  uint64_t lsb = *(msg + 1);
  uint64_t msb = *(msg + 2);

  readCounter = (msg[0] == 0xA0) ? 0 : readCounter;

  readCounter |= (msb << firstBit);
  readCounter |= (lsb << (firstBit + 7));

  return readCounter;
}

bool MidiHeartBeat::send()
{
  using namespace std::chrono_literals;

  uint8_t message1[] = { 0xA0, 0x00, 0x00 };  // A0 = 1st of three messages => bits 0  .. 13
  uint8_t message2[] = { 0xA1, 0x00, 0x00 };  // A1 = 2nd of three messages => bits 14 .. 27
  uint8_t message3[] = { 0xA2, 0x00, 0x00 };  // A2 = 3rd of three messages => bits 28 .. 41

  encode(message1, 0, m_counter);
  encode(message2, 14, m_counter);
  encode(message3, 28, m_counter);

  m_output->send(message1, 3);
  m_output->send(message2, 3);
  m_output->send(message3, 3);

  // testDecoding(message1);
  // testDecoding(message2);
  // auto decoded = testDecoding(message3);
  // Log::warning("counter:", m_counter, " => decoded:", decoded);

  m_counter++;
  return true;
}
