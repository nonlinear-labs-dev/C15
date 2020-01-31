/*
 * nl_tcd_adc_work.c
 *
 *  Created on: 13.03.2015
 *      Author: ssc
 *  Last change on : 2020-01-03 KSTR
 */

#include "math.h"

#include "nl_tcd_adc_work.h"
#include "nl_tcd_msg.h"
#include "ipc/emphase_ipc.h"

#include "ipc/emphase_ipc.h"
#include "spibb/nl_bb_msg.h"

#include "nl_tcd_test.h"
#include "nl_tcd_interpol.h"
#include "ehc/nl_ehc.h"

#define GROUND_THRESHOLD     20
#define CHANGE_FOR_DETECTION 500

#define DELTA 40  // hysteresis for pedals

#define BENDER_DEADRANGE    20    // +/-1 % of +/-2047
#define BENDER_SMALL_THRESH 200   // +/-10 % of +/-2047, test range
#define BENDER_TEST_PERIOD  80    // 1 s = 80 * 12.5 ms
#define BENDER_RAMP_INC     8     // 400 / 8 = 50 steps from the threshold to zero (50 * 12.5 ms = 625 ms)
#define BENDER_FACTOR       4540  // 4540 / 4096 = 2047 / 1847 for saturation = 100 % at 90 % of the input range

#define AT_DEADRANGE 30    // 0.73 % of 0 ... 4095
#define AT_FACTOR    5080  // 5080 / 4096 for saturation = 100 % at 81 % of the input range

static uint32_t bbSendValue[NUM_HW_SOURCES] = {};

static int32_t  lastPitchbend;
static uint32_t pitchbendZero;

static uint32_t  pbSignalIsSmall;
static uint32_t  pbTestTime;
static uint32_t  pbTestMode;
static uint32_t  pbRampMode;
static int32_t   pbRamp;
static int32_t   pbRampInc;
static uint32_t  allBenderTables[2][33] = {};  // contains the bender curves
static uint32_t* benderTable;

static uint32_t  lastAftertouch;
static uint32_t  allAftertouchTables[2][33] = {};  // contains the chosen aftertouch curve
static uint32_t* aftertouchTable;

//========= ribbons ========

#if 0
// default calibration tables (derived from a variety of ribbons)
static int32_t RIBBON_1_DEFAULT_CALIBRATION_TABLE_X[34] = {
  160, 187, 299, 431, 559, 685, 806, 922, 1035, 1151, 1266, 1381, 1497, 1610, 1723, 1834, 1946, 2057,
  2167, 2275, 2384, 2495, 2606, 2723, 2840, 2958, 3081, 3208, 3338, 3472, 3613, 3758, 3904, 4033
};
static int32_t RIBBON_2_DEFAULT_CALIBRATION_TABLE_X[34] = {
  160, 187, 299, 431, 559, 685, 806, 922, 1035, 1151, 1266, 1381, 1497, 1610, 1723, 1834, 1946, 2057,
  2167, 2275, 2384, 2495, 2606, 2723, 2840, 2958, 3081, 3208, 3338, 3472, 3613, 3758, 3904, 4033
};

static int32_t RIBBON_DEFAULT_CALIBRATION_TABLE_Y[33] = {
  0, 712, 1198, 1684, 2170, 2655, 3141, 3627, 4113, 4599, 5085, 5571, 6057, 6542, 7028, 7514, 8000,
  8500, 8993, 9494, 9994, 10495, 10995, 11496, 11996, 12497, 12997, 13498, 13998, 14499, 14999, 15500, 16000
};
#else
// custom calibration tables (hardcoded, device topcover specific)
static int32_t RIBBON_1_DEFAULT_CALIBRATION_TABLE_X[34] = {
  122, 145, 254, 371, 490, 604, 720, 836, 959, 1078, 1198, 1316, 1441, 1552, 1672, 1790, 1902, 2020,
  2135, 2250, 2363, 2480, 2600, 2717, 2843, 2963, 3088, 3215, 3350, 3485, 3629, 3780, 3911, 4033
};
static int32_t RIBBON_2_DEFAULT_CALIBRATION_TABLE_X[34] = {
  117, 139, 243, 359, 473, 589, 705, 826, 949, 1068, 1184, 1308, 1429, 1542, 1661, 1775, 1895, 2009,
  2124, 2239, 2353, 2470, 2589, 2708, 2829, 2949, 3078, 3208, 3342, 3480, 3623, 3776, 3910, 4033
};

