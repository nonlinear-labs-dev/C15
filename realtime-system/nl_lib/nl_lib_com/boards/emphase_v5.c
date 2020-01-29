/******************************************************************************/
/** @file		emphase_v5.c
    @date		2015-11-30 NNI
    @author		2015-01-27 DTZ
    changes:
    	2019-11-18 KSTR	:
    		added pins for supervisor communication (toggle line and
    		non-maskable "always unmute" hardware jumper
*******************************************************************************/

#include <stdint.h>
#include "drv/nl_pin.h"
#include "cmsis/lpc43xx_cgu.h"
#include "drv/nl_gpio.h"

#include "drv/nl_dbg.h"
#include "espi/nl_espi_core.h"
#include "drv/nl_kbs.h"
#include "spibb/nl_spi_bb.h"
#include "sup/nl_sup.h"

static void ConfigureClocks(void);
static void Delay100(void);
static void InitDebugPins(void);
static void InitEspiPins(void);
static void InitBbbLpcSpiPins(void);
static void InitKeybedScannerPins(void);
static void InitSupPins(void);

/*******************************************************************************
	main init function
*******************************************************************************/
void EMPHASE_V5_M4_Init(void)
{
  ConfigureClocks();

  InitBbbLpcSpiPins();
  InitDebugPins();
  InitSupPins();
}

void EMPHASE_V5_M0_Init(void)
{
  InitDebugPins();
  InitEspiPins();
  InitKeybedScannerPins();
}

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
static void ConfigureClocks(void)
{
  /* XTAL OSC */
  CGU_SetXTALOSC(12000000);                                 // set f_osc = 12 MHz (external XTAL OSC is a 12 MHz device)
  CGU_EnableEntity(CGU_CLKSRC_XTAL_OSC, ENABLE);            // Enable xtal osc clock entity as clcok source
  Delay100();                                               // delay about 100 µs
  CGU_EntityConnect(CGU_CLKSRC_XTAL_OSC, CGU_CLKSRC_PLL1);  // connect XTAL to PLL1

  /* STEP 1: set cpu to mid frequency (according to datasheet) */
  CGU_SetPLL1(8);                             // f_osc x 8 = 96 MHz
  CGU_EnableEntity(CGU_CLKSRC_PLL1, ENABLE);  // Enable PLL1 after setting is done
  CGU_EntityConnect(CGU_CLKSRC_PLL1, CGU_BASE_M4);
  CGU_UpdateClock();

  /* STEP 2: set cpu to high frequency */
  CGU_SetPLL1(17);    // set PLL1 to: f_osc x 17 = 204 MHz
  CGU_UpdateClock();  // Update Clock Frequency

  /* connect UART0 to PLL1 */
  CGU_EntityConnect(CGU_CLKSRC_PLL1, CGU_BASE_UART2);
  CGU_UpdateClock();

  /* connect USB0 to PLL0 which is set to 480 MHz */
  CGU_EnableEntity(CGU_CLKSRC_PLL0, DISABLE);
  CGU_SetPLL0();
  CGU_EnableEntity(CGU_CLKSRC_PLL0, ENABLE);
  CGU_UpdateClock();
  CGU_EntityConnect(CGU_CLKSRC_PLL0, CGU_BASE_USB0);

  /* nni: connect SSP0 and SSP1 to the PLL1 */
  CGU_EntityConnect(CGU_CLKSRC_PLL1, CGU_BASE_SSP0);
  CGU_EntityConnect(CGU_CLKSRC_PLL1, CGU_BASE_SSP1);
  CGU_UpdateClock();

#if 0  // these variables are for monitoring the frequencies while development
	volatile uint32_t usb0Clk = CGU_GetPCLKFrequency(CGU_PERIPHERAL_USB0);
	volatile uint32_t cpuClk = CGU_GetPCLKFrequency(CGU_PERIPHERAL_M3CORE);
	volatile uint32_t uartClk = CGU_GetPCLKFrequency(CGU_PERIPHERAL_UART0);
	volatile uint32_t usb1Clk = CGU_GetPCLKFrequency(CGU_PERIPHERAL_USB1);
#endif
}

/******************************************************************************/
/** @brief  	modul internal delay function - waits at least 100 µs @ 180 MHz
				with lower frequencies it waits longer
*******************************************************************************/
static void Delay100(void)
{
  uint32_t cnt = 20000;
  for (; cnt > 0; cnt--)
    ;
}

