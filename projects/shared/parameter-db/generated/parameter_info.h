#pragma once

/******************************************************************************/
/** @file       parameter_info.h
    @date       2020-12-05, 14:53
    @version    1.7B-8
    @author     M. Seeber
    @brief      parameter-related details outside of the parameter definition
    @todo
*******************************************************************************/

#include "parameter_declarations.h"

namespace C15
{

  namespace Properties
  {

    enum class LayerId
    {
      I,
      II,
      _LENGTH_
    };

    enum class LayerMode
    {
      Single,
      Split,
      Layer
    };

    // naming every HW Source return behavior
    enum class HW_Return_Behavior
    {
      Stay,
      Zero,
      Center
    };

  }  // namespace C15::Properties

}  // namespace C15