static int32_t RIBBON_DEFAULT_CALIBRATION_TABLE_Y[33] = {
  0, 712, 1198, 1684, 2170, 2655, 3141, 3627, 4113, 4599, 5085, 5571, 6057, 6542, 7028, 7514, 8000,
  8500, 8993, 9494, 9994, 10495, 10995, 11496, 11996, 12497, 12997, 13498, 13998, 14499, 14999, 15500, 16000
};
#endif

static LIB_interpol_data_T RIBBON_1_DEFAULT_CALIBRATION_DATA = {
  sizeof(RIBBON_DEFAULT_CALIBRATION_TABLE_Y) / sizeof(RIBBON_DEFAULT_CALIBRATION_TABLE_Y[0]),
  &RIBBON_1_DEFAULT_CALIBRATION_TABLE_X[1],
  RIBBON_DEFAULT_CALIBRATION_TABLE_Y
};
static LIB_interpol_data_T RIBBON_2_DEFAULT_CALIBRATION_DATA = {
  sizeof(RIBBON_DEFAULT_CALIBRATION_TABLE_Y) / sizeof(RIBBON_DEFAULT_CALIBRATION_TABLE_Y[0]),
  &RIBBON_2_DEFAULT_CALIBRATION_TABLE_X[1],
  RIBBON_DEFAULT_CALIBRATION_TABLE_Y
};

// user calibration tables
static int32_t ribbon_1_calibration_table_x[34];
static int32_t ribbon_1_calibration_table_y[33];
static int32_t ribbon_2_calibration_table_x[34];
static int32_t ribbon_2_calibration_table_y[33];

static LIB_interpol_data_T ribbon_1_calibration_data = {
  sizeof(ribbon_1_calibration_table_y) / sizeof(ribbon_1_calibration_table_y[0]),
  &ribbon_1_calibration_table_x[1],
  ribbon_1_calibration_table_y
};

static LIB_interpol_data_T ribbon_2_calibration_data = {
  sizeof(ribbon_2_calibration_table_y) / sizeof(ribbon_2_calibration_table_y[0]),
  &ribbon_2_calibration_table_x[1],
  ribbon_2_calibration_table_y
};

// type for working variables
typedef struct
{
  int32_t              last;           ///< last raw value (0...4095)
  int32_t              touch;          ///< flag for "touch begins"
  int32_t              behavior;       ///< bit field: Bit 0:return/#non-return, Bit 1:relative/#absolute
  int32_t              relFactor;      ///< scale factor for relative movement
  uint32_t             isEditControl;  ///< flag for usage as "Edit Control Slider"
  uint32_t             editBehavior;   ///< flag absolute/#relative
  uint32_t             incBase;        ///< base value for relative increment
  int32_t              output;         ///< processed output value (0...16000)
  int32_t              threshold;      ///< touch threshold
  LIB_interpol_data_T* calibration;    ///< pointer to calibration data to be used
  uint8_t              ipcId;          ///< ID to fetch raw data from M0 kernel
  uint32_t             hwSourceId;     ///< ID for BB and TCD message

} Ribbon_Data_T;

// working variables
static Ribbon_Data_T ribbon[2];  // two ribbons
#define RIB1 0
#define RIB2 1

// global control
static uint32_t suspend;
static int      send_raw_sensor_messages = 0;  // sends raw sensor values every 12.5ms when set (!= 0)

static int32_t SetThreshold(int32_t val)
{  // set threshold to 80%
  val *= 8;
  val /= 10;
  return val;
}

/*****************************************************************************
* @brief	ADC_WORK_Select_BenderTable
* @param	0: soft, 1: normal, 2: hard
******************************************************************************/
void ADC_WORK_Select_BenderTable(uint32_t curve)
{
  if (curve > 2)
    return;
  benderTable = allBenderTables[curve];
}

void Generate_BenderTable(uint32_t curve)
{
  float_t range = 8000.0;  // separate processing of absolute values for positive and negative range

  float_t s = 0.5;

  switch (curve)  // s defines the curve shape
  {
    case 0:     // soft
      s = 0.0;  // y = x
      break;
    case 1:     // normal
      s = 0.5;  // y = 0.5 * x + 0.5 * x^3
      break;
    case 2:     // hard
      s = 1.0;  // y = x^3
      break;
    default:
      /// Error
      break;
  }

  uint32_t i_max = 32;
  uint32_t i;
  float    x;

  for (i = 0; i <= i_max; i++)
  {
    x = (float) i / (float) i_max;

    allBenderTables[curve][i] = (uint32_t)(range * x * ((1.0 - s) + s * x * x));
  }
}