/*******************************************************************************
	espi module
*******************************************************************************/
static PIN_CFG_T lpc_espi_data_sck = {
  .pinId        = { 3, 3 },
  .ioType       = PIN_TYPE_PIN,
  .inputBuffer  = PIN_INBUF_OFF,
  .glitchFilter = PIN_FILTER_ON,
  .slewRate     = PIN_SRATE_FAST,
  .pullDown     = PIN_PDN_OFF,
  .pullUp       = PIN_PUP_OFF,
  .function     = 2
};

static PIN_CFG_T lpc_espi_data_sdo = {
  .pinId        = { 3, 7 },
  .ioType       = PIN_TYPE_PIN,
  .inputBuffer  = PIN_INBUF_OFF,
  .glitchFilter = PIN_FILTER_ON,
  .slewRate     = PIN_SRATE_FAST,
  .pullDown     = PIN_PDN_OFF,
  .pullUp       = PIN_PUP_OFF,
  .function     = 5
};

static PIN_CFG_T lpc_espi_data_sdi = {
  .pinId        = { 3, 6 },
  .ioType       = PIN_TYPE_PIN,
  .inputBuffer  = PIN_INBUF_ON,
  .glitchFilter = PIN_FILTER_ON,
  .slewRate     = PIN_SRATE_FAST,
  .pullDown     = PIN_PDN_OFF,
  .pullUp       = PIN_PUP_OFF,
  .function     = 5
};

static PIN_CFG_T lpc_espi_data_sap = {
  .pinId        = { 3, 8 },
  .ioType       = PIN_TYPE_GPIO,
  .gpioId       = { 5, 11 },
  .direction    = PIN_GPIO_DIR_OUT,
  .inputBuffer  = PIN_INBUF_OFF,
  .glitchFilter = PIN_FILTER_ON,
  .slewRate     = PIN_SRATE_SLOW,
  .pullDown     = PIN_PDN_OFF,
  .pullUp       = PIN_PUP_OFF,
  .function     = 4
};

static PIN_CFG_T lpc_espi_data_dmx = {
  .pinId        = { 7, 3 },
  .ioType       = PIN_TYPE_GPIO,
  .gpioId       = { 3, 11 },
  .direction    = PIN_GPIO_DIR_OUT,
  .inputBuffer  = PIN_INBUF_OFF,
  .glitchFilter = PIN_FILTER_ON,
  .slewRate     = PIN_SRATE_SLOW,
  .pullDown     = PIN_PDN_OFF,
  .pullUp       = PIN_PUP_OFF,
  .function     = 0
};

static PIN_CFG_T lpc_espi_scs[6] = {
  {
      .pinId        = { 7, 0 },
      .ioType       = PIN_TYPE_GPIO,
      .gpioId       = { 3, 8 },
      .direction    = PIN_GPIO_DIR_OUT,
      .inputBuffer  = PIN_INBUF_OFF,
      .glitchFilter = PIN_FILTER_ON,
      .slewRate     = PIN_SRATE_SLOW,
      .pullDown     = PIN_PDN_OFF,
      .pullUp       = PIN_PUP_OFF,
      .function     = 0,
  },
  {
      .pinId        = { 7, 1 },
      .ioType       = PIN_TYPE_GPIO,
      .gpioId       = { 3, 9 },
      .direction    = PIN_GPIO_DIR_OUT,
      .inputBuffer  = PIN_INBUF_OFF,
      .glitchFilter = PIN_FILTER_ON,
      .slewRate     = PIN_SRATE_SLOW,
      .pullDown     = PIN_PDN_OFF,
      .pullUp       = PIN_PUP_OFF,
      .function     = 0,
  },
  {
      .pinId        = { 7, 2 },
      .ioType       = PIN_TYPE_GPIO,
      .gpioId       = { 3, 10 },
      .direction    = PIN_GPIO_DIR_OUT,
      .inputBuffer  = PIN_INBUF_OFF,
      .glitchFilter = PIN_FILTER_ON,
      .slewRate     = PIN_SRATE_SLOW,
      .pullDown     = PIN_PDN_OFF,
      .pullUp       = PIN_PUP_OFF,
      .function     = 0,
  },

  {
      .pinId        = { 7, 4 },
      .ioType       = PIN_TYPE_GPIO,
      .gpioId       = { 3, 12 },
      .direction    = PIN_GPIO_DIR_OUT,
      .inputBuffer  = PIN_INBUF_OFF,
      .glitchFilter = PIN_FILTER_ON,
      .slewRate     = PIN_SRATE_SLOW,
      .pullDown     = PIN_PDN_OFF,
      .pullUp       = PIN_PUP_OFF,
      .function     = 0,
  },
  {
      .pinId        = { 7, 5 },
      .ioType       = PIN_TYPE_GPIO,
      .gpioId       = { 3, 13 },
      .direction    = PIN_GPIO_DIR_OUT,
      .inputBuffer  = PIN_INBUF_OFF,
      .glitchFilter = PIN_FILTER_ON,
      .slewRate     = PIN_SRATE_SLOW,
      .pullDown     = PIN_PDN_OFF,
      .pullUp       = PIN_PUP_OFF,
      .function     = 0,
  },
  {
      .pinId        = { 7, 6 },
      .ioType       = PIN_TYPE_GPIO,
      .gpioId       = { 3, 14 },
      .direction    = PIN_GPIO_DIR_OUT,
      .inputBuffer  = PIN_INBUF_OFF,
      .glitchFilter = PIN_FILTER_ON,
      .slewRate     = PIN_SRATE_SLOW,
      .pullDown     = PIN_PDN_OFF,
      .pullUp       = PIN_PUP_OFF,
      .function     = 0,
  }
};

