/******************************************************************************/
/** @file     interpol.h
    @date     2019-12-15
    @version  0
    @author   KSTR
    @brief    lookup table with piece-wise linear interpolation
*******************************************************************************/

#pragma once

namespace Interpol
{
  // Data structure containing the lookup and interpolation info
  class Interpol
  {
   public:
    /// @brief Interpol(unsigned const numberOfPoints, float const *const xArray, float const *const yArray)
    Interpol(unsigned const numberOfPoints, float const *const xArray, float const *const yArray)
        : m_points(numberOfPoints)
        , m_xArray(xArray)
        , m_yArray(yArray)
    {  // Sanity checks
      m_validData = !(m_points < 2 || m_xArray == nullptr || m_yArray == nullptr);
    }

    float lookUpAndInterpolate(float const x) const;

   private:
    float interpolateSegment(float const x0, float const y0, float const x1, float const y1, float const x) const;
    unsigned const m_points;      ///< number of points in the table, for both arrays !
    float const *const m_xArray;  ///< x-values (input) must be sorted / monotonic !!
    float const *const m_yArray;  ///< y-values (output) can be arbitrary
    bool m_validData = false;
  };

}  // namespace

//EOF