/*****************************************************************************
* @brief	ADC_WORK_Select_AftertouchTable -
* @param	0: soft, 1: normal, 2: hard
******************************************************************************/
void ADC_WORK_Select_AftertouchTable(uint32_t curve)
{
  if (curve > 2)
    return;
  aftertouchTable = allAftertouchTables[curve];
}

void Generate_AftertouchTable(uint32_t curve)
{
  float_t range = 16000.0;  // full TCD range

  float_t s = 0.7;

  switch (curve)  // s defines the curve shape
  {
    case 0:     // soft
      s = 0.0;  // y = x
      break;
    case 1:     // normal
      s = 0.7;  // y = 0.3 * x + 0.7 * x^6
      break;
    case 2:      // hard
      s = 0.95;  // y = 0.05 * x + 0.95 * x^6
      break;
    default:
      /// Error
      break;
  }

  uint32_t i_max = 32;
  uint32_t i;
  float    x;

  for (i = 0; i <= i_max; i++)
  {
    x = (float) i / (float) i_max;

    allAftertouchTables[curve][i] = (uint32_t)(range * x * ((1.0 - s) + s * x * x * x * x * x));
  }
}

/*****************************************************************************
* @brief	ADC_WORK_Init -
******************************************************************************/
void ADC_WORK_Init(void)
{
  uint32_t i;

  lastPitchbend = 0;
  pitchbendZero = 2048;

  pbSignalIsSmall = 0;
  pbTestTime      = 0;
  pbTestMode      = 0;
  pbRampMode      = 0;
  pbRamp          = 0;
  pbRampInc       = 0;

  Generate_BenderTable(0);
  Generate_BenderTable(1);
  Generate_BenderTable(2);
  ADC_WORK_Select_BenderTable(1);

  lastAftertouch = 0;
  Generate_AftertouchTable(0);
  Generate_AftertouchTable(1);
  Generate_AftertouchTable(2);
  ADC_WORK_Select_AftertouchTable(1);

  // initialize ribbon data
  for (i = 0; i <= 1; i++)
  {
    ribbon[i].last          = 0;
    ribbon[i].touch         = 0;
    ribbon[i].behavior      = 0;    // Abs, Non-Return
    ribbon[i].relFactor     = 256;  // == 1.0
    ribbon[i].isEditControl = 0;
    ribbon[i].editBehavior  = 0;
    ribbon[i].incBase       = 0;
    ribbon[i].output        = 0;
    ribbon[i].calibration   = (i == 0 ? &RIBBON_1_DEFAULT_CALIBRATION_DATA : &RIBBON_2_DEFAULT_CALIBRATION_DATA);  // use default calibration
    ribbon[i].threshold     = SetThreshold(ribbon[i].calibration->x_values[0]);
    ribbon[i].ipcId         = (i == 0 ? IPC_ADC_RIBBON1 : IPC_ADC_RIBBON1);
    ribbon[i].hwSourceId    = (i == 0 ? HW_SOURCE_ID_RIBBON_1 : HW_SOURCE_ID_RIBBON_2);
  }

  NL_EHC_Init();

  suspend = 0;
}

/*****************************************************************************
* @brief	SendEditMessageToBB - when using a ribbon as an edit control,
* 			the messages are sent by calls to this function
* @param	paramId: parameter Ids (254 ... 289), see: nl_tcd_param_work.h
* @param	value: ribbon position (0 ... 16000)
* @param	inc: position increment (theoretically -16000 ... 16000)
******************************************************************************/
static void SendEditMessageToBB(uint32_t paramId, uint32_t value, int32_t inc)
{
  if (ribbon[RIB1].editBehavior)  // 1: the ribbon sends the absolute value
  {
    BB_MSG_WriteMessage2Arg(BB_MSG_TYPE_EDIT_CONTROL, paramId, value);  // sends the value as an Edit Control message; results being displayed on the upper Ribbon
    BB_MSG_SendTheBuffer();                                             /// später mit somethingToSend = 1; !!!
  }
  else  // 0: the ribbon sends the increment
  {
    uint16_t inc16;

    if (inc < 0)
    {
      if (inc < -8000)  // limiting it to a 14-bit range like with other controls
      {
        inc = -8000;
      }

      inc16 = 0x8000 | (-inc);  // 0x8000 is the sign bit, -inc is the absolute

      BB_MSG_WriteMessage2Arg(BB_MSG_TYPE_EDIT_CONTROL, paramId, inc16);  // sends the increment as an Edit Control message; results being displayed on the upper Ribbon
      BB_MSG_SendTheBuffer();                                             /// später mit somethingToSend = 1; !!!
    }
    else if (inc > 0)
    {
      if (inc > 8000)  // limiting it to a 14-bit range like with other controls
      {
        inc = 8000;
      }

      inc16 = inc;

      BB_MSG_WriteMessage2Arg(BB_MSG_TYPE_EDIT_CONTROL, paramId, inc16);  // sends the increment as an Edit Control message; results being displayed on the upper Ribbon
      BB_MSG_SendTheBuffer();                                             /// später mit somethingToSend = 1; !!!
    }
  }
}

