/******************************************************************************/
/** @file		nltoolbox.h
    @date		2018-06-27
    @version	0.1
    @authors    Anton Schmied[2017-03-16], Matthias Seeber[2018-06-27]
    @brief		A collection of frequently used function for crossfading,
                conversion, etc.
*******************************************************************************/

#pragma once

#include <cmath>
#include <stdint.h>
#include <algorithm>

namespace NlToolbox
{

  /*****************************************************************************/
  /** @brief    Constants
 *            Provides single precision floats for crucial constants.
 *            This ensures being independant on math headers, which may
 *            or may not provide the desired float type.
******************************************************************************/

  namespace Constants
  {

    const float pi = 3.14159265f;         // PI as a (single precision) float
    const float twopi = 2.f * pi;         // two PI as a (single precision) float
    const float halfpi = 0.5f * pi;       // half PI as a (single precision) float
    const float sqrt_two = 1.414213562f;  // squareroot of two as a (single precision) float;

  }  // namespace Constants

  /*****************************************************************************/
  /** @brief    Crossfade Tools
******************************************************************************/

  namespace Crossfades
  {

    /*****************************************************************************/
    /** @brief    mix calculation between the raw and processed sample
 *  @param    raw Sample
 *  @param    processed sample
 *  @param    dry Amount
 *  @param    wet Amount
 *  @return   a mix of both samples
******************************************************************************/

    inline float crossFade(float _sample1, float _sample2, float _gain1, float _gain2)
    {
      return ((_sample1 * _gain1) + (_sample2 * _gain2));
    }

    /*****************************************************************************/
    /** @brief    bipolar mix calculation between two samples
 *  @param    first sample
 *  @param    second sample
 *  @param    mix amount
 *  @return   a mix of both samples
******************************************************************************/

    inline float bipolarCrossFade(float _sample1, float _sample2, float _mix)
    {
      _sample1 = (1.f - std::abs(_mix)) * _sample1;
      _sample2 = _mix * _sample2;

      return _sample1 + _sample2;
    }

    /*****************************************************************************/
    /** @brief    unipolar mix calculation between two samples
 *  @param    first sample
 *  @param    second sample
 *  @param    mix amount
 *  @return   a mix of both samples
******************************************************************************/

    inline float unipolarCrossFade(float _sample1, float _sample2, float _mix)
    {
      _sample1 = (1.f - _mix) * _sample1;
      _sample2 = _mix * _sample2;

      return _sample1 + _sample2;
    }

  }  // namespcace Crossfades

  /*****************************************************************************/
  /** @brief    Math Tools
******************************************************************************/

  namespace Math
  {

    /*****************************************************************************/
    /** @brief    sine calculation of an incoming value
 *  @param    x (expected range: [-pi ... pi])
 *  @return   sine value
******************************************************************************/

    inline float sin(float x)
    {
      float x_square = x * x;

      x = (((((x_square * -2.39e-8f + 2.7526e-6f) * x_square + (-0.000198409f)) * x_square + 0.00833333f) * x_square
            + (-0.166667f))
               * x_square
           + 1.f)
          * x;

      return x;
    }

    /*****************************************************************************/
    /** @brief    cosine calculation of an incoming value
 *  @param    x (expected range: [-pi ... pi])
 *  @return   cosine value
******************************************************************************/

    inline float cos(float x)
    {
      float x_square = x * x;

      x = (((((x_square * -2.605e-7f + 2.47609e-5f) * x_square + (-0.00138884f)) * x_square + 0.0416666f) * x_square
            + (-0.499923f))
           * x_square)
          + 1.f;

      return x;
    }

    /*****************************************************************************/
    /** @brief    tangent calculation of an incoming value
 *  @param    value
 *  @return   tangent value
******************************************************************************/

    inline float tan(float x)
    {
      x = 0.133333f * std::pow(x, 5.f) + 0.333333f * std::pow(x, 3.f) + x;

      return x;
    }

    /*****************************************************************************/
    /** @brief    arctangent calculation of an incoming value
 *  @param    value
 *  @return   arctangent value
******************************************************************************/

    inline float arctan(float x)
    {
      if(x > 1.f)
      {
        x = 1.f / x;
        float x_square = x * x;

        x = ((((((((x_square * 0.00286623f - 0.0161857f) * x_square + 0.0429096f) * x_square - 0.0752896f) * x_square
                 + 0.106563f)
                    * x_square
                - 0.142089f)
                   * x_square
               + 0.199936f)
                  * x_square
              - 0.333331f)
                 * x_square
             + 1.f)
            * x;

        x = 1.5708f - x;
      }
      else if(x < -1.f)
      {
        x = 1.f / x;
        float x_square = x * x;

        x = ((((((((x_square * 0.00286623f - 0.0161857f) * x_square + 0.0429096f) * x_square - 0.0752896f) * x_square
                 + 0.106563f)
                    * x_square
                - 0.142089f)
                   * x_square
               + 0.199936f)
                  * x_square
              - 0.333331f)
                 * x_square
             + 1.f)
            * x;

        x = -1.5708f - x;
      }
      else
      {
        float x_square = x * x;

        x = ((((((((x_square * 0.00286623f - 0.0161857f) * x_square + 0.0429096f) * x_square - 0.0752896f) * x_square
                 + 0.106563f)
                    * x_square
                - 0.142089f)
                   * x_square
               + 0.199936f)
                  * x_square
              - 0.333331f)
                 * x_square
             + 1.f)
            * x;
      }

      return x;
    }