static ESPI_PINS_T espi_gpios = {
  .sap = &lpc_espi_data_sap.gpioId,
  .dmx = &lpc_espi_data_dmx.gpioId,

  .scs[0] = &lpc_espi_scs[0].gpioId,
  .scs[1] = &lpc_espi_scs[1].gpioId,
  .scs[2] = &lpc_espi_scs[2].gpioId,
  .scs[3] = &lpc_espi_scs[3].gpioId,
  .scs[4] = &lpc_espi_scs[4].gpioId,
  .scs[5] = &lpc_espi_scs[5].gpioId
};

static void InitEspiPins(void)
{
  PIN_Config(&lpc_espi_data_sck);
  PIN_Config(&lpc_espi_data_sdo);
  PIN_Config(&lpc_espi_data_sdi);
  PIN_Config(&lpc_espi_data_sap);
  PIN_Config(&lpc_espi_data_dmx);

  PIN_Config(&lpc_espi_scs[0]);
  PIN_Config(&lpc_espi_scs[1]);
  PIN_Config(&lpc_espi_scs[2]);
  PIN_Config(&lpc_espi_scs[3]);
  PIN_Config(&lpc_espi_scs[4]);
  PIN_Config(&lpc_espi_scs[5]);

  ESPI_Config(LPC_SSP0, &espi_gpios);
}

/*******************************************************************************
	bbb spi module
*******************************************************************************/
static PIN_CFG_T bbb_lpc_bilo = {
  .pinId        = { 1, 3 },
  .ioType       = PIN_TYPE_PIN,
  .inputBuffer  = PIN_INBUF_OFF,
  .glitchFilter = PIN_FILTER_ON,
  .slewRate     = PIN_SRATE_SLOW,
  .pullDown     = PIN_PDN_OFF,
  .pullUp       = PIN_PUP_OFF,
  .function     = 5
};

static PIN_CFG_T bbb_lpc_boli = {
  .pinId        = { 1, 4 },
  .ioType       = PIN_TYPE_PIN,
  .inputBuffer  = PIN_INBUF_ON,
  .glitchFilter = PIN_FILTER_ON,
  .slewRate     = PIN_SRATE_SLOW,
  .pullDown     = PIN_PDN_OFF,
  .pullUp       = PIN_PUP_OFF,
  .function     = 5
};

static PIN_CFG_T bbb_lpc_rdy = {
  .pinId        = { 1, 7 },
  .ioType       = PIN_TYPE_GPIO,
  .gpioId       = { 1, 0 },
  .direction    = PIN_GPIO_DIR_OUT,
  .inputBuffer  = PIN_INBUF_OFF,
  .glitchFilter = PIN_FILTER_ON,
  .slewRate     = PIN_SRATE_SLOW,
  .pullDown     = PIN_PDN_OFF,
  .pullUp       = PIN_PUP_OFF,
  .function     = 0
};