/// "return"-Behaviour braucht eigene Funktion, die mit einem Timer-Task aufgerufen wird
/// mit Abfrage, ob das Ribbon losgelassen wurde. Sprungartiges Zurücksetzen.

void ADC_WORK_WriteHWValueForBB(uint32_t hwSourceId, uint32_t value)
{
  bbSendValue[hwSourceId] = value | 0x80000;  // the bit is set to check for values to send, will be masked out
}

void ADC_WORK_SendBBMessages(void)  // is called as a regular COOS task
{
  uint32_t i;
  uint32_t send = 0;

  for (i = 0; i < NUM_HW_SOURCES; i++)
  {
    if (bbSendValue[i] > 0)
    {
      if (BB_MSG_WriteMessage2Arg(BB_MSG_TYPE_PARAMETER, i, (bbSendValue[i] & 0xFFFF)) > -1)
      {
        bbSendValue[i] = 0;
        send           = 1;
      }
    }
  }

  if (send == 1)
  {
    BB_MSG_SendTheBuffer();
  }
}

/*****************************************************************************
* @brief	ADC_WORK_SetRibbon1Behaviour
* @param	0: Abs + Non-Return, 1: Abs + Return, 2: Rel + Non-Return, 3: Rel + Return
******************************************************************************/
void ADC_WORK_SetRibbon1Behaviour(uint32_t behaviour)
{
  ribbon[RIB1].behavior = behaviour;
  if (ribbon[RIB1].isEditControl == 0)  // initialization if working as a HW source
  {
    if ((ribbon[RIB1].behavior == 1) || (ribbon[RIB1].behavior == 3))  // "return" behaviour
    {
      ribbon[RIB1].output = 8000;
      ADC_WORK_WriteHWValueForBB(HW_SOURCE_ID_RIBBON_1, 8000);
    }
  }
}

uint32_t ADC_WORK_GetRibbon1Behaviour(void)
{
  if (ribbon[RIB1].behavior & 0x01)
    return RETURN_TO_CENTER;
  else
    return NON_RETURN;
}

/*****************************************************************************
* @brief	ADC_WORK_SetRibbonRelFactor -
* @param
******************************************************************************/
void ADC_WORK_SetRibbonRelFactor(uint32_t factor)
{
  ribbon[RIB1].relFactor = factor;
  ribbon[RIB2].relFactor = factor;
}

/*****************************************************************************
* @brief	ADC_WORK_SetRibbon2Behaviour -
* @param	0: Abs + Non-Return, 1: Abs + Return, 2: Rel + Non-Return, 3: Rel + Return
******************************************************************************/
void ADC_WORK_SetRibbon2Behaviour(uint32_t behaviour)
{
  ribbon[RIB2].behavior = behaviour;
  if (ribbon[RIB2].isEditControl == 0)  // initialization if working as a HW source
  {
    if ((ribbon[RIB2].behavior == 1) || (ribbon[RIB2].behavior == 3))  // "return" behaviour
    {
      ribbon[RIB2].output = 8000;
      ADC_WORK_WriteHWValueForBB(HW_SOURCE_ID_RIBBON_2, 8000);
    }
  }
}

uint32_t ADC_WORK_GetRibbon2Behaviour(void)
{
  if (ribbon[RIB2].behavior & 0x01)
    return RETURN_TO_CENTER;
  else
    return NON_RETURN;
}

/*****************************************************************************
* @brief	ADC_WORK_SetRibbon1EditMode -
* @param	0: Play, 1: (Parameter) Edit
******************************************************************************/
void ADC_WORK_SetRibbon1EditMode(uint32_t mode)
{
  ribbon[RIB1].isEditControl = mode;
}