    /*****************************************************************************/
    /** @brief    sine calculation of an incoming value - 3rd degree polynomial
 *  @param    value
 *  @return   sine value
******************************************************************************/

    inline float sinP3_wrap(float _x)
    {
      _x += -0.25f;

      //    _x = _x - round(_x);

      if(_x >= 0.f)
      {
        _x -= static_cast<int>(_x + 0.5f);
      }
      else
      {
        _x -= static_cast<int>(_x - 0.5f);
      }

      _x += _x;
      _x = std::abs(_x);
      _x = 0.5f - _x;

      float x_square = _x * _x;
      return _x * ((2.26548f * x_square - 5.13274f) * x_square + 3.14159f);
    }

    /*****************************************************************************/
    /** @brief    sine calculation of an incoming value - 3rd degree polynomial
 *            with no warping
 *  @param    value
 *  @return   sine value
******************************************************************************/

    inline float sinP3_noWrap(float _x)
    {
      _x += _x;
      _x = std::abs(_x);
      _x = 0.5f - _x;

      float x_square = _x * _x;
      return _x * ((2.26548f * x_square - 5.13274f) * x_square + 3.14159f);
    }

    /*****************************************************************************/
    /** @brief    interpolation of 4 neighbouring samples from the delay buffers
 *  @param    fractional part of the number of the delay samples
 *  @param	  sample from delay buffer with index n-1
 *  @param	  sample from delay buffer with index n
 *  @param	  sample from delay buffer with index n+1
 *  @param	  sample from delay buffer with index n+2
 *  @return	  interpolated value of the four samples
******************************************************************************/

    inline float interpolRT(float fract, float sample_tm1, float sample_t0, float sample_tp1, float sample_tp2)
    {
      float fract_square = fract * fract;
      float fract_cube = fract_square * fract;

      float a = 0.5f * (sample_tp1 - sample_tm1);
      float b = 0.5f * (sample_tp2 - sample_t0);
      float c = sample_t0 - sample_tp1;

      return sample_t0 + fract * a + fract_cube * (a + b + 2.f * c) - fract_square * (2.f * a + b + 3.f * c);
    }

    /*****************************************************************************/
    /** @brief    bell function with y = 0 at x = 0 & 1 and y = 1 at x = 0.5,
 *            expontential  when x < 0 | x > 1
 *  @param    x
 *  @return	  y
******************************************************************************/
    inline float bell(float _x)
    {
      _x = std::abs(_x - 0.5f) * 4.f - 1.f;
      return (2.f - std::abs(_x)) * _x * -0.5f + 0.5f;
    }

  }  // namespace Math

  /*****************************************************************************/
  /** @brief    Frequently used conversion functions
******************************************************************************/

  namespace Conversion
  {

    /*****************************************************************************/
    /** @brief    conversion from a value in dB in to an amplification factor
 *  @param    value in dB
 *  @return   value as amplitude factor
******************************************************************************/

    inline float db2af(float dbIn)
    {
      return std::pow(1.12202f, dbIn);
    }

    /*****************************************************************************/
    /** @brief    conversion from a value as amplitude factor into dB
 *  @param    value as amplitude factor
 *  @return   value in dB
******************************************************************************/

    inline float af2db(float afIn)
    {
      if(afIn == 0.f)  // vorsichthalber gegen ne = schützen
        afIn = 1e15f;

      return 20.f * std::log10(afIn);  // gibt es eine feine annäherung?
    }

    /*****************************************************************************/
    /** @brief    conversion from pitch to frequency
 *  @param    pitch value
 *  @return   frequency value
******************************************************************************/

    inline float pitch2freq(float pitch)
    {
      /// Beide der filgenden Ansätze bedienen sich der pow()- Funtkion
      /// Zur Optimierung sollen diese mit einem Polynom oder
      /// einem Tabellen-Ansatz ersetzt werden!

      return std::pow(2.f, (pitch - 69.f) / 12.f) * 440.f;

      //    return pow(1.05946309f, pitch) * 8.17579892f;         // alternative
    }

    /*****************************************************************************/
    /** @brief    conversion from float to int using casting within an expression
 *  @param    float value
 *  @return   int value
******************************************************************************/

    inline int float2int(float _value)
    {
      /// cmath approach
      //    return round(_value);

      /// Statement approach
      //    return (_value >= 0.f)
      //           ? static_cast<int>(_value + 0.5f)
      //           : static_cast<int>(_value - 0.5f);

      /// Expression approach
      if(_value >= 0.f)
      {
        return static_cast<int>(_value + 0.5f);
      }
      else
      {
        return static_cast<int>(_value - 0.5f);
      }
    }

  }  // namespace Conversion

