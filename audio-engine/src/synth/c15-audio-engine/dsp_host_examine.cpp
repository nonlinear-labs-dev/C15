#include "dsp_host_examine.h"

void tcd_log_entry::set(const uint32_t _time, const uint32_t _cmdId, const uint32_t _arg0, const uint32_t _arg1)
{
  m_time = _time;
  m_cmdId = _cmdId;
  m_arg = (_arg0 << 7) + _arg1;
}

void examine_tcd_input_log::reset()
{
  m_index = 0;
  m_length = 0;
  m_timestamp = 0;
}

void examine_tcd_input_log::tick()
{
  m_timestamp++;
}

void examine_tcd_input_log::add(const uint32_t _cmdId, const uint32_t _arg0, const uint32_t _arg1)
{
  m_entry[m_index].set(m_timestamp, _cmdId, _arg0, _arg1);
  m_timestamp = 0;
  m_index = (m_index + 1) % tcd_log_buffersize;
  m_length = std::min(m_length + 1, tcd_log_buffersize);
  m_startPos = (tcd_log_buffersize + m_index - m_length) % tcd_log_buffersize;
}

std::ostream& operator<<(std::ostream& lhs, const examine_tcd_input_log& rhs)
{
  lhs << std::endl << "TCD MIDI Input Log: (Length: " << rhs.m_length << ")" << std::endl;

  if(rhs.m_length > 0)
  {
    lhs << "(elapsed Samples\t: Command Argument, ...) - Argument is always unsigned 14 bit" << std::endl;
    uint32_t idx = rhs.m_startPos;
    for(uint32_t i = 0; i < rhs.m_length; i++)
    {
      if(rhs.m_entry[idx].m_time > 0)
      {
        lhs << std::endl;
        lhs << rhs.m_entry[idx].m_time << "\t: ";
      }
      lhs << tcd_command_names[rhs.m_entry[idx].m_cmdId] << " ";
      lhs << rhs.m_entry[idx].m_arg << ", ";
      idx = (idx + 1) % tcd_log_buffersize;
    }
    lhs << std::endl << "\nEnd of TCD MIDI Input Log" << std::endl;
  }

  return lhs;
}

std::ostream& operator<<(std::ostream& lhs, const examine_param& rhs)
{
  lhs << std::endl << "Examine Parameter (" << static_cast<uint32_t>(rhs.m_selected) << "):\n" << std::endl;
  lhs << "ID\tIndex\tSize\tClock\tPoly\tScale\tArg\tPost\tNorm" << std::endl;
  lhs << rhs.m_id << "\t" << rhs.m_index << "\t" << rhs.m_size << "\t" << rhs.m_clockType << "\t";
  lhs << rhs.m_polyType << "\t" << rhs.m_scaleId << "\t" << rhs.m_scaleArg << "\t";
  lhs << rhs.m_postId << "\t" << rhs.m_normalize << std::endl;

  lhs << "\nRender Status (per voice):\n" << std::endl;
  lhs << "Voice\tState\tPre\tSignal\t\tdx0\t\tdx1\t\tx\t\tStart\t\tDiff\t\tDest" << std::endl;
  for(uint32_t i = 0; i < rhs.m_size; i++)
  {
    lhs << i << "\t" << rhs.m_state[i] << "\t" << rhs.m_preload[i] << "\t" << rhs.mparams[i] << "\t\t";
    lhs << rhs.m_dx[i][0] << "\t\t" << rhs.m_dx[i][1] << "\t\t" << rhs.m_x[i] << "\t\t";
    lhs << rhs.m_start[i] << "\t\t" << rhs.m_diff[i] << "\t\t" << rhs.m_dest[i] << std::endl;
  }

  return lhs;
}

std::ostream& operator<<(std::ostream& lhs, const examine_signal& rhs)
{
  lhs << std::endl << "Examine Signal (" << static_cast<uint32_t>(rhs.m_selected) << "):\n" << std::endl;

  for(uint32_t i = 0; i < rhs.m_size; i++)
  {
    lhs << rhs.m_value[i] << "\t";
  }

  lhs << std::endl << "\nOutput Signal (left, right): " << rhs.m_left << ", " << rhs.m_right << std::endl;

  return lhs;
}
