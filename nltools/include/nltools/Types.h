#pragma once

enum class MacroControls
{
  NONE = 0,
  MC1 = 1,
  MC2 = 2,
  MC3 = 3,
  MC4 = 4,
  NUM_CHOICES = 5
};

enum class RibbonTouchBehaviour
{
  ABSOLUTE,
  RELATIVE,
  NUM_TOUCH_BEHAVIOURS
};

enum class RibbonReturnMode
{
  STAY = 0,
  RETURN = 1,
  NUM_RETURN_MODES
};

enum class PedalModes
{
  STAY = 0,
  RETURN_TO_ZERO = 1,
  RETURN_TO_CENTER = 2,
  NUM_PEDAL_MODES
};

enum class ReturnMode
{
  None,
  Center,
  Zero
};
