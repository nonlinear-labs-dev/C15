#include <stdio.h>
#include <stdint.h>

typedef struct
{
  unsigned ctrlId : 3;            // controller number 0...7, aka input (main) ADC channel 0...7, 0/1=J1T/R, 2/3=J2T/R, etc,
  unsigned hwId : 4;              // hardware ID used for messages to AE and PG
  unsigned silent : 1;            // disable messaging to AudioEngine
  unsigned is3wire : 1;           // controller connection type, 0=2wire(rheo/sw/cv), 1=3wire(pot)
  unsigned pullup : 1;            // controller input sensing, 0=unloaded(pot/CV), 1=with pullup(rheo/sw)
  unsigned continuous : 1;        // controller output type, 0=continuous(all), 1=bi-stable(all)
  unsigned polarityInvert : 1;    // invert, or don't, the final output(all)
  unsigned autoHoldStrength : 3;  // controller auto-hold 0..7, 0(off)...4=autoHold-Strength for pot/rheo
  unsigned doAutoRanging : 1;     // enable auto-ranging, or assume static (but adjustable) thresholds/levels
} EHC_ControllerConfig_T;

uint16_t configToUint16(const EHC_ControllerConfig_T c)
{
  uint16_t ret = 0;
  ret |= c.autoHoldStrength << 0;
  ret |= c.continuous << 3;
  ret |= c.doAutoRanging << 4;
  ret |= c.polarityInvert << 5;
  ret |= c.pullup << 6;
  ret |= c.is3wire << 7;
  ret |= c.ctrlId << 8;
  ret |= c.silent << 11;
  ret |= c.hwId << 12;
  return ret;
}
EHC_ControllerConfig_T uint16ToConfig(const uint16_t c)
{
  EHC_ControllerConfig_T ret;
  ret.autoHoldStrength = (c & 0b0000000000000111) >> 0;
  ret.continuous       = (c & 0b0000000000001000) >> 3;
  ret.doAutoRanging    = (c & 0b0000000000010000) >> 4;
  ret.polarityInvert   = (c & 0b0000000000100000) >> 5;
  ret.pullup           = (c & 0b0000000001000000) >> 6;
  ret.is3wire          = (c & 0b0000000010000000) >> 7;
  ret.ctrlId           = (c & 0b0000111100000000) >> 8;
  ret.silent           = (c & 0b0001000000000000) >> 11;
  ret.hwId             = (c & 0b1111000000000000) >> 12;
  return ret;
}

int main(void)
{
  printf("generating EHC config register bits\n");
  EHC_ControllerConfig_T c;

  c.hwId             = 0;
  c.ctrlId           = 0;
  c.pullup           = 1;
  c.silent           = 0;
  c.is3wire          = 0;
  c.continuous       = 1;
  c.doAutoRanging    = 0;
  c.polarityInvert   = 0;
  c.autoHoldStrength = 2;

  const char fname[] = "EHC-P1T-C1-RHEO.lpcmsg";

  uint16_t bits = configToUint16(c);

  printf("bits (hex) : %04X\n\n", bits);

  FILE *out = fopen(fname, "wb");
  if (!out)
    return 3;
  uint16_t data[4];
  data[0] = 0x0F00;  // EHC Configuration message
  data[1] = 2;
  data[2] = 0x0100;  // EHC Command : Set config register
  data[3] = bits;

  if (fwrite(data, sizeof(uint16_t), 4, out) != 4)
    return 3;
  printf("EHC config file \"%s\" written\n", fname);
  return 0;
}