  /*****************************************************************************/
  /** @brief    Other Tools (might move to a more specific namespaces)
******************************************************************************/

  namespace Others
  {

    /*****************************************************************************/
    /** @brief
 *  @param
 *  @param
 *  @param
 *  @return
******************************************************************************/

    inline float threeRanges(float sample, float ctrlSample, float foldAmnt)
    {
      if(ctrlSample < -0.25f)
      {
        sample = (sample + 1.f) * foldAmnt + (-1.f);
      }
      else if(ctrlSample > 0.25f)
      {
        sample = (sample + (-1.f)) * foldAmnt + 1.f;
      }

      return sample;
    }

    /*****************************************************************************/
    /** @brief
 *  @param
 *  @param
 *  @param
 *  @return
******************************************************************************/

    inline float parAsym(float sample, float sample_square, float asymAmnt)
    {

      return ((1.f - asymAmnt) * sample) + (2 * asymAmnt * sample_square);
    }

  }  // namespace Others

  /*****************************************************************************/
  /** @brief  Curve tools
******************************************************************************/

  namespace Curves
  {

    /*****************************************************************************/
    /** @brief    Control shaper for the range from -1 to +1. It increases the
 *            resolution near to the range limits
 *  @param    value
 *  @return   resulting value
******************************************************************************/

    inline float applySineCurve(float _in)
    {
      _in = (_in * 2.f) + (-1.f);

      return _in * _in * _in * (-0.25f) + (_in * 0.75f) + 0.5f;
    }

    /*****************************************************************************/
    /** @brief    Shaper for control signals. One breakpoint.
 *            [0], [50] and [100] adjust the levels for [in] = 0,
 *            [in] = 0.5(breakpoint) and [in] = 1.
 *            Matthias: renamed object, fixed bug for values (0.5 ... 1.0)
******************************************************************************/

    struct Shaper_1_BP
    {
      float m_Factor1, m_Factor2;
      float m_Startpoint, m_Breakpoint, m_Endpoint;
      float m_Split = 1.f / 2.f;

      void setCurve(float _startpoint, float _breakpoint, float _endpoint)
      {
        m_Factor1 = (_breakpoint - _startpoint) / (m_Split - 0.f);
        m_Factor2 = (_endpoint - _breakpoint) / (1.f - m_Split);

        m_Startpoint = _startpoint;
        m_Breakpoint = _breakpoint;
        m_Endpoint = _endpoint;
      }

      float applyCurve(float _in)
      {
        float out;

        if(_in <= m_Split)
        {
          out = (_in * m_Factor1) + m_Startpoint;
        }
        else
        {
          out = ((_in - m_Split) * m_Factor2) + m_Breakpoint;
        }

        return out;
      }
    };

    /*****************************************************************************/
    /** @brief    Shaper for control signals. Two breakpoints.
 *            [0], [33.3], [66.6] and [100]
 *            (intended for SVF Resonance Post Processing)
******************************************************************************/

    struct Shaper_2_BP
    {
      float m_Factor1, m_Factor2, m_Factor3;
      float m_Startpoint, m_Breakpoint1, m_Breakpoint2, m_Endpoint;
      float m_Split1 = 1.f / 3.f;
      float m_Split2 = 2.f / 3.f;

      void setCurve(float _startpoint, float _breakpoint1, float _breakpoint2, float _endpoint)
      {
        m_Factor1 = (_breakpoint1 - _startpoint) / (m_Split1 - 0.f);
        m_Factor2 = (_breakpoint2 - _breakpoint1) / (m_Split2 - m_Split1);
        m_Factor3 = (_endpoint - _breakpoint2) / (1.f - m_Split2);

        m_Startpoint = _startpoint;
        m_Breakpoint1 = _breakpoint1;
        m_Breakpoint2 = _breakpoint2;
        m_Endpoint = _endpoint;
      }

      float applyCurve(float _in)
      {
        float out;

        if(_in <= m_Split1)
        {
          out = (_in * m_Factor1) + m_Startpoint;
        }
        else if(_in <= m_Split2)
        {
          out = ((_in - m_Split1) * m_Factor2) + m_Breakpoint1;
        }
        else
        {
          out = ((_in - m_Split2) * m_Factor3) + m_Breakpoint2;
        }

        return out;
      }
    };

    /*****************************************************************************/
    /** @brief      Squared Curvature Evaluation
 *              (usable for polynomial transitions of TCD renderers)
 *  @param      value (representing transition progress) [0 ... 1]
 *              (can also be bipolar [-1 ... 1])
 *  @param      curvature (affecting the transition slope) [-1 ... 1]
 *  @return     resulting value
 *              (depending on the curvature argument, the transformation
 *              shapes incoming values to be within the following ranges:
 *              (1 - (1 - value)^2),    (curvature = -1, decreasing slope)
 *              (value),                (curvature = 0, constant slope)
 *              (value^2),              (curvature = 1, increasing slope)
******************************************************************************/

    inline float SquaredCurvature(const float _value, const float _curvature)
    {
      return (_value * (1.f + (_curvature * (std::abs(_value) - 1.f))));
    }

  }  // Namespace Curves

}  // Namespace NlToolbox
