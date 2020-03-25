#pragma once

/******************************************************************************/
/** @file       parameter_declarations.h
    @date
    @version    1.7-0
    @author     M. Seeber
    @brief      describing all necessary parameter-related details
                (see sheet ParameterList_1.7B)
    @todo
*******************************************************************************/

namespace C15
{

  namespace Descriptors
  {

    enum class ParameterType
    {
      None,
      Hardware_Source,
      Global_Parameter,
      Hardware_Amount,
      Macro_Control,
      Macro_Time,
      Modulateable_Parameter,
      Unmodulateable_Parameter
    };

    enum class SmootherSection
    {
      None,
      Global,
      Poly,
      Mono
    };

    enum class SmootherClock
    {
      Sync,
      Audio,
      Fast,
      Slow
    };

    enum class ParameterSignal
    {
      None,
      Global_Signal,
      Quasipoly_Signal,
      Truepoly_Signal,
      Mono_Signal
    };

  }  // namespace C15::Descriptors

  namespace Properties
  {

    enum class Scale
    {
      None,
      Linear,
      Parabolic,
      Cubic,
      S_Curve,
      Expon_Gain,
      Expon_Osc_Pitch,
      Expon_Lin_Pitch,
      Expon_Shaper_Drive,
      Expon_Mix_Drive,
      Expon_Env_Time,
      _LENGTH_
    };

  }  // namespace C15::Properties

  namespace Parameters
  {

    enum class Hardware_Sources
    {
      _LENGTH_
    };

    enum class Global_Parameters
    {
      _LENGTH_
    };

    enum class Hardware_Amounts
    {
      _LENGTH_
    };

    enum class Macro_Controls
    {
      None,
      _LENGTH_
    };

    enum class Macro_Times
    {
      None,
      _LENGTH_
    };

    enum class Modulateable_Parameters
    {
      _LENGTH_
    };

    enum class Unmodulateable_Parameters
    {
      _LENGTH_
    };

  }  // namespace C15::Parameters

  namespace Smoothers
  {

    enum class Global_Sync
    {
      _LENGTH_
    };

    enum class Global_Audio
    {
      _LENGTH_
    };

    enum class Global_Fast
    {
      _LENGTH_
    };

    enum class Global_Slow
    {
      _LENGTH_
    };

    enum class Poly_Sync
    {
      _LENGTH_
    };

    enum class Poly_Audio
    {
      _LENGTH_
    };

    enum class Poly_Fast
    {
      _LENGTH_
    };

    enum class Poly_Slow
    {
      _LENGTH_
    };

    enum class Mono_Sync
    {
      _LENGTH_
    };

    enum class Mono_Audio
    {
      _LENGTH_
    };

    enum class Mono_Fast
    {
      _LENGTH_
    };

    enum class Mono_Slow
    {
      _LENGTH_
    };

  }  // namespace C15::Smoothers

  namespace Signals
  {

    enum class Global_Signals
    {
      _LENGTH_
    };

    enum class Quasipoly_Signals
    {
      _LENGTH_
    };

    enum class Truepoly_Signals
    {
      _LENGTH_
    };

    enum class Mono_Signals
    {
      _LENGTH_
    };

  }  // namespace C15::Signals

}  // namespace C15
