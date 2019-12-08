#pragma once

#include <stdint.h>

namespace ctr
{

  class CalToRef
  {
   public:
    enum Options
    {
      Quiet,
      VerboseMessages
    };
    CalToRef(Options option);
    ~CalToRef();

    void startAddIn(void);

    // add in the revVal->dutVal mapping, if possible
    // return 0:success, 1:dropped values, 2:discarded values, 3:fatal error
    int addInSamplePair(uint16_t binNumber, uint16_t values);

    void endAddIn(void);

    unsigned getValidSamples(void);  // returns number of valid sample pairs

    // return : did something
    bool doAveraging(void);

   private:
    bool m_verbose;
    unsigned m_totalSamples;
    unsigned m_validSamples;
    unsigned m_droppedSamples;
    unsigned m_discardedSamples;

    static constexpr auto m_MAX_DELTA = 100;  // maximum allowed difference for ref and dut values for a point
    static constexpr auto m_THRESHOLD = 100;  // minimum allowed value (worst-case ribbon threshold)
    static constexpr auto m_RANGE = 4096;     // (ADC) value range
    static constexpr auto m_MAX_DIST = 32;    // ???? max allowed distance between adjacent points

    struct Data
    {
      uint32_t value;
      uint32_t count;
    };

    Data m_data[m_RANGE];
    uint32_t m_lastBinNumber;
    uint16_t m_lastValue;
  };
}

// EOF
