#pragma once

#include "pin_setup.h"

//void PINS_Init(void);

// clang-format off

//
// --------------- debug pins and LEDs ----------------
//
// LEDs :
#define LED_A_init() GPIO_DIR_OUT(2, 10); SFSP(5, 1) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 0
#define LED_A           GPIO_Word(2, 10)
#define LED_B_init() GPIO_DIR_OUT(2, 11); SFSP(5, 2) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 0
#define LED_B           GPIO_Word(2, 11)
#define LED_C_init() GPIO_DIR_OUT(2, 12); SFSP(5, 3) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 0
#define LED_C           GPIO_Word(2, 12)
#define LED_D_init() GPIO_DIR_OUT(2, 13); SFSP(5, 4) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 0
#define LED_D           GPIO_Word(2, 13)
#define LED_E_init() GPIO_DIR_OUT(2, 14); SFSP(5, 5) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 0
#define LED_E           GPIO_Word(2, 14)

// debug GPIO's : all outputs
#define pinGPIO_3_1_init() GPIO_DIR_OUT(3, 1); SFSP(6, 2) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 0
#define pinGPIO_3_1           GPIO_Word(3, 1)
#define pinGPIO_3_2_init() GPIO_DIR_OUT(3, 2); SFSP(6, 3) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 0
#define pinGPIO_3_2           GPIO_Word(3, 2)
#define pinGPIO_3_3_init() GPIO_DIR_OUT(3, 3); SFSP(6, 4) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 0
#define pinGPIO_3_3           GPIO_Word(3, 3)
#define pinGPIO_3_4_init() GPIO_DIR_OUT(3, 4); SFSP(6, 5) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 0
#define pinGPIO_3_4           GPIO_Word(3, 4)

// LED assignments :
#define ledM0heartbeat  LED_A
#define ledM4heartbeat  LED_B
#define ledAudioOk      LED_C
#define ledWarning      LED_D
#define ledError        LED_E

// board ID : input with external 10k pullup (HW V1.7) or open (older), so we need a soft pulldown
#define pinBoardID_init() GPIO_DIR_IN(3, 0); SFSP(6, 1) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_DPU + 0
#define pinBoardID           GPIO_Bit(3, 0)

static inline void debugPinsInit(void)
{
  LED_A_init();
  LED_B_init();
  LED_C_init();
  LED_D_init();
  LED_E_init();
  ledM0heartbeat = 1;
  ledM4heartbeat = 1;
  ledAudioOk     = 1;
  ledWarning     = 1;
  ledError       = 1;

  pinGPIO_3_1_init();
  pinGPIO_3_2_init();
  pinGPIO_3_3_init();
  pinGPIO_3_4_init();
  pinGPIO_3_1 = 0;
  pinGPIO_3_2 = 0;
  pinGPIO_3_3 = 0;
  pinGPIO_3_4 = 0;

  pinBoardID_init();
}

//
// --------------- ESPI pins (M0 eSPI interface for sensors, master) ----------------
//
#define pinEspi_SCK_init() SFSP(3, 3) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 2
#define pinEspi_SDO_init() SFSP(3, 7) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 5
#define pinEspi_SDI_init() SFSP(3, 6) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 5

#define pinEspi_SAP_init()   GPIO_DIR_OUT(5, 11); SFSP(3, 8) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 4
#define pinEspi_SAPb             GPIO_Bit(5, 11)
#define pinEspi_DMX_init()   GPIO_DIR_OUT(3, 11); SFSP(7, 3) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 0
#define pinEspi_DMXb             GPIO_Bit(3, 11)
#define pPinEspi_SCS0_init() GPIO_DIR_OUT(3,  8); SFSP(7, 0) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 0
#define pPinEspi_SCS0w         pGPIO_Word(3,  8)
#define pPinEspi_SCS1_init() GPIO_DIR_OUT(3,  9); SFSP(7, 1) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 0
#define pPinEspi_SCS1w         pGPIO_Word(3,  9)
#define pPinEspi_SCS2_init() GPIO_DIR_OUT(3, 10); SFSP(7, 2) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 0
#define pPinEspi_SCS2w         pGPIO_Word(3, 10)
#define pPinEspi_SCS3_init() GPIO_DIR_OUT(3, 12); SFSP(7, 4) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 0
#define pPinEspi_SCS3w         pGPIO_Word(3, 12)
#define pPinEspi_SCS4_init() GPIO_DIR_OUT(3, 13); SFSP(7, 5) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 0
#define pPinEspi_SCS4w         pGPIO_Word(3, 13)
#define pPinEspi_SCS5_init() GPIO_DIR_OUT(3, 14); SFSP(7, 6) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 0
#define pPinEspi_SCS5w         pGPIO_Word(3, 14)

static inline void espiPinsInit(void)
{
  pinEspi_SCK_init();
  pinEspi_SDO_init();
  pinEspi_SDI_init();
  pinEspi_SAP_init();
  pinEspi_DMX_init();
  pPinEspi_SCS0_init();
  pPinEspi_SCS1_init();
  pPinEspi_SCS2_init();
  pPinEspi_SCS3_init();
  pPinEspi_SCS4_init();
  pPinEspi_SCS5_init();
}

//
// --------------- BSPI pins (M4 SPI interface from BBB, slave) ----------------
//
#define pinBspiSCK_init()  SFSP(1, 19) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 1
#define pinBspiBILO_init() SFSP(1,  3) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 5
#define pinBspiBOLI_init() SFSP(1,  4) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 5