static PIN_CFG_T bbb_lpc_prq = {
  .pinId        = { 1, 8 },
  .ioType       = PIN_TYPE_GPIO,
  .gpioId       = { 1, 1 },
  .direction    = PIN_GPIO_DIR_OUT,
  .inputBuffer  = PIN_INBUF_OFF,
  .glitchFilter = PIN_FILTER_ON,
  .slewRate     = PIN_SRATE_SLOW,
  .pullDown     = PIN_PDN_OFF,
  .pullUp       = PIN_PUP_OFF,
  .function     = 0
};

static PIN_CFG_T bbb_lpc_scs = {
  .pinId        = { 1, 5 },
  .ioType       = PIN_TYPE_GPIO,
  .gpioId       = { 1, 8 },
  .direction    = PIN_GPIO_DIR_IN,
  .inputBuffer  = PIN_INBUF_ON,
  .glitchFilter = PIN_FILTER_ON,
  .slewRate     = PIN_SRATE_SLOW,
  .pullDown     = PIN_PDN_OFF,
  .pullUp       = PIN_PUP_OFF,
  .function     = 5
};

static PIN_CFG_T bbb_lpc_sp1 = {
  .pinId        = { 1, 10 },
  .ioType       = PIN_TYPE_GPIO,
  .gpioId       = { 1, 3 },
  .direction    = PIN_GPIO_DIR_OUT,
  .inputBuffer  = PIN_INBUF_OFF,
  .glitchFilter = PIN_FILTER_ON,
  .slewRate     = PIN_SRATE_SLOW,
  .pullDown     = PIN_PDN_OFF,
  .pullUp       = PIN_PUP_OFF,
  .function     = 0
};

static PIN_CFG_T bbb_lpc_sp2 = {
  .pinId        = { 1, 11 },
  .ioType       = PIN_TYPE_GPIO,
  .gpioId       = { 1, 4 },
  .direction    = PIN_GPIO_DIR_OUT,
  .inputBuffer  = PIN_INBUF_OFF,
  .glitchFilter = PIN_FILTER_ON,
  .slewRate     = PIN_SRATE_SLOW,
  .pullDown     = PIN_PDN_OFF,
  .pullUp       = PIN_PUP_OFF,
  .function     = 0
};

static PIN_CFG_T bbb_lpc_sck = {
  .pinId        = { 1, 19 },
  .ioType       = PIN_TYPE_PIN,
  .inputBuffer  = PIN_INBUF_ON,
  .glitchFilter = PIN_FILTER_ON,
  .slewRate     = PIN_SRATE_SLOW,
  .pullDown     = PIN_PDN_OFF,
  .pullUp       = PIN_PUP_OFF,
  .function     = 1
};

static SPI_BB_PINS_T bbb_lpc_pins = {
  .cs        = &bbb_lpc_scs.gpioId,
  .heartbeat = &bbb_lpc_sp2.gpioId,
  .prq       = &bbb_lpc_prq.gpioId,
  .rdy       = &bbb_lpc_rdy.gpioId
};

static void InitBbbLpcSpiPins(void)
{
  PIN_Config(&bbb_lpc_bilo);
  PIN_Config(&bbb_lpc_boli);
  PIN_Config(&bbb_lpc_prq);
  PIN_Config(&bbb_lpc_rdy);
  PIN_Config(&bbb_lpc_scs);
  PIN_Config(&bbb_lpc_sp1);
  PIN_Config(&bbb_lpc_sp2);
  PIN_Config(&bbb_lpc_sck);

  SPI_BB_Config(LPC_SSP1, &bbb_lpc_pins);
}

