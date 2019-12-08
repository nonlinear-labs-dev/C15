#pragma once
#define CAL_TO_REF_H

#include <stdint.h>

namespace ctr
{

  class CalToRef
  {
   public:
    enum Options
    {
      Normal,
      Verbose
    };
    CalToRef(Options option);
    ~CalToRef();

    void addInValuesStart(void);

    // add in the revVal->dutVal mapping, if possible
    // return 0:success, 1:note, 2:warning, 3:fatal error
    int addInValues(uint16_t refVal, uint16_t dutVal);

    void addInValuesEnd(void);

    // return : did something
    bool doAveraging(void);

   private:
    bool verbose;
    unsigned m_totalSamples;
    unsigned m_droppedSamples;
    unsigned m_discardedSamples;
    static constexpr auto m_MAX_DELTA = 100;  // maximum allowed difference for ref and dut values for a point
    static constexpr auto m_THRESHOLD = 100;  // minimum allowed value (worst-case ribbon threshold)
    static constexpr auto m_RANGE = 4096;     // (ADC) value range
    uint32_t m_accu[m_RANGE];
    uint16_t m_cnt[m_RANGE];
  };
}

// EOF
