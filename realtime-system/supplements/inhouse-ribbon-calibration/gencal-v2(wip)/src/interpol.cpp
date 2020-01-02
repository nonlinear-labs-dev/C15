/******************************************************************************/
/** @file		nl_tcd_interpol.c
    @date		2019-12-02
    @version	0
    @author		KSTR
    @brief		lookup table with piece-wise linear interpolation
    @ingroup	nl_tcd_modules
*******************************************************************************/

#include "interpol.h"

namespace Interpol
{
  /*****************************************************************************/
  /**	@brief  table lookup with interpolation between points
  *   @param  x value for which to calculate
  *   @return interpolated y value
  ******************************************************************************/
  float Interpol::lookUpAndInterpolate(float const x) const
  {
    if(!m_validData)
      return 0.0;
    // Check input bounds and saturate to numerical exact values if out-of-bounds
    if(x > m_xArray[m_points - 1])
      return m_yArray[m_points - 1];
    else if(x < m_xArray[0])
      return m_yArray[0];

    // Find the segment that holds x
    for(unsigned segment = 0; segment < (m_points - 1); segment++)
      if((x >= m_xArray[segment]) && (x <= m_xArray[segment + 1]))  // segment found
        return interpolateSegment(m_xArray[segment],                // x0
                                  m_yArray[segment],                // y0
                                  m_xArray[segment + 1],            // x1
                                  m_yArray[segment + 1],            // y1
                                  x);                               // x

    return 0.0;  // Should never come here !! segment not found
  }

  //
  // Returns the interpolated y-value. Saturates to y0 or y1 if x outside interval [x0, x1]
  //
  inline float Interpol::interpolateSegment(float const x0, float const y0, float const x1, float const y1,
                                            float const x) const
  {
    // return direct exact numerical values for corner cases and out-of-range input
    if(x <= x0)
      return y0;
    if(x >= x1)
      return y1;
    // do interpolation (with some computation error terms) for others
    return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
  }
}

// EOF