/*******************************************************************************
	keybed scanner module
	lines: outputs [4]
	keys:  inputs  [8]
	line 0 = a, line 1 = b, line 2 = c, line 3 = d
*******************************************************************************/
static PIN_CFG_T lpc_ks_line[4] = {
  {
      .pinId        = { 0, 0 },
      .ioType       = PIN_TYPE_GPIO,
      .gpioId       = { 0, 0 },
      .direction    = PIN_GPIO_DIR_OUT,
      .inputBuffer  = PIN_INBUF_OFF,
      .glitchFilter = PIN_FILTER_ON,
      .slewRate     = PIN_SRATE_FAST,
      .pullDown     = PIN_PDN_OFF,
      .pullUp       = PIN_PUP_OFF,
      .function     = 0,
  },
  {
      .pinId        = { 0, 1 },
      .ioType       = PIN_TYPE_GPIO,
      .gpioId       = { 0, 1 },
      .direction    = PIN_GPIO_DIR_OUT,
      .inputBuffer  = PIN_INBUF_OFF,
      .glitchFilter = PIN_FILTER_ON,
      .slewRate     = PIN_SRATE_FAST,
      .pullDown     = PIN_PDN_OFF,
      .pullUp       = PIN_PUP_OFF,
      .function     = 0,
  },
  {
      .pinId        = { 1, 15 },
      .ioType       = PIN_TYPE_GPIO,
      .gpioId       = { 0, 2 },
      .direction    = PIN_GPIO_DIR_OUT,
      .inputBuffer  = PIN_INBUF_OFF,
      .glitchFilter = PIN_FILTER_ON,
      .slewRate     = PIN_SRATE_FAST,
      .pullDown     = PIN_PDN_OFF,
      .pullUp       = PIN_PUP_OFF,
      .function     = 0,
  },
  { .pinId        = { 1, 16 },
    .ioType       = PIN_TYPE_GPIO,
    .gpioId       = { 0, 3 },
    .direction    = PIN_GPIO_DIR_OUT,
    .inputBuffer  = PIN_INBUF_OFF,
    .glitchFilter = PIN_FILTER_ON,
    .slewRate     = PIN_SRATE_FAST,
    .pullDown     = PIN_PDN_OFF,
    .pullUp       = PIN_PUP_OFF,
    .function     = 0 }
};

static PIN_CFG_T lpc_ks_key[8] = {
  {
      .pinId        = { 2, 0 },
      .ioType       = PIN_TYPE_GPIO,
      .gpioId       = { 5, 0 },
      .direction    = PIN_GPIO_DIR_IN,
      .inputBuffer  = PIN_INBUF_ON,
      .glitchFilter = PIN_FILTER_ON,
      .slewRate     = PIN_SRATE_FAST,
      .pullDown     = PIN_PDN_OFF,
      .pullUp       = PIN_PUP_OFF,
      .function     = 4,
  },
  {
      .pinId        = { 2, 1 },
      .ioType       = PIN_TYPE_GPIO,
      .gpioId       = { 5, 1 },
      .direction    = PIN_GPIO_DIR_IN,
      .inputBuffer  = PIN_INBUF_ON,
      .glitchFilter = PIN_FILTER_ON,
      .slewRate     = PIN_SRATE_FAST,
      .pullDown     = PIN_PDN_OFF,
      .pullUp       = PIN_PUP_OFF,
      .function     = 4,
  },
  {
      .pinId        = { 2, 2 },
      .ioType       = PIN_TYPE_GPIO,
      .gpioId       = { 5, 2 },
      .direction    = PIN_GPIO_DIR_IN,
      .inputBuffer  = PIN_INBUF_ON,
      .glitchFilter = PIN_FILTER_ON,
      .slewRate     = PIN_SRATE_FAST,
      .pullDown     = PIN_PDN_OFF,
      .pullUp       = PIN_PUP_OFF,
      .function     = 4,
  },
  {
      .pinId        = { 2, 3 },
      .ioType       = PIN_TYPE_GPIO,
      .gpioId       = { 5, 3 },
      .direction    = PIN_GPIO_DIR_IN,
      .inputBuffer  = PIN_INBUF_ON,
      .glitchFilter = PIN_FILTER_ON,
      .slewRate     = PIN_SRATE_FAST,
      .pullDown     = PIN_PDN_OFF,
      .pullUp       = PIN_PUP_OFF,
      .function     = 4,
  },
  {
      .pinId        = { 2, 4 },
      .ioType       = PIN_TYPE_GPIO,
      .gpioId       = { 5, 4 },
      .direction    = PIN_GPIO_DIR_IN,
      .inputBuffer  = PIN_INBUF_ON,
      .glitchFilter = PIN_FILTER_ON,
      .slewRate     = PIN_SRATE_FAST,
      .pullDown     = PIN_PDN_OFF,
      .pullUp       = PIN_PUP_OFF,
      .function     = 4,
  },
  {
      .pinId        = { 2, 5 },
      .ioType       = PIN_TYPE_GPIO,
      .gpioId       = { 5, 5 },
      .direction    = PIN_GPIO_DIR_IN,
      .inputBuffer  = PIN_INBUF_ON,
      .glitchFilter = PIN_FILTER_ON,
      .slewRate     = PIN_SRATE_FAST,
      .pullDown     = PIN_PDN_OFF,
      .pullUp       = PIN_PUP_OFF,
      .function     = 4,
  },
  {
      .pinId        = { 2, 6 },
      .ioType       = PIN_TYPE_GPIO,
      .gpioId       = { 5, 6 },
      .direction    = PIN_GPIO_DIR_IN,
      .inputBuffer  = PIN_INBUF_ON,
      .glitchFilter = PIN_FILTER_ON,
      .slewRate     = PIN_SRATE_FAST,
      .pullDown     = PIN_PDN_OFF,
      .pullUp       = PIN_PUP_OFF,
      .function     = 4,
  },
  {
      .pinId        = { 2, 8 },
      .ioType       = PIN_TYPE_GPIO,
      .gpioId       = { 5, 7 },
      .direction    = PIN_GPIO_DIR_IN,
      .inputBuffer  = PIN_INBUF_ON,
      .glitchFilter = PIN_FILTER_ON,
      .slewRate     = PIN_SRATE_FAST,
      .pullDown     = PIN_PDN_OFF,
      .pullUp       = PIN_PUP_OFF,
      .function     = 4,
  }
};