#define pinBspi_RDY_init() GPIO_DIR_OUT(1, 0); SFSP(1,  7) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 0
#define pinBspi_RDYb           GPIO_Bit(1, 0)
#define pinBspi_PRQ_init() GPIO_DIR_OUT(1, 1); SFSP(1,  8) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 0
#define pinBspi_PRQb           GPIO_Bit(1, 1)
#define pinBspi_SCS_init()  GPIO_DIR_IN(1, 8); SFSP(1,  5) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 5
#define pinBspi_SCSb           GPIO_Bit(1, 8)
#define pinBspi_HBT_init() GPIO_DIR_OUT(1, 4); SFSP(1, 11) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 0
#define pinBspi_HBTb           GPIO_Bit(1, 4)
#define pinBspi_AUX_init()  GPIO_DIR_IN(1, 3); SFSP(1, 10) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 0
#define pinBspi_AUXb           GPIO_Bit(1, 3)

static inline void bspiPinsInit(void)
{
  pinBspiSCK_init();
  pinBspiBILO_init();
  pinBspiBOLI_init();
  pinBspi_RDY_init();
  pinBspi_PRQ_init();
  pinBspi_SCS_init();
  pinBspi_HBT_init();
  pinBspi_AUX_init();
}

//
// --------------- Keybed Scanner pins ----------------
//
#define pinKbs_DMX_init()   GPIO_DIR_OUT(2, 2); SFSP(4,  2) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 0
#define pinKbs_DMX             GPIO_Word(2, 2)
#define pinKbs_LineA_init() GPIO_DIR_OUT(0, 0); SFSP(0,  0) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 0
#define pinKbs_LineA           GPIO_Word(0, 0)
#define pinKbs_LineB_init() GPIO_DIR_OUT(0, 1); SFSP(0,  1) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 0
#define pinKbs_LineB           GPIO_Word(0, 1)
#define pinKbs_LineC_init() GPIO_DIR_OUT(0, 2); SFSP(1, 15) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 0
#define pinKbs_LineC           GPIO_Word(0, 2)
#define pinKbs_LineD_init() GPIO_DIR_OUT(0, 3); SFSP(1, 16) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 0
#define pinKbs_LineD           GPIO_Word(0, 3)

#define pinKbs_Key0_init() GPIO_DIR_IN(5, 0); SFSP(2, 0) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 4
#define pinKbs_Key0          GPIO_Word(5, 0)
#define pinKbs_Key1_init() GPIO_DIR_IN(5, 1); SFSP(2, 1) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 4
#define pinKbs_Key1          GPIO_Word(5, 1)
#define pinKbs_Key2_init() GPIO_DIR_IN(5, 2); SFSP(2, 2) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 4
#define pinKbs_Key2          GPIO_Word(5, 2)
#define pinKbs_Key3_init() GPIO_DIR_IN(5, 3); SFSP(2, 3) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 4
#define pinKbs_Key3          GPIO_Word(5, 3)
#define pinKbs_Key4_init() GPIO_DIR_IN(5, 4); SFSP(2, 4) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 4
#define pinKbs_Key4          GPIO_Word(5, 4)
#define pinKbs_Key5_init() GPIO_DIR_IN(5, 5); SFSP(2, 5) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 4
#define pinKbs_Key5          GPIO_Word(5, 5)
#define pinKbs_Key6_init() GPIO_DIR_IN(5, 6); SFSP(2, 6) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 4
#define pinKbs_Key6          GPIO_Word(5, 6)
#define pinKbs_Key7_init() GPIO_DIR_IN(5, 7); SFSP(2, 8) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 4
#define pinKbs_Key7          GPIO_Word(5, 7)

static inline void kbsPinsInit(void)
{
  pinKbs_DMX_init();
  pinKbs_DMX = 1;
  pinKbs_LineA_init();
  pinKbs_LineB_init();
  pinKbs_LineC_init();
  pinKbs_LineD_init();
  pinKbs_LineA = 0;
  pinKbs_LineB = 0;
  pinKbs_LineC = 0;
  pinKbs_LineD = 0;
  pinKbs_DMX = 0;  // enable line demuxer
  pinKbs_Key0_init();
  pinKbs_Key1_init();
  pinKbs_Key2_init();
  pinKbs_Key3_init();
  pinKbs_Key4_init();
  pinKbs_Key5_init();
  pinKbs_Key6_init();
  pinKbs_Key7_init();
}

//
// --------------- Supervisor pins ----------------
//
#define pinSup_OUT_init() GPIO_DIR_OUT(2, 15); SFSP(5, 6) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 0
#define pinSup_OUTb           GPIO_Bit(2, 15)
#define pinSup_IN_init()   GPIO_DIR_IN(2,  7); SFSP(5, 7) = SFS_EIF + SFS_EIB + SFS_DHS + SFS_EPD + SFS_EPU + 0
#define pinSup_INb            GPIO_Bit(2,  7)

static inline void supPinsInit(void)
{
  pinSup_OUT_init();
  pinSup_IN_init();
}

// ------- Init all the pins -------
// since M4 starts first, it's best to init the pins needed by M0 also right there
static inline void PINS_Init(void)
{
  debugPinsInit();
  espiPinsInit();
  kbsPinsInit();
  bspiPinsInit();
  supPinsInit();
}

// clang-format on
