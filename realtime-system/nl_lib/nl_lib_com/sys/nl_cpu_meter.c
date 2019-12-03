/******************************************************************************/
/** @file		nl_cpu_meter.c
    @date		2015-11-08
    @brief    	CPU usage metering for LPC43xx
    @author		Nemanja Nikodijevic 2015-11-08
*******************************************************************************/

#include "sys/nl_cpu_meter.h"
#include "drv/nl_cgu.h"
#include "drv/nl_pin.h"
#include "drv/nl_gpio.h"

typedef enum meterState
{
  CPU_METER_IDLE = 0,
  CPU_METER_TASK = 1
} CPU_Meter_State;

static CPU_Meter_State  cpuMeterState = CPU_METER_IDLE;
static LPC_TIMERn_Type* CPU_Timer;
static uint32_t         systick_cnt = 0;
static uint8_t          cpu_usage   = 0;
static uint32_t         timestamp   = 0;
static uint32_t         ticks_idle = 0, ticks_task = 0;
static GPIO_NAME_T      thled_gpio;
static uint8_t          thold_led = 0;

static uint32_t CPU_Meter_Diff(void)
{
  uint32_t diff = 0;
  uint32_t cur  = CPU_Timer->TC;

  if (timestamp > cur)
  {
    diff = (0xFFFFFFFF - timestamp) + cur + 1;
  }
  else
  {
    diff = cur - timestamp;
  }

  timestamp = cur;
  return diff;
}

static void CPU_Meter_Apply_Diff(void)
{
  uint32_t diff = CPU_Meter_Diff();

  switch (cpuMeterState)
  {
    case CPU_METER_IDLE:
      ticks_idle += diff;
      break;
    case CPU_METER_TASK:
      ticks_task += diff;
      break;
    default:
      return;
  }
}

void CPU_Meter_Init(LPC_TIMERn_Type* TIMx, uint32_t tick_us)
{
  CPU_Timer   = TIMx;
  systick_cnt = tick_us * (NL_LPC_CLK / 1000000);

  CPU_Timer->CCR &= ~(0x03);
  CPU_Timer->CCR |= 1; /* timer mode - capture on rising edge */

  CPU_Timer->TC = 0;
  CPU_Timer->PC = 0;
  CPU_Timer->PR = 0;

  /* reset TC */
  CPU_Timer->TCR = 0;
  CPU_Timer->TCR |= (1 << 1);
  CPU_Timer->TCR &= ~(1 << 1);
  CPU_Timer->TC  = 0;
  CPU_Timer->TCR = 1;
}

void CPU_Meter_Update_Usage(void)
{
  CPU_Meter_Apply_Diff();
  cpu_usage  = (uint8_t)((ticks_task * 100) / (ticks_task + ticks_idle));
  ticks_task = 0;
  ticks_idle = 0;
  if (thold_led != 0)
  {
    if (thold_led < cpu_usage)
      NL_GPIO_Clr(&thled_gpio);
    else
      NL_GPIO_Set(&thled_gpio);
  }
}

void CPU_Meter_Task(void)
{
  CPU_Meter_Apply_Diff();
  cpuMeterState = CPU_METER_TASK;
}

void CPU_Meter_Idle(void)
{
  CPU_Meter_Apply_Diff();
  cpuMeterState = CPU_METER_IDLE;
}

uint8_t CPU_Meter_Get_Percentage(void)
{
  return cpu_usage;
}

void CPU_Meter_Set_Threshold_Led(uint8_t port, uint8_t pin, uint8_t th)
{
  thled_gpio.pin  = pin;
  thled_gpio.port = port;
  thold_led       = th;
  NL_GPIO_Set(&thled_gpio);
}
