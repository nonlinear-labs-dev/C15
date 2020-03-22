#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void IOerror(int ret)
{
  puts("\nI/O error (driver). Terminating.");
  exit(ret);
}

// ===================
void writeData(FILE *const output, uint16_t const len, uint16_t *data)
{
  if (len == 0)
    return;

  for (uint16_t i = 0; i < len / 2; i++)
  {
    if (fputc(*data & 0xFF, output) == EOF)
      IOerror(3);
    if (fputc((*data++ & 0xFF00) >> 8, output) == EOF)
      IOerror(3);
  }
}

// ===================
void Usage(void)
{
  puts("Usage:");
  puts(" ehc  <command>");
  puts("  commands:");
  puts("  get             : sent \"fetch data request\"");
  puts("                  (display with read-lpc-msgs in another shell");
  puts("  clear-all       : reset&clear all EHCs");
  puts("  enable          : enable EHC processing");
  puts("  disable         : disable EHC processing");
  puts("  range <id>, <min>, <max>  : set range of ehc id to min..max (uint16)");
  puts("  config <params> : configure an EHC");
  puts("    params : <id> <port> [<flag>, ...]");
  puts("      id    : logical ID (1...8)");
  puts("      port  : physical port (p1t, p1r, p2t, ..., p4r)");
  puts("      flags :");
  puts("         sil   : silent (no msgs to ePC)");
  puts("         pot   : is a 3-wire pot");
  puts("         pul   : activate pullup");
  puts("         inv   : invert output");
  puts("         con   : continuous output (vs bi-stable)");
  puts("         are   : auto-ranging enable");
  puts("         ahs n : auto-hold strength 0...4 (default=2)");
  exit(3);
}

// ===================
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

// ===================
#define GET      "get"
#define CLEARALL "clear-all"
#define ENABLE   "enable"
#define DISABLE  "disable"
#define RANGE    "range"
#define CONFIG   "config"

uint16_t GET_DATA[]      = { 0x0A00, 0x0001, 0x0002 };
uint16_t CLEARALL_DATA[] = {
  // clang-format off
  0x0F00, 0x0002, 0x0100, 0xF800,
  0x0F00, 0x0002, 0x0100, 0xF900,
  0x0F00, 0x0002, 0x0100, 0xFA00,
  0x0F00, 0x0002, 0x0100, 0xFB00,
  0x0F00, 0x0002, 0x0100, 0xFC00,
  0x0F00, 0x0002, 0x0100, 0xFD00,
  0x0F00, 0x0002, 0x0100, 0xFE00,
  0x0F00, 0x0002, 0x0100, 0xFF00,
  // clang-format on
};
uint16_t ENDIS_DATA[]  = { 0x0700, 0x0002, 0xFF04, 0x0000 };
uint16_t RANGE_DATA[]  = { 0x0F00, 0x0002, 0x0000, 0x0000 };
uint16_t CONFIG_DATA[] = { 0x0F00, 0x0002, 0x0100, 0x0000 };

