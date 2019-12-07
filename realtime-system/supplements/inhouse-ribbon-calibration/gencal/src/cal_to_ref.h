#pragma once
#define CAL_TO_REF_H

#include <stdint.h>

namespace ctr
{
  // add in the revVal->dutVal mapping, if possible
  // return 0:success, 1:note, 2:warning, 3:fatal error
  int addInValues(uint16_t refVal, uint16_t dutVal);

  // return : did something
  bool doAveraging(void);
}

// EOF
