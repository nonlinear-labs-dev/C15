#include <stdint.h>
#include "cmsis/lpc43xx_cgu.h"

static void Delay300(void);

/******************************************************************************/
/** @brief    	configures the clocks of the whole system
	@details	The main cpu clock should set to 204 MHz. According to the
				datasheet, therefore the clock has to be increased in two steps.
				1. low frequency range to mid frequency range
				2. mid frequency range to high frequency range
				clock ranges:
				    - low		f_cpu: < 90 MHz
				    - mid		f_cpu: 90 MHz to 110 MHz
				  	- high		f_cpu: up to 204 MHz
				settings (f: frequency, v: value):
					- f_osc  =  12 MHz
					- f_pll1 = 204 MHz		v_pll1 = 17
					- f_cpu  = 204 MHz
*******************************************************************************/
void CPU_ConfigureClocks(void)
{
  /* XTAL OSC */
  CGU_SetXTALOSC(12000000);                                 // set f_osc = 12 MHz (external XTAL OSC is a 12 MHz device)
  Delay300();                                               // delay at least 300 µs
  CGU_EnableEntity(CGU_CLKSRC_XTAL_OSC, ENABLE);            // Enable xtal osc clock entity as clcok source
  Delay300();                                               // delay at least 300 µs
  CGU_EntityConnect(CGU_CLKSRC_XTAL_OSC, CGU_CLKSRC_PLL1);  // connect XTAL to PLL1
  Delay300();                                               // delay at least 300 µs

  /* STEP 1: set cpu to mid frequency (according to datasheet) */
  CGU_SetPLL1(8);                             // f_osc x 8 = 96 MHz
  Delay300();                                 // delay at least 300 µs
  CGU_EnableEntity(CGU_CLKSRC_PLL1, ENABLE);  // Enable PLL1 after setting is done
  Delay300();                                 // delay at least 300 µs
  CGU_EntityConnect(CGU_CLKSRC_PLL1, CGU_BASE_M4);
  Delay300();  // delay at least 300 µs
  CGU_UpdateClock();
  Delay300();  // delay at least 300 µs

  /* STEP 2: set cpu to high frequency */
  CGU_SetPLL1(17);    // set PLL1 to: f_osc x 17 = 204 MHz
  Delay300();         // delay at least 300 µs
  CGU_UpdateClock();  // Update Clock Frequency
  Delay300();         // delay at least 300 µs

  /* connect UART0 to PLL1 */
  CGU_EntityConnect(CGU_CLKSRC_PLL1, CGU_BASE_UART2);
  Delay300();  // delay at least 300 µs
  CGU_UpdateClock();
  Delay300();  // delay at least 300 µs

  /* connect USB0 to PLL0 which is set to 480 MHz */
  CGU_EnableEntity(CGU_CLKSRC_PLL0, DISABLE);
  Delay300();  // delay at least 300 µs
  CGU_SetPLL0();
  Delay300();  // delay at least 300 µs
  CGU_EnableEntity(CGU_CLKSRC_PLL0, ENABLE);
  Delay300();  // delay at least 300 µs
  CGU_UpdateClock();
  Delay300();  // delay at least 300 µs
  CGU_EntityConnect(CGU_CLKSRC_PLL0, CGU_BASE_USB0);
  Delay300();  // delay at least 300 µs

  /* connect USB1 to PLL0 (480MHz) via /8 divider to get the required 60MHz */
  /*                     IDIV=4          AUTOBLOCK   CLK_SEL=PLL0USB */
  LPC_CGU->IDIVA_CTRL = ((4 - 1) << 2) | (1 << 11) | (0x07 << 24);  // setup IDIVA divider for PLL0USB/2
  /*                     IDIV=2          AUTOBLOCK   CLK_SEL=IDIVA */
  LPC_CGU->IDIVB_CTRL = ((2 - 1) << 2) | (1 << 11) | (0x0C << 24);  // setup IDIVB divider for IDIVA/2
  /*                       AUTOBLOCK   CLK_SEL=IDIVB */
  LPC_CGU->BASE_USB1_CLK = (1 << 11) | (0x0D << 24);  //  connect USB1_CLK to IDIVB

  /* nni: connect SSP0 and SSP1 to the PLL1 */
  CGU_EntityConnect(CGU_CLKSRC_PLL1, CGU_BASE_SSP0);
  Delay300();  // delay at least 300 µs
  CGU_EntityConnect(CGU_CLKSRC_PLL1, CGU_BASE_SSP1);
  Delay300();  // delay at least 300 µs
  CGU_UpdateClock();

#if 0  // these variables are for monitoring the frequencies while development
	volatile uint32_t usb0Clk = CGU_GetPCLKFrequency(CGU_PERIPHERAL_USB0);
	volatile uint32_t cpuClk = CGU_GetPCLKFrequency(CGU_PERIPHERAL_M3CORE);
	volatile uint32_t uartClk = CGU_GetPCLKFrequency(CGU_PERIPHERAL_UART0);
	volatile uint32_t usb1Clk = CGU_GetPCLKFrequency(CGU_PERIPHERAL_USB1);
#endif
}

/******************************************************************************/
/** @brief  	modul internal delay function - waits at least 300 µs @ 200 MHz
				with lower frequencies it waits longer
*******************************************************************************/
static void Delay300(void)
{
  register uint32_t cnt = 60000;  // 60'000 * 5ns = 300us
  while (--cnt)
    asm volatile("nop");  // 1 cycle = 5ns (@200mHz)
}
