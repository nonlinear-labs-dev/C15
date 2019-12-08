#include <stdio.h>
#include <math.h>
#include "cal_to_ref.h"

namespace ctr
{
  static int inline difference(int a, int b)
  {
    return abs(a - b);
  }

  CalToRef::CalToRef(Options option)
      : m_totalSamples(0)
      , m_validSamples(0)
      , m_droppedSamples(0)
      , m_discardedSamples(0)
      , m_lastBinNumber(0)
      , m_lastValue(0)
  {
    m_verbose = (option == VerboseMessages);
    for(auto i = 0; i < m_RANGE; i++)
    {
      m_data[i].value = 0;
      m_data[i].count = 0;
    }
  }

  CalToRef::~CalToRef()
  {
  }

  void CalToRef::startAddIn(void)
  {
    if(m_verbose)
      puts("Reading ref and dut raw values...");
  }

  void CalToRef::endAddIn(void)
  {
    if(m_verbose)
      printf("Reading done, %u sample pairs total: %u valid, %u dropped, %u discarded\n", m_totalSamples,
             m_validSamples, m_droppedSamples, m_discardedSamples);
  }

  unsigned CalToRef::getValidSamples(void)
  {
    return m_validSamples;
  }

  // -------------------------------------
  int CalToRef::addInSamplePair(uint16_t binNumber, uint16_t value)
  {
    if(m_verbose)
      printf("%5hu %5hu : ", binNumber, value);

    if((binNumber >= m_RANGE) || (value >= m_RANGE))
    {
      printf("FATAL: Value(s) out of range [0,%d]!\n", m_RANGE - 1);
      return 3;  // fatal: values out of range for array index
    }

    ++m_totalSamples;

    if((binNumber < m_THRESHOLD) || (value < m_THRESHOLD))
    {  // drop useless samples
      ++m_droppedSamples;
      if(m_verbose)
        printf("dropped, value(s) lower than worst-case ribbon threshold (%d).\n", m_THRESHOLD);
      return 1;
    }

    if(int diff = difference(binNumber, value) > m_MAX_DELTA)
    {  // discard sample pairs that are not likely to be valid
      ++m_discardedSamples;
      if(m_verbose)
        printf("discarded, value difference (%d) is too large (> %d).\n", diff, m_MAX_DELTA);
      return 2;
    }

    int ret = 0;
    if(m_totalSamples > 1)
    {  // ignore this for first sample pair
      int diff = 0;
      if((diff = difference(int(binNumber), int(m_lastBinNumber))) > m_MAX_DIST)
      {  // discard value if it is too far away from last value and probably invalid
        if(m_verbose)
          printf("discarded, ref point distance (|%u-%u|=%d) is too large (> %d).\n", binNumber, m_lastBinNumber, diff,
                 m_MAX_DIST);
        ret = 2;  // will discard this sample pair later
      }
      if((diff = difference(int(value), int(m_lastValue))) > m_MAX_DIST)
      {  // discard value if it is too far away from last value and probably invalid
        if(m_verbose)
        {
          if(ret)
            printf("              ");
          printf("discarded, dut point distance (|%u-%u|=%d) is too large (> %d).\n", value, m_lastValue, diff,
                 m_MAX_DIST);
        }
        ret = 2;  // will discard this sample pair later
      }
    }
    m_lastBinNumber = binNumber;  // save previous values first so next pair ...
    m_lastValue = value;          // ... has something valid to compare with,
    if(ret)                       // then discard this pair if required
    {
      ++m_discardedSamples;
      return ret;
    }

    // now we finally have a sample pair that is likely to be valid in our context

    if((m_data[binNumber].count >= UINT32_MAX) || (m_data[binNumber].count >= (UINT32_MAX - value)))
    {  // do not add in value if data would overflow
      ++m_discardedSamples;
      if(m_verbose)
        printf("discarded, arithmetic overflow.\n");
      return 2;
    }

    if(m_verbose)
      printf("Ok\n");
    ++m_validSamples;
    m_data[binNumber].value += value;  // accumulate-in value (averaging is done later)
    ++m_data[binNumber].count;         // increase denominator for averaging

    return 0;
  }

  // -----------------------------
  bool CalToRef::doAveraging(void)
  {
    bool didSomething = false;
    for(auto i = 0; i < m_RANGE; i++)
    {
      if(m_data[i].count)  // populated bin ?
      {
        m_data[i].value /= m_data[i].count;
        if(m_verbose)
          printf("%4d --> %4d\n", i, m_data[i].value);
        didSomething = true;
      }
    }
    return didSomething;
  }

}  //namespace

// EOF