/*****************************************************************************
* @brief	ADC_WORK_SetRibbon1EditBehaviour -
* @param	0: Relative + Non-Return, 1: Absolute + Non-Return
******************************************************************************/
void ADC_WORK_SetRibbon1EditBehaviour(uint32_t behaviour)
{
  ribbon[RIB1].editBehavior = behaviour;
}

/*****************************************************************************
* @brief	ADC_WORK_SetPedal1Behaviour -
* @param	0: Non-Return, 1: Return to Zero, 2: Return to Center
******************************************************************************/
void ADC_WORK_SetPedal1Behaviour(uint32_t behaviour)
{
}

uint32_t ADC_WORK_GetPedal1Behaviour(void)
{
  return 0;
}

/*****************************************************************************
* @brief	ADC_WORK_SetPedal2Behaviour -
* @param	0: Non-Return, 1: Return to Zero, 2: Return to Center
******************************************************************************/
void ADC_WORK_SetPedal2Behaviour(uint32_t behaviour)
{
}

uint32_t ADC_WORK_GetPedal2Behaviour(void)
{
  return 0;
}

/*****************************************************************************
* @brief	ADC_WORK_SetPedal3Behaviour -
* @param	0: Non-Return, 1: Return to Zero, 2: Return to Center
******************************************************************************/
void ADC_WORK_SetPedal3Behaviour(uint32_t behaviour)
{
}

uint32_t ADC_WORK_GetPedal3Behaviour(void)
{
  return 0;
}

/*****************************************************************************
* @brief	ADC_WORK_SetPedal4Behaviour -
* @param	0: Non-Return, 1: Return to Zero, 2: Return to Center
******************************************************************************/
void ADC_WORK_SetPedal4Behaviour(uint32_t behaviour)
{
}

uint32_t ADC_WORK_GetPedal4Behaviour(void)
{
  return 0;
}

/*****************************************************************************
* @brief	ADC_WORK_SetRibbonCalibration -
* @param	length: # of words in data array
* @param	data: array containing calibration data sets for the ribbons
******************************************************************************/
void ADC_WORK_SetRibbonCalibration(uint16_t length, uint16_t* data)
{
  if (length != (34 + 33 + 34 + 33))  // data must contain X (34 point) and Y (33 points) sets,  for each ribbon
    return;

  int i;
  for (i = 0; i < 34; i++)
    ribbon_1_calibration_table_x[i] = (int32_t) *data++;
  for (i = 0; i < 33; i++)
    ribbon_1_calibration_table_y[i] = (int32_t) *data++;
  for (i = 0; i < 34; i++)
    ribbon_2_calibration_table_x[i] = (int32_t) *data++;
  for (i = 0; i < 33; i++)
    ribbon_2_calibration_table_y[i] = (int32_t) *data++;

  // Integrity checks
  for (i = 0; i < 34; i++)
  {
    if (ribbon_1_calibration_table_x[i] < 100  // lowest raw value on touch is sure > 100 (140 typ.)
        || ribbon_1_calibration_table_x[i] > 4095
        || ribbon_2_calibration_table_x[i] < 100  // lowest raw value on touch is sure > 100 (140 typ.)
        || ribbon_2_calibration_table_x[i] > 4095)
      return;  // x-values are not in valid range for raw values
  }
  for (i = 0; i < 33; i++)
  {
    if (ribbon_1_calibration_table_y[i] < 0
        || ribbon_1_calibration_table_y[i] > 16000
        || ribbon_2_calibration_table_y[i] < 0
        || ribbon_2_calibration_table_y[i] > 16000)
      return;  // y-values are not in valid range for output values
  }
  for (i = 1; i < 33; i++)
  {
    if (ribbon_1_calibration_table_x[i] <= ribbon_1_calibration_table_x[i - 1]
        || ribbon_2_calibration_table_x[i] <= ribbon_2_calibration_table_x[i - 1])
      return;  // x-values are not monotonically rising
  }

  ribbon[RIB1].calibration = &ribbon_1_calibration_data;
  ribbon[RIB2].calibration = &ribbon_2_calibration_data;
  ribbon[RIB1].threshold   = SetThreshold(ribbon[RIB1].calibration->x_values[0]);
  ribbon[RIB2].threshold   = SetThreshold(ribbon[RIB2].calibration->x_values[0]);
}

