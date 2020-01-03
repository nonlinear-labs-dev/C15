// initial ESPI override to set all LEDs on TopCover and PanelUnit
// KSTR 2019-06-21

#include "ESPI.h"

#include "globals.h"
#include "coos.h"
#include "hardware.h"
#include "bit_manipulation.h"
#include "pin_manipulation.h"

int8_t ESPI_Process_ID;

static uint8_t  step = 0;
static uint16_t cntr = 0;

void ESPI_StartOverride(void)
{
  if (config.hardware_id != 71)  // no override for other HW version than 7.1
    return;
  ESPI_Process_ID = coos_add_task(ESPI_Process, 100, -ESPI_TIMESLICE);  // ESPI control handler, start with 100ms delay
  step            = 0;
  cntr            = 0;
}

void ESPI_Process(void)
{
  switch (step)
  {
    case 0:  // enable override
      PinSet(
          SYS_OPT_ESPI_DISABLE);  // disable BBB's control over ESPI, tri-stating the bus and pre-set the chip-selects
      step++;
      return;
    case 1:  // start clocking 140 "ones" into the shift registers (count >= 33LEDs per ribbon X 2 States X 2 ribbons == 132)
      cntr = 140;
      step++;
      return;
    case 2:      // pulse SCK high
      if (cntr)  // bits left ?
      {
        PinSet(SYS_OPT_ESPI_SCK);
        step++;
      }
      else
        step = 10;
      return;
    case 3:        // pulse SCK low
      if (cntr--)  // bits left ?
      {
        PinClr(SYS_OPT_ESPI_SCK);
        step--;
      }
      else
        step = 10;
      return;
    case 10:  // pulse SAP high
      PinSet(SYS_OPT_ESPI_SAP);
      step++;
      return;

    case 11:  // pulse SAP and the DISABLE low, and kill this task
      PinClr(SYS_OPT_ESPI_SAP);
      coos_delete_task(ESPI_Process_ID);  // allow some time to pass.
      PinClr(SYS_OPT_ESPI_DISABLE);       // enable BBB's control over ESPI again
      return;
  }
}

// end of file
