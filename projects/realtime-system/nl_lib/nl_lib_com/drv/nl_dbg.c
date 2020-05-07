/******************************************************************************/
/** @file		nl_dbg.c
    @date		2013-04-23
    @version	0.03
    @author		Daniel Tzschentke [2012-03-08]
    @brief		This module is a debug interface and provides access to LEDs, 
				buttons and the logic-analyzer IOs
    @ingroup	nl_drv_modules
*******************************************************************************/

#include "drv/nl_dbg.h"
#include "drv/nl_gpio.h"

static DBG_PINS_T* pins;

void (*const DBG_Pod_On[4])(void) = {
  DBG_GPIO3_1_On,
  DBG_GPIO3_2_On,
  DBG_GPIO3_3_On,
  DBG_GPIO3_4_On,
};

void (*const DBG_Pod_Off[4])(void) = {
  DBG_GPIO3_1_On,
  DBG_GPIO3_2_On,
  DBG_GPIO3_3_On,
  DBG_GPIO3_4_On,
};

/******************************************************************************/
/** @brief    	Function for the module configuration
	@param		UARTx	Pointer to selected UART peripheral, should be:
  					- LPC_USART0	:USART0 peripheral
  					- LPC_USART2	:USART2 peripheral *****
  					- LPC_USART3	:USART3 peripheral
	@param		speed	UART baudrate (115200)
	@param		pins	pin structure pointer
*******************************************************************************/
void DBG_Config(DBG_PINS_T* dbgpins)
{
  pins = dbgpins;
}

/******************************************************************************/
/** @brief    	Inits the GPIOS for LEDs, buttons and the logic analyzer port
*******************************************************************************/
void DBG_Init(void)
{
}

/******************************************************************************/
/** @brief    	Function to turn the board leds on or off
	@param		led gpio structure
	@param		state
				- ON
				- OFF
*******************************************************************************/
void DBG_Led(GPIO_NAME_T* led, uint8_t state)
{
  NL_GPIO_SetState(led, state);
}

/******************************************************************************/
/** @brief    	Functions to use with the COOS -> No parameters are possible.
*******************************************************************************/
void DBG_Led_Error_On(void)
{
  NL_GPIO_Set(pins->led_error);
}
void DBG_Led_Error_Off(void)
{
  NL_GPIO_Clr(pins->led_error);
}

void DBG_Led_Warning_On(void)
{
  NL_GPIO_Set(pins->led_warning);
}
void DBG_Led_Warning_Off(void)
{
  NL_GPIO_Clr(pins->led_warning);
}

void DBG_Led_Cpu_On(void)
{
  NL_GPIO_Set(pins->led_cpu);
}
void DBG_Led_Cpu_Off(void)
{
  NL_GPIO_Clr(pins->led_cpu);
}

void DBG_Led_Audio_On(void)
{
  NL_GPIO_Set(pins->led_audio);
}
void DBG_Led_Audio_Off(void)
{
  NL_GPIO_Clr(pins->led_audio);
}

void DBG_GPIO3_1_On(void)
{
  NL_GPIO_Set(pins->pod[0]);
}
void DBG_GPIO3_1_Off(void)
{
  NL_GPIO_Clr(pins->pod[0]);
}
void DBG_GPIO3_2_On(void)
{
  NL_GPIO_Set(pins->pod[1]);
}
void DBG_GPIO3_2_Off(void)
{
  NL_GPIO_Clr(pins->pod[1]);
}
void DBG_GPIO3_3_On(void)
{
  NL_GPIO_Set(pins->pod[2]);
}
void DBG_GPIO3_3_Off(void)
{
  NL_GPIO_Clr(pins->pod[2]);
}
void DBG_GPIO3_4_On(void)
{
  NL_GPIO_Set(pins->pod[3]);
}
void DBG_GPIO3_4_Off(void)
{
  NL_GPIO_Clr(pins->pod[3]);
}

static uint16_t errorTimer          = 0;
static uint16_t warningTimer        = 0;
static uint16_t errorTimerFlicker   = 0;
static uint16_t warningTimerFlicker = 0;

void DBG_Led_Error_TimedOn(int16_t time)
{
  if (time == 0)
    return;
  errorTimerFlicker = errorTimer != 0 && time > 0;
  if (time < 0)
    time = -time;
  if (time >= errorTimer)
    errorTimer = time;
  if (errorTimerFlicker)
    DBG_Led_Error_Off();
  else
    DBG_Led_Error_On();
}

void DBG_Led_Warning_TimedOn(int16_t time)
{
  if (time == 0)
    return;
  warningTimerFlicker = warningTimer != 0 && time > 0;
  if (time < 0)
    time = -time;
  if (time >= warningTimer)
    warningTimer = time;
  if (warningTimerFlicker)
    DBG_Led_Warning_Off();
  else
    DBG_Led_Warning_On();
}

/******************************************************************************/
/** @brief    	Handling the M4 LEDs (HeartBeat, Warning, Error), every 100ms
*******************************************************************************/
void DBG_Process(void)
{
  static int HB_Toggle = 0;
  static int HB_Timer  = 5;
  if (!--HB_Timer)
  {
    HB_Toggle = NL_GPIO_Tgl(pins->led_heartbeat, HB_Toggle);
    HB_Timer  = 5;
  }

  // Error LED
  if (errorTimerFlicker)
  {
    if (!--errorTimerFlicker)
    {
      DBG_Led_Error_On();
    }
  }
  else if (errorTimer)
  {
    if (!--errorTimer)
      DBG_Led_Error_Off();
  }

  // Warning LED
  if (warningTimerFlicker)
  {
    if (!--warningTimerFlicker)
    {
      DBG_Led_Warning_On();
    }
  }
  else if (warningTimer)
  {
    if (!--warningTimer)
      DBG_Led_Warning_Off();
  }
}

/** EMPHASE V5 */
void DBG_Process_M4(void)
{
  DBG_Process();
}

void DBG_Process_M0(void)
{
  static uint8_t toggleM0 = 0;
  if (toggleM0 == 0)
  {
    NL_GPIO_Set(pins->led_cpu);
    toggleM0 = 1;
  }
  else
  {
    NL_GPIO_Clr(pins->led_cpu);
    toggleM0 = 0;
  }
}

void DBG_Led_Task_Overflow_On(void)
{
  NL_GPIO_Set(pins->led_warning);
}

void DBG_Led_Task_Overflow_Off(void)
{
  NL_GPIO_Clr(pins->led_warning);
}