static PIN_CFG_T lpc_ks_dmx = {
  .pinId        = { 4, 2 },
  .ioType       = PIN_TYPE_GPIO,
  .gpioId       = { 2, 2 },
  .direction    = PIN_GPIO_DIR_OUT,
  .inputBuffer  = PIN_INBUF_OFF,
  .glitchFilter = PIN_FILTER_ON,
  .slewRate     = PIN_SRATE_FAST,
  .pullDown     = PIN_PDN_OFF,
  .pullUp       = PIN_PUP_OFF,
  .function     = 0
};

static KBS_PINS_T keybed_pins = {
  .line[0] = &lpc_ks_line[0].gpioId,
  .line[1] = &lpc_ks_line[1].gpioId,
  .line[2] = &lpc_ks_line[2].gpioId,
  .line[3] = &lpc_ks_line[3].gpioId,

  .key[0] = &lpc_ks_key[0].gpioId,
  .key[1] = &lpc_ks_key[1].gpioId,
  .key[2] = &lpc_ks_key[2].gpioId,
  .key[3] = &lpc_ks_key[3].gpioId,
  .key[4] = &lpc_ks_key[4].gpioId,
  .key[5] = &lpc_ks_key[5].gpioId,
  .key[6] = &lpc_ks_key[6].gpioId,
  .key[7] = &lpc_ks_key[7].gpioId,

  .dmx = &lpc_ks_dmx.gpioId
};

static void InitKeybedScannerPins(void)
{
  PIN_Config(&lpc_ks_line[0]);
  PIN_Config(&lpc_ks_line[1]);
  PIN_Config(&lpc_ks_line[2]);
  PIN_Config(&lpc_ks_line[3]);

  PIN_Config(&lpc_ks_key[0]);
  PIN_Config(&lpc_ks_key[1]);
  PIN_Config(&lpc_ks_key[2]);
  PIN_Config(&lpc_ks_key[3]);
  PIN_Config(&lpc_ks_key[4]);
  PIN_Config(&lpc_ks_key[5]);
  PIN_Config(&lpc_ks_key[6]);
  PIN_Config(&lpc_ks_key[7]);

  PIN_Config(&lpc_ks_dmx);

  KBS_Config(&keybed_pins);
  KBS_Init();
}

/*******************************************************************************
	debug module
*******************************************************************************/
static PIN_CFG_T lpc_dbg_led_m4_hb = {
  .pinId        = { 5, 2 },
  .ioType       = PIN_TYPE_GPIO,
  .gpioId       = { 2, 11 },
  .direction    = PIN_GPIO_DIR_OUT,
  .inputBuffer  = PIN_INBUF_OFF,
  .glitchFilter = PIN_FILTER_ON,
  .slewRate     = PIN_SRATE_SLOW,
  .pullDown     = PIN_PDN_OFF,
  .pullUp       = PIN_PUP_OFF,
  .function     = 0
};