/*****************************************************************************
* @brief  Process Ribbons, array'd style, new Linearization/Calibration
******************************************************************************/
static void ProcessRibbons(void)
{
  int32_t value;
  int32_t valueToSend;

  for (int i = 0; i <= 1; i++)
  {
    uint32_t send        = 0;
    uint32_t touchBegins = 0;
    int32_t  inc         = 0;

    value = IPC_ReadAdcBufferAveraged(ribbon[i].ipcId);

    if (value > ribbon[i].last + 1)  // rising values (min. +2)
    {
      if (value > ribbon[i].threshold)  // above the touch threshold
      {
        if (ribbon[i].touch == 0)
        {
          ribbon[i].touch = 1;
          touchBegins     = 1;
        }
        valueToSend = LIB_InterpolateValue(ribbon[i].calibration, value);
        send        = 1;
      }
      ribbon[i].last = value;
    }
    else if (value + 1 < ribbon[i].last)  // falling values (min. -2)
    {
      if (value > ribbon[i].threshold)  // above the touch threshold; ribbon1Touch is already on, because the last value was higher
      {
        // outputs the previous value (one sample delay) to avoid sending samples of the falling edge of a touch release
        valueToSend = LIB_InterpolateValue(ribbon[i].calibration, ribbon[i].last);
        send        = 1;
      }
      else  // below the touch threshold
      {
        ribbon[i].touch = 0;

        if (ribbon[i].isEditControl == 0)  // working as a play control
        {
          if ((ribbon[i].behavior == 1) || (ribbon[i].behavior == 3))  // "return" behaviour
          {
            MSG_HWSourceUpdate(ribbon[i].hwSourceId, 8000);
            ADC_WORK_WriteHWValueForBB(ribbon[i].hwSourceId, 8000);

            ribbon[i].output = 8000;
          }
        }
      }
      ribbon[i].last = value;
    }

    if (send)
    {
      if (touchBegins)  // in the incremental mode the jump to the touch position has to be ignored
      {
        inc         = 0;
        touchBegins = 0;
      }
      else
        inc = valueToSend - ribbon[i].incBase;

      ribbon[i].incBase = valueToSend;

      if (ribbon[i].isEditControl)
      {
        inc = (inc * ribbon[i].relFactor) / 256;
        SendEditMessageToBB(ribbon[i].hwSourceId, valueToSend, inc);
      }
      else
      {
        if (ribbon[i].behavior < 2)  // absolute
        {
          ribbon[i].output = valueToSend;

          MSG_HWSourceUpdate(ribbon[i].hwSourceId, ribbon[i].output);
          ADC_WORK_WriteHWValueForBB(ribbon[i].hwSourceId, ribbon[i].output);
        }
        else  // relative
        {
          if (inc != 0)
          {
            inc = (inc * ribbon[i].relFactor) / 256;
            ribbon[i].output += inc;

            if (ribbon[i].output < 0)
              ribbon[i].output = 0;
            if (ribbon[i].output > 16000)
              ribbon[i].output = 16000;

            MSG_HWSourceUpdate(ribbon[i].hwSourceId, ribbon[i].output);
            ADC_WORK_WriteHWValueForBB(ribbon[i].hwSourceId, ribbon[i].output);
          }
        }
      }
    }
  }  // for all ribbons
}

void ADC_WORK_SetRawSensorMessages(uint32_t flag)
{
  send_raw_sensor_messages = (flag != 0);
}

void ADC_WORK_Suspend(void)
{
  suspend = 1;
}

void ADC_WORK_Resume(void)
{
  suspend = 0;
}

/*****************************************************************************
* @brief  ADC_WORK_Process -
******************************************************************************/
void ADC_WORK_Process(void)
{
  if (suspend)
  {
    return;
  }

#if 0
  if (1 || send_raw_sensor_messages)
  {
    uint16_t data[13];

    data[0] = (Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_PEDAL_1_DETECT) << 0)
        | (Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_PEDAL_2_DETECT) << 1)
        | (Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_PEDAL_3_DETECT) << 2)
        | (Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_PEDAL_4_DETECT) << 3);
    data[1]  = Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_PEDAL_1_ADC_TIP);
    data[2]  = Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_PEDAL_1_ADC_RING);
    data[3]  = Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_PEDAL_2_ADC_TIP);
    data[4]  = Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_PEDAL_2_ADC_RING);
    data[5]  = Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_PEDAL_3_ADC_TIP);
    data[6]  = Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_PEDAL_3_ADC_RING);
    data[7]  = Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_PEDAL_4_ADC_TIP);
    data[8]  = Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_PEDAL_4_ADC_RING);
    data[9]  = Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_PITCHBENDER_ADC);
    data[10] = Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_AFTERTOUCH_ADC);
    data[11] = Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_RIBBON_1_ADC);
    data[12] = Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_RIBBON_2_ADC);
    BB_MSG_WriteMessage(BB_MSG_TYPE_SENSORS_RAW, 13, data);
    BB_MSG_SendTheBuffer();
  }
