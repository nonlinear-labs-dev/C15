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
#include "drv/nl_uart_dma.h"
#include <stdlib.h>
#include <string.h>

static uint8_t* to_send = NULL;

static LPC_USARTn_Type* dbg_uart;
static uint32_t         dbg_uart_baudrate;

static DBG_PINS_T* pins;

#if 0
void (* const DBG_Pod_On[8])(void) = {
		DBG_Pod_0_On,
		DBG_Pod_1_On,
		DBG_Pod_2_On,
		DBG_Pod_3_On,
		DBG_Pod_4_On,
		DBG_Pod_5_On,
		DBG_Pod_6_On,
		DBG_Pod_7_On,
};

void (* const DBG_Pod_Off[8])(void) = {
		DBG_Pod_0_Off,
		DBG_Pod_1_Off,
		DBG_Pod_2_Off,
		DBG_Pod_3_Off,
		DBG_Pod_4_Off,
		DBG_Pod_5_Off,
		DBG_Pod_6_Off,
		DBG_Pod_7_Off,
};
#endif

static void DBG_Callback(uint32_t status)
{
  free(to_send);
  to_send = NULL;
}

/******************************************************************************/
/** @brief    	Function for the module configuration
	@param		UARTx	Pointer to selected UART peripheral, should be:
  					- LPC_USART0	:USART0 peripheral
  					- LPC_USART2	:USART2 peripheral *****
  					- LPC_USART3	:USART3 peripheral
	@param		speed	UART baudrate (115200)
	@param		pins	pin structure pointer
*******************************************************************************/
void DBG_Config(LPC_USARTn_Type* UARTx, uint32_t speed, DBG_PINS_T* dbgpins)
{
  dbg_uart          = UARTx;
  dbg_uart_baudrate = speed;
  pins              = dbgpins;
}

/******************************************************************************/
/** @brief    	Inits the GPIOS for LEDs, buttons and the logic analyzer port
*******************************************************************************/
void DBG_Init(void)
{
  UART_DMA_Init(dbg_uart, dbg_uart_baudrate);
}

/******************************************************************************/
/** @brief    	Write string to the UART output
 *  @param		str		String to write
 *  @return		Number of bytes written if success; 0 if failure
*******************************************************************************/
uint32_t DBG_Write(char* str)
{
  uint32_t i;
  uint32_t len = strlen(str);

  if (to_send != NULL)
    return 0;

  to_send = (uint8_t*) malloc(len * sizeof(uint8_t));
  for (i = 0; i < len; i++)
    to_send[i] = (uint8_t) str[i];

  return UART_DMA_Send(dbg_uart, to_send, len, DBG_Callback);
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

#if 0
void DBG_Pod_0_On(void)
{
	NL_GPIO_Set(pins->pod[0]);
}
void DBG_Pod_0_Off(void)
{
	NL_GPIO_Clr(pins->pod[0]);
}
void DBG_Pod_1_On(void)
{
	NL_GPIO_Set(pins->pod[1]);
}
void DBG_Pod_1_Off(void)
{
	NL_GPIO_Clr(pins->pod[1]);
}
void DBG_Pod_2_On(void)
{
	NL_GPIO_Set(pins->pod[2]);
}
void DBG_Pod_2_Off(void)
{
	NL_GPIO_Clr(pins->pod[2]);
}
void DBG_Pod_3_On(void)
{
	NL_GPIO_Set(pins->pod[3]);
}
void DBG_Pod_3_Off(void)
{
	NL_GPIO_Clr(pins->pod[3]);
}
void DBG_Pod_4_On(void)
{
	NL_GPIO_Set(pins->pod[4]);
}
void DBG_Pod_4_Off(void)
{
	NL_GPIO_Clr(pins->pod[4]);
}
void DBG_Pod_5_On(void)
{
	NL_GPIO_Set(pins->pod[5]);
}
void DBG_Pod_5_Off(void)
{
	NL_GPIO_Clr(pins->pod[5]);
}
void DBG_Pod_6_On(void)
{
	NL_GPIO_Set(pins->pod[6]);
}
void DBG_Pod_6_Off(void)
{
	NL_GPIO_Clr(pins->pod[6]);
}
void DBG_Pod_7_On(void)
{
	NL_GPIO_Set(pins->pod[7]);
}
void DBG_Pod_7_Off(void)
{
	NL_GPIO_Clr(pins->pod[7]);
}
#endif

/******************************************************************************/
/** @brief    	This function takes care about the heartbeat led. It toggles
				the led every call. So it has to be called regulary to get the
				blinking LED.
*******************************************************************************/
void DBG_Process(void)
{
  static uint8_t toggle = 0;
  if (toggle == 0)
  {
    NL_GPIO_Set(pins->led_heartbeat);
    toggle = 1;
  }
  else
  {
    NL_GPIO_Clr(pins->led_heartbeat);
    toggle = 0;
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
