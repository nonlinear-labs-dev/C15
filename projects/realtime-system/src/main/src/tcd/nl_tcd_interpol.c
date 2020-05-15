/******************************************************************************/
/** @file		nl_tcd_interpol.c
    @date		2019-12-02
    @version	0
    @author		KSTR
    @brief		lookup table with piece-wise linear interpolation
    @ingroup	nl_tcd_modules
*******************************************************************************/

#include "nl_tcd_interpol.h"

// Returns the interpolated y-value. Saturates to y0 or y1 if x outside interval [x0, x1]
static inline int16_t InterpolateSegment(int16_t const x0, int16_t const y0,
                                         int16_t const x1, int16_t const y1, int16_t const x)
{
  if (x <= x0)
    return y0;
  if (x >= x1)
    return y1;
  return y0 + (int16_t)((int32_t)(x - x0) * (int32_t)(y1 - y0) / (int32_t)(x1 - x0));
}

/*****************************************************************************/
/**	@brief  table lookup with interpolation
*   @param  table pointer to interolation table data structure
*   @param  x value for which to calculate
*   @return interpolated y value
******************************************************************************/
int16_t LIB_InterpolateValue(LIB_interpol_data_T *table, int16_t x)
{
  // Sanity checks
  if (!table)  // no table supplied ?
    return x;
  if (table->points < 2)  // no enough points in table ?
    return x;
  if (!table->x_values || !table->y_values)  // no data point arrays supplied ?
    return x;

  // Check input bounds and saturate if out-of-bounds
  if (x > (table->x_values[table->points - 1]))
    return table->y_values[table->points - 1];
  else if (x < (table->x_values[0]))
    return table->y_values[0];

  // Find the segment that holds x
  for (int segment = 0; segment < (table->points - 1); segment++)
    if ((x >= table->x_values[segment])
        && (x <= table->x_values[segment + 1]))                // segment found
      return InterpolateSegment(table->x_values[segment],      // x0
                                table->y_values[segment],      // y0
                                table->x_values[segment + 1],  // x1
                                table->y_values[segment + 1],  // y1
                                x);                            // x
  // Should never come here !!
  return table->y_values[table->points - 1];  // segment not found, return last y in table
}

/* Usage :

// interpolation data arrays, x and y values
static int16_t table_x[] = {0, 10, 20, 30, 55, 100 };
static int16_t table_y[] = {0, 20, 40, 60, -80, 100 };

// variable holding the interpolation table
static LIB_interpol_table_T myTable =
{
    sizeof(table_x)/sizeof(table_x[0]), // Number of data points
    table_x,                            // Array of x-coordinates
    table_y                             // Array of y-coordinates
};

int main(int argc, char *argv[]) 
{
    for (int x = 0; x <= 100; x++)
        printf("%5d  %5d\n", x, LIB_InterpolateValue(&myTable, x));
    return 0;
}
*/

// EOF
