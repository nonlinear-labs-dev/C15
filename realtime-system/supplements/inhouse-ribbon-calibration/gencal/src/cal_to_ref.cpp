#include <stdio.h>
#include <math.h>
#include "cal_to_ref.h"

namespace ctr
{
  static uint32_t accu[4096];
  static uint16_t cnt[4096];
#define MAX_DELTA (100)  // maximum allowed difference for ref and dut values for a point
#define THRESHOLD (100)  // minimum allowed value (worst-case ribbon threshold)
#define MAX_VAL (4096)   // maximum allowed valued (ADC max)

  int addInValues(uint16_t refVal, uint16_t dutVal)
  {
    if(refVal > MAX_VAL || dutVal > MAX_VAL)
    {
      puts("ERROR: Value(s) out of range 0...MAX_VAL!");
      return 3;  // fatal: values out of range for array index
    }

    if(refVal < THRESHOLD || dutVal < THRESHOLD)
    {
      puts("NOTE: Input discarded, value(s) lower than worst-case ribbon threshold.");
      return 1;
    }

    if(abs((int(refVal) - int(dutVal))) > MAX_DELTA)
    {
      puts("WARNING: Input discarded, difference is too large.");
      return 2;
    }

    if(cnt[refVal] >= UINT16_MAX)
    {  // do not add in value if counter would overflow
      puts("WARNING: Input discarded, accumulator overflow.");
      return 2;
    }

    accu[refVal] += dutVal;  // accumulate in value (averaging is done later)
    ++cnt[refVal];           // increase denominator for averaging

    return 0;
  }

  bool doAveraging(void)
  {
    bool didSomething = false;
    for(int i = 0; i < MAX_VAL; i++)
    {
      if(cnt[i])  // populated bin ?
      {
        accu[i] /= cnt[i];
        didSomething = true;
      }
    }
    return didSomething;
  }

}  //namespace

// EOF
