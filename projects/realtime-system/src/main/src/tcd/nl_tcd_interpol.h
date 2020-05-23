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

// Data structure containing the lookup and interpolation info
typedef struct
{
  int      points;    ///< number of points in the table, for both arrays !
  int16_t *x_values;  ///< x-values (input) must be sorted / monotonic !!
  int16_t *y_values;  ///< y-values (output) can be arbitrary
} LIB_interpol_data_T;

// Table lookup with interpolation
int16_t LIB_InterpolateValue(LIB_interpol_data_T *table, int16_t x);