static PIN_CFG_T lpc_dbg_led_task_ovfl = {
  .pinId        = { 5, 4 },
  .ioType       = PIN_TYPE_GPIO,
  .gpioId       = { 2, 13 },
  .direction    = PIN_GPIO_DIR_OUT,
  .inputBuffer  = PIN_INBUF_OFF,
  .glitchFilter = PIN_FILTER_ON,
  .slewRate     = PIN_SRATE_SLOW,
  .pullDown     = PIN_PDN_OFF,
  .pullUp       = PIN_PUP_OFF,
  .function     = 0
};

static PIN_CFG_T lpc_dbg_led_error = {
  .pinId        = { 5, 5 },
  .ioType       = PIN_TYPE_GPIO,
  .gpioId       = { 2, 14 },
  .direction    = PIN_GPIO_DIR_OUT,
  .inputBuffer  = PIN_INBUF_OFF,
  .glitchFilter = PIN_FILTER_ON,
  .slewRate     = PIN_SRATE_SLOW,
  .pullDown     = PIN_PDN_OFF,
  .pullUp       = PIN_PUP_OFF,
  .function     = 0
};

static PIN_CFG_T lpc_dbg_led_m0_hb = {
  .pinId        = { 5, 1 },
  .ioType       = PIN_TYPE_GPIO,
  .gpioId       = { 2, 10 },
  .direction    = PIN_GPIO_DIR_OUT,
  .inputBuffer  = PIN_INBUF_OFF,
  .glitchFilter = PIN_FILTER_ON,
  .slewRate     = PIN_SRATE_SLOW,
  .pullDown     = PIN_PDN_OFF,
  .pullUp       = PIN_PUP_OFF,
  .function     = 0
};

static PIN_CFG_T lpc_dbg_led_audio_ok = {
  .pinId        = { 5, 3 },
  .ioType       = PIN_TYPE_GPIO,
  .gpioId       = { 2, 12 },
  .direction    = PIN_GPIO_DIR_OUT,
  .inputBuffer  = PIN_INBUF_OFF,
  .glitchFilter = PIN_FILTER_ON,
  .slewRate     = PIN_SRATE_SLOW,
  .pullDown     = PIN_PDN_OFF,
  .pullUp       = PIN_PUP_OFF,
  .function     = 0
};

static PIN_CFG_T lpc_dbg_pod[] = {
  {
      .pinId        = { 6, 2 },
      .ioType       = PIN_TYPE_GPIO,
      .gpioId       = { 3, 1 },
      .direction    = PIN_GPIO_DIR_OUT,
      .inputBuffer  = PIN_INBUF_OFF,
      .glitchFilter = PIN_FILTER_ON,
      .slewRate     = PIN_SRATE_SLOW,
      .pullDown     = PIN_PDN_OFF,
      .pullUp       = PIN_PUP_OFF,
      .function     = 0,
  },
  {
      .pinId        = { 6, 3 },
      .ioType       = PIN_TYPE_GPIO,
      .gpioId       = { 3, 2 },
      .direction    = PIN_GPIO_DIR_OUT,
      .inputBuffer  = PIN_INBUF_OFF,
      .glitchFilter = PIN_FILTER_ON,
      .slewRate     = PIN_SRATE_SLOW,
      .pullDown     = PIN_PDN_OFF,
      .pullUp       = PIN_PUP_OFF,
      .function     = 0,
  },
  {
      .pinId        = { 6, 4 },
      .ioType       = PIN_TYPE_GPIO,
      .gpioId       = { 3, 3 },
      .direction    = PIN_GPIO_DIR_OUT,
      .inputBuffer  = PIN_INBUF_OFF,
      .glitchFilter = PIN_FILTER_ON,
      .slewRate     = PIN_SRATE_SLOW,
      .pullDown     = PIN_PDN_OFF,
      .pullUp       = PIN_PUP_OFF,
      .function     = 0,
  },
  {
      .pinId        = { 6, 5 },
      .ioType       = PIN_TYPE_GPIO,
      .gpioId       = { 3, 4 },
      .direction    = PIN_GPIO_DIR_OUT,
      .inputBuffer  = PIN_INBUF_OFF,
      .glitchFilter = PIN_FILTER_ON,
      .slewRate     = PIN_SRATE_SLOW,
      .pullDown     = PIN_PDN_OFF,
      .pullUp       = PIN_PUP_OFF,
      .function     = 0,
  }
};