#endif

#if 0
  if (1 || send_raw_sensor_messages)
  {
    static uint16_t data[13];
    static int state=0; // 0:both PU's, 1:ring PU, 2:tip PU
    static int wait = 0;

    data[0] = (Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_PEDAL_1_DETECT) << 0)
        | (Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_PEDAL_2_DETECT) << 1)
        | (Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_PEDAL_3_DETECT) << 2)
        | (Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_PEDAL_4_DETECT) << 3);
    data[1]  = Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_PEDAL_1_ADC_TIP);
    data[2]  = Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_PEDAL_1_ADC_RING);
    data[9]  = Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_PITCHBENDER_ADC);
    data[10] = Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_AFTERTOUCH_ADC);
    data[11] = Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_RIBBON_1_ADC);
    data[12] = Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_RIBBON_2_ADC);


    if (wait == 0)
    {
      switch (state)
      {
      case 0 :
        Emphase_IPC_PlayBuffer_Write(EMPHASE_IPC_PEDAL_1_STATE, PEDAL_RING_TO_PULLUP);
        state = 1;
        wait=03;
        // data[3] = data[1]; // tip with PU
        // data[4] = data[2]; // ring with PU
        break;
      case 1 :
        Emphase_IPC_PlayBuffer_Write(EMPHASE_IPC_PEDAL_1_STATE, PEDAL_TIP_TO_PULLUP);
        state = 2;
        wait=07;
        data[5] = data[1];  // tip OPEN
        data[6] = data[2];  // ring with PU
        break;
      case 2 :
        Emphase_IPC_PlayBuffer_Write(EMPHASE_IPC_PEDAL_1_STATE, PEDAL_RING_TO_PULLUP);
        state = 1;
        wait=07;
        data[7] = data[1];  // tip with PU
        data[8] = data[2];  // ring OPEN
        break;
      }
      BB_MSG_WriteMessage(BB_MSG_TYPE_SENSORS_RAW, 13, data);
      BB_MSG_SendTheBuffer();
    }
    else
    {
      --wait;
    }
  }
