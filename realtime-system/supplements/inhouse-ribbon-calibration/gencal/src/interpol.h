/******************************************************************************/
/** @file		nl_tcd_interpol.h
    @date		2019-12-02
    @version	0
    @author		KSTR
    @brief		lookup table with piece-wise linear interpolation
    @ingroup	nl_tcd_modules
*******************************************************************************/

#pragma once
#include "stdint.h"

namespace Interpol
{
  // Data structure containing the lookup and interpolation info
  typedef struct
  {
    int points;         ///< number of points in the table, for both arrays !
    int32_t *x_values;  ///< x-values (input) must be sorted / monotonic !!
    int32_t *y_values;  ///< y-values (output) can be arbitrary
  } InterpolData;

  // Table lookup with interpolation
  int32_t InterpolateValue(InterpolData *table, int32_t x);
}  // namespace

//EOF