static PIN_CFG_T lpc_dbg_uart_tx = {
  .pinId        = { 2, 10 },
  .ioType       = PIN_TYPE_PIN,
  .inputBuffer  = PIN_INBUF_OFF,
  .glitchFilter = PIN_FILTER_ON,
  .slewRate     = PIN_SRATE_SLOW,
  .pullDown     = PIN_PDN_OFF,
  .pullUp       = PIN_PUP_OFF,
  .function     = 2
};

static DBG_PINS_T debug_pins = {
  .led_heartbeat = &lpc_dbg_led_m4_hb.gpioId,
  .led_warning   = &lpc_dbg_led_task_ovfl.gpioId,
  .led_error     = &lpc_dbg_led_error.gpioId,
  .led_cpu       = &lpc_dbg_led_m0_hb.gpioId,
  .led_audio     = &lpc_dbg_led_audio_ok.gpioId,

  .pod[0] = &lpc_dbg_pod[0].gpioId,
  .pod[1] = &lpc_dbg_pod[1].gpioId,
  .pod[2] = &lpc_dbg_pod[2].gpioId,
  .pod[3] = &lpc_dbg_pod[3].gpioId,
};

static void InitDebugPins(void)
{
  PIN_Config(&lpc_dbg_led_m4_hb);
  PIN_Config(&lpc_dbg_led_task_ovfl);
  PIN_Config(&lpc_dbg_led_error);
  PIN_Config(&lpc_dbg_led_m0_hb);
  PIN_Config(&lpc_dbg_led_audio_ok);
  PIN_Config(&lpc_dbg_uart_tx);

  PIN_Config(&lpc_dbg_pod[0]);
  PIN_Config(&lpc_dbg_pod[1]);
  PIN_Config(&lpc_dbg_pod[2]);
  PIN_Config(&lpc_dbg_pod[3]);

  DBG_Config(LPC_USART2, 115200, &debug_pins);
}

/*******************************************************************************
	supervisor module
*******************************************************************************/
static PIN_CFG_T lpc_sup_mute_req = {
  .pinId        = { 5, 6 },
  .ioType       = PIN_TYPE_GPIO,
  .gpioId       = { 2, 15 },
  .direction    = PIN_GPIO_DIR_OUT,
  .inputBuffer  = PIN_INBUF_OFF,
  .glitchFilter = PIN_FILTER_ON,
  .slewRate     = PIN_SRATE_SLOW,
  .pullDown     = PIN_PDN_ON,
  .pullUp       = PIN_PUP_OFF,
  .function     = 0
};

static PIN_CFG_T lpc_sup_mute_ack = {
  .pinId        = { 5, 7 },
  .ioType       = PIN_TYPE_GPIO,
  .gpioId       = { 2, 7 },
  .direction    = PIN_GPIO_DIR_IN,
  .inputBuffer  = PIN_INBUF_ON,
  .glitchFilter = PIN_FILTER_ON,
  .slewRate     = PIN_SRATE_SLOW,
  .pullDown     = PIN_PDN_ON,
  .pullUp       = PIN_PUP_OFF,
  .function     = 0
};

static PIN_CFG_T lpc_unmute_jumper = {
  .pinId  = { 6, 6 },
  .ioType = PIN_TYPE_GPIO,
#warning "Does not work, reads only 1.5V with pullup"
  .gpioId       = { 0, 5 },
  .direction    = PIN_GPIO_DIR_IN,
  .inputBuffer  = PIN_INBUF_ON,
  .glitchFilter = PIN_FILTER_ON,
  .slewRate     = PIN_SRATE_SLOW,
  .pullDown     = PIN_PDN_OFF,
  .pullUp       = PIN_PUP_ON,
  .function     = 0,
};

static SUP_PINS_T sup_pins = {
  .lpc_sup_mute_request = &lpc_sup_mute_req.gpioId,
  .lpc_sup_mute_status  = &lpc_sup_mute_ack.gpioId,
  .lpc_unmute_jumper    = &lpc_unmute_jumper.gpioId
};

static void InitSupPins(void)
{
  PIN_Config(&lpc_sup_mute_req);
  PIN_Config(&lpc_sup_mute_ack);
  PIN_Config(&lpc_unmute_jumper);

  SUP_Config(&sup_pins);
}

// EOF