#endif

  int32_t value;
  int32_t valueToSend;

  //==================== Pitchbender

  value = IPC_ReadAdcBufferAveraged(IPC_ADC_PITCHBENDER);  // 0 ... 4095

  value = value - pitchbendZero;  // -2048 ... 2047 (after initialization)

  if ((value > -BENDER_SMALL_THRESH) && (value < BENDER_SMALL_THRESH))  // small signals
  {
    if (!pbSignalIsSmall)  // entering the small-signal range
    {
      pbTestTime = 0;
      pbTestMode = 1;  // start testing
    }

    pbSignalIsSmall = 1;
  }
  else  // large signals
  {
    if (pbSignalIsSmall)  // leaving the small-signal range
    {
      pbTestMode = 0;  // discard testing
    }

    pbSignalIsSmall = 0;
  }

  if (pbTestMode)
  {
    if (pbTestTime < BENDER_TEST_PERIOD)
    {
      pbTestTime++;
    }
    else  // test time finished
    {
      pbTestMode = 0;
      pbRampMode = 1;  // start the ramp to zero

      if (value > 0)
      {
        pbRamp    = value;            // absolute amount of shifting
        pbRampInc = BENDER_RAMP_INC;  // positive increment, will shift the zero line up
      }
      else
      {
        pbRamp    = -value;            // absolute amount of shifting
        pbRampInc = -BENDER_RAMP_INC;  // negative increment, will shift the zero line down
      }
    }
  }

  if (pbRampMode)
  {
    pbRamp -= BENDER_RAMP_INC;  // determines the size of the ramp

    if (pbRamp <= 0)  // ramp has reached zero
    {
      pbRampMode = 0;  // and is stopped
    }
    else  // the steps before the ramp reaches zero
    {
      pitchbendZero += pbRampInc;  // shifting the zero line = negative feedback
    }
  }

  if (value != lastPitchbend)
  {
    if (value > BENDER_DEADRANGE)  // is in the positive work range
    {
      valueToSend = value - BENDER_DEADRANGE;  // absolute amount

      valueToSend = valueToSend * BENDER_FACTOR;  // saturation factor

      if (valueToSend > 2047 * 4096)
      {
        valueToSend = 8000;  // clipping
      }
      else
      {
        valueToSend = valueToSend >> 12;  // 0 ... 2047 (11 Bit)

        uint32_t fract = valueToSend & 0x3F;                                                         // lower 6 bits used for interpolation
        uint32_t index = valueToSend >> 6;                                                           // upper 5 bits (0...31) used as index in the table
        valueToSend    = (benderTable[index] * (64 - fract) + benderTable[index + 1] * fract) >> 6;  // (0...8000) * 64 / 64
      }

      valueToSend = 8000 + valueToSend;  // 8001 ... 16000

      MSG_HWSourceUpdate(HW_SOURCE_ID_PITCHBEND, valueToSend);
      ADC_WORK_WriteHWValueForBB(HW_SOURCE_ID_PITCHBEND, valueToSend);
      TEST_Output(4, valueToSend);
    }
    else if (value < -BENDER_DEADRANGE)  // is in the negative work range
    {
      valueToSend = -BENDER_DEADRANGE - value;  // absolute amount

      valueToSend = valueToSend * BENDER_FACTOR;  // saturation factor

      if (valueToSend > 2047 * 4096)
      {
        valueToSend = 8000;  // clipping
      }
      else
      {
        valueToSend = valueToSend >> 12;  // 0 ... 2047 (11 Bit)

        uint32_t fract = valueToSend & 0x3F;                                                         // lower 6 bits used for interpolation
        uint32_t index = valueToSend >> 6;                                                           // upper 5 bits (0...31) used as index in the table
        valueToSend    = (benderTable[index] * (64 - fract) + benderTable[index + 1] * fract) >> 6;  // (0...8000) * 64 / 64
      }

      valueToSend = 8000 - valueToSend;  // 7999 ... 0

      MSG_HWSourceUpdate(HW_SOURCE_ID_PITCHBEND, valueToSend);
      ADC_WORK_WriteHWValueForBB(HW_SOURCE_ID_PITCHBEND, valueToSend);
      TEST_Output(4, valueToSend);
    }
    else  // is in the dead range
    {
      if ((lastPitchbend > BENDER_DEADRANGE) || (lastPitchbend < -BENDER_DEADRANGE))  // was outside of the dead range before
      {
        MSG_HWSourceUpdate(HW_SOURCE_ID_PITCHBEND, 8000);
        ADC_WORK_WriteHWValueForBB(HW_SOURCE_ID_PITCHBEND, 8000);
        TEST_Output(4, 8000);
      }
    }

    lastPitchbend = value;
  }

  //==================== Aftertouch

  value = IPC_ReadAdcBufferAveraged(IPC_ADC_AFTERTOUCH);

  if (value != lastAftertouch)
  {
    if (value > AT_DEADRANGE)  // outside of the dead range
    {
      valueToSend = value - AT_DEADRANGE;

      valueToSend = value * AT_FACTOR;  // saturation factor

      if (valueToSend > 4095 * 4096)
      {
        valueToSend = 16000;
      }
      else
      {
        valueToSend = valueToSend >> 12;  // 0 ... 4095

        uint32_t fract = valueToSend & 0x7F;                                                                  // lower 7 bits used for interpolation
        uint32_t index = valueToSend >> 7;                                                                    // upper 5 bits (0...31) used as index in the table
        valueToSend    = (aftertouchTable[index] * (128 - fract) + aftertouchTable[index + 1] * fract) >> 7;  // (0...16000) * 128 / 128
      }

      MSG_HWSourceUpdate(HW_SOURCE_ID_AFTERTOUCH, valueToSend);
      ADC_WORK_WriteHWValueForBB(HW_SOURCE_ID_AFTERTOUCH, valueToSend);
      TEST_Output(5, valueToSend);
    }
    else  // inside of the dead range
    {
      if (lastAftertouch > AT_DEADRANGE)  // was outside of the dead range before   /// define
      {
        MSG_HWSourceUpdate(HW_SOURCE_ID_AFTERTOUCH, 0);
        ADC_WORK_WriteHWValueForBB(HW_SOURCE_ID_AFTERTOUCH, 0);
        TEST_Output(5, 0);
      }
    }

    lastAftertouch = value;
  }

  //==================== Ribbons
  ProcessRibbons();

  //==================== External Hardware Controllers
  NL_EHC_Process();
}
