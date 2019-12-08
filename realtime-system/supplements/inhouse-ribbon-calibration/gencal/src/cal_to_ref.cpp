#include <stdio.h>
#include <math.h>
#include "cal_to_ref.h"

namespace ctr
{
  CalToRef::CalToRef(Options option)
      : m_totalSamples(0)
      , m_droppedSamples(0)
      , m_discardedSamples(0)

  {
    verbose = (option == Verbose);
    for(auto i = 0; i < m_RANGE; i++)
    {
      m_accu[i] = 0;
      m_cnt[i] = 0;
    }
  }

  CalToRef::~CalToRef()
  {
  }

  void CalToRef::addInValuesStart(void)
  {
    if(verbose)
      puts("Reading ref and dut raw values...");
  }

  void CalToRef::addInValuesEnd(void)
  {
    if(verbose)
      printf("Reading done, %u sample pairs total, %u dropped, %u discarded\n", m_totalSamples, m_droppedSamples,
             m_discardedSamples);
  }

  int CalToRef::addInValues(uint16_t refVal, uint16_t dutVal)
  {
    if(verbose)
      printf("%5hu %5hu : ", refVal, dutVal);

    if((refVal > (m_RANGE - 1)) || (dutVal > (m_RANGE - 1)))
    {
      printf("ERROR: Value(s) out of range 0...%d!\n", m_RANGE);
      return 3;  // fatal: values out of range for array index
    }

    ++m_totalSamples;

    if((refVal < m_THRESHOLD) || (dutVal < m_THRESHOLD))
    {
      ++m_droppedSamples;
      if(verbose)
        printf("dropped, value(s) lower than worst-case ribbon threshold.\n");
      return 1;
    }

    if(abs((int(refVal) - int(dutVal))) > m_MAX_DELTA)
    {
      ++m_discardedSamples;
      if(verbose)
        printf("discarded, difference is too large.\n");
      return 2;
    }

    if(m_cnt[refVal] >= UINT16_MAX)
    {  // do not add in value if counter would overflow
      ++m_discardedSamples;
      if(verbose)
        printf("discarded, accumulator overflow.\n");
      return 2;
    }

    printf("Ok\n");

    m_accu[refVal] += dutVal;  // accumulate in value (averaging is done later)
    ++m_cnt[refVal];           // increase denominator for averaging

    return 0;
  }

  bool CalToRef::doAveraging(void)
  {
    bool didSomething = false;
    for(auto i = 0; i < m_RANGE; i++)
    {
      if(m_cnt[i])  // populated bin ?
      {
        m_accu[i] /= m_cnt[i];
        didSomething = true;
      }
    }
    return didSomething;
  }

}  //namespace

// EOF