#define DRIVER "/dev/lpc_bb_driver"
//#define DRIVER "test.bin"
// ===================
int main(int argc, char const *argv[])
{
  FILE *driver;

  if (argc == 1)
    Usage();

  driver = fopen(DRIVER, "r+");
  if (!driver)
    IOerror(3);

  // get
  if (strncmp(argv[1], GET, sizeof GET) == 0)
    writeData(driver, sizeof GET_DATA, &GET_DATA[0]);

  // clear-all
  else if (strncmp(argv[1], CLEARALL, sizeof CLEARALL) == 0)
    writeData(driver, sizeof CLEARALL_DATA, &CLEARALL_DATA[0]);

  //enable
  else if (strncmp(argv[1], ENABLE, sizeof ENABLE) == 0)
  {
    ENDIS_DATA[3] = 0x0001;
    writeData(driver, sizeof ENDIS_DATA, &ENDIS_DATA[0]);
  }

  // disable
  else if (strncmp(argv[1], DISABLE, sizeof DISABLE) == 0)
  {
    ENDIS_DATA[3] = 0x0000;
    writeData(driver, sizeof ENDIS_DATA, &ENDIS_DATA[0]);
  }

  // range
  else if (strncmp(argv[1], RANGE, sizeof RANGE) == 0)
  {
    if (argc != 5)
    {
      puts("range: too few arguments!");
      Usage();
    }
    RANGE_DATA[2] = 0x0000;
    if (sscanf(argv[2], "%hu", &RANGE_DATA[2]) != 1)
    {
      puts("range: argument error (uint16 expected)");
      Usage();
    }
    if (RANGE_DATA[2] == 0 || RANGE_DATA[2] > 8)
    {
      puts("range: EHC id must be 1...8");
      Usage();
    }
    RANGE_DATA[2]--;
    RANGE_DATA[2] |= 0x0200;
    if (sscanf(argv[3], "%hu", &RANGE_DATA[3]) != 1)
    {
      puts("range: argument error (uint16 expected)");
      Usage();
    }
    writeData(driver, sizeof RANGE_DATA, &RANGE_DATA[0]);
    RANGE_DATA[2] &= 0x00FF;
    RANGE_DATA[2] |= 0x0300;
    if (sscanf(argv[4], "%hu", &RANGE_DATA[3]) != 1)
    {
      puts("range: argument error (uint16 expected)");
      Usage();
    }
    writeData(driver, sizeof RANGE_DATA, &RANGE_DATA[0]);
  }

  // config
  else if (strncmp(argv[1], CONFIG, sizeof CONFIG) == 0)
  {
    if (argc < 4)
    {
      puts("config: too few arguments!");
      Usage();
    }
    uint16_t id;
    if (sscanf(argv[2], "%hu", &id) != 1)
    {
      puts("config: argument error (uint16 expected)");
      Usage();
    }
    if (id == 0 || id > 8)
    {
      puts("config: EHC id must be 1...8");
      Usage();
    }
    id--;
    if (id > 3)
      id -= 4;
    EHC_ControllerConfig_T config = uint16ToConfig(0);
    config.hwId                   = id;
    if (strncmp(argv[3], "p1t", 3) == 0)
      config.ctrlId = 0;
    else if (strncmp(argv[3], "p1r", 3) == 0)
      config.ctrlId = 1;
    else if (strncmp(argv[3], "p2t", 3) == 0)
      config.ctrlId = 2;
    else if (strncmp(argv[3], "p2r", 3) == 0)
      config.ctrlId = 3;
    else if (strncmp(argv[3], "p3t", 3) == 0)
      config.ctrlId = 4;
    else if (strncmp(argv[3], "p3r", 3) == 0)
      config.ctrlId = 5;
    else if (strncmp(argv[3], "p4t", 3) == 0)
      config.ctrlId = 6;
    else if (strncmp(argv[3], "p4r", 3) == 0)
      config.ctrlId = 7;
    else
    {
      puts("config: invalid port");
      Usage();
    }
    argc -= 4;
    argv += 4;
    uint16_t ahs = 2;
    while (argc > 0)
    {
      if (strncmp(argv[0], "sil", 3) == 0)
        config.silent = 1;
      else if (strncmp(argv[0], "pot", 3) == 0)
        config.is3wire = 1;
      else if (strncmp(argv[0], "pul", 3) == 0)
        config.pullup = 1;
      else if (strncmp(argv[0], "inv", 3) == 0)
        config.polarityInvert = 1;
      else if (strncmp(argv[0], "con", 3) == 0)
        config.continuous = 1;
      else if (strncmp(argv[0], "are", 3) == 0)
        config.doAutoRanging = 1;
      else if (strncmp(argv[0], "ahs", 3) == 0)
      {
        if (argc < 2)
        {
          puts("config: ahs flag needs an argument");
          Usage();
        }
        if (sscanf(argv[1], "%hu", &ahs) != 1)
        {
          puts("config: ahs flag needs a numeric argument");
          Usage();
        }
        if (ahs > 4)
        {
          puts("config: ahs flag argument must be 0...4");
          Usage();
        }
        argv++;
        argc--;
      }
      else
      {
        printf("config: illegal flag: %s\n", argv[0]);
        Usage();
      }
      argv++;
      argc--;
    }
    if (config.continuous)
      config.autoHoldStrength = ahs;
    CONFIG_DATA[3] = configToUint16(config);
    writeData(driver, sizeof CONFIG_DATA, &CONFIG_DATA[0]);
  }

  // unknown
  else
  {
    puts("unknown command");
    Usage();
  }
  fclose(driver);
  return 0;
}
