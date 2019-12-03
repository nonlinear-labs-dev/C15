// BBB communication
// KSTR 2019-11-04

#include "comm.h"

#include "bit_manipulation.h"
#include "pin_manipulation.h"
#include "hardware.h"
#include <avr/interrupt.h>

static uint8_t enable = 0;

#define UBRR_300  (416)  // 2000000/(16ul*BITS_PER_SECOND) -1   , for 300bps
#define UBRR_1200 (103)  // 2000000/(16ul*BITS_PER_SECOND) -1   , for 1200bps
#define UBRR_9600 (12)   // 2000000/(16ul*BITS_PER_SECOND) -1   , for 9600bps

#define UBRR (UBRR_1200)

static uint8_t data[3];

void COMM_Init(void)
{
  data[0] = data[1] = data[2] = 0;
  cli();
  UBRRH = (uint8_t)(UBRR >> 8);
  UBRRL = (uint8_t)(UBRR);
  UCSRB = (1 << RXEN) | (1 << TXEN);
  UCSRC = (1 << URSEL) | (0 << UMSEL) | (3 << UPM0) | (1 << USBS) | (3 << UCSZ0);  // no parity, 1 stop bits, 8 data bits
  sei();
  enable = 1;
}

void COMM_DeInit(void)
{
  enable = 0;
  UCSRB  = 0;
}

static uint8_t step = 0;

void COMM_StartStatusWrite(void)
{
  if (step == 0)
    step = 1;
}

void COMM_ProccessReadCommands(void)
{
  if (!enable)
    return;

  if (BitGet(UCSRA, RXC))  // receive register full ?
  {
    uint8_t ok = !BitGet(UCSRA, FE) && !BitGet(UCSRA, DOR) && !BitGet(UCSRA, UPE);
    data[2]    = data[1];
    data[1]    = data[0];
    data[0]    = UDR;
    if (!ok)
    {
      data[0] = data[1] = data[2] = 0;
    }
    else if (data[0] == 'c' && data[2] == 'c')
      switch (data[1])
      {
        case 'L':
          if (config.GetRunState() == RS_RUNNING || config.GetRunState() == RS_BOOTING)
            BitSet(config.status, STAT_SYSTEM_LOCKED);
          break;
        case 'l':
          BitClr(config.status, STAT_SYSTEM_LOCKED);
          break;

        case 'O':
          BitSet(config.status, STAT_MUTING_OVERRIDE_ENABLE);
          break;
        case 'o':
          BitClr(config.status, STAT_MUTING_OVERRIDE_ENABLE);
          break;

        case 'U':
          BitSet(config.status, STAT_MUTING_OVERRIDE_VALUE);
          break;
        case 'u':
          BitClr(config.status, STAT_MUTING_OVERRIDE_VALUE);
          break;
      }
  }
}

void COMM_ProccessWriteStatus(void)
{
  if (!enable)
    return;
  if (BitGet(UCSRA, UDRE))  // send register empty ?
  {
    // LED_B(-1);
    switch (step)
    {
      case 0:
        break;  // wait for someone start the chain
      case 1:
        UDR = 0x80 + config.status;
        step++;
        break;
      case 2:
        UDR = config.hardware_id;
        step++;
        break;
      case 3:
        UDR  = config.firmware_id;
        step = 0;
        break;
    }
  }
}

// end of file
