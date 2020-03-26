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
  puts("  get              : sent \"fetch data request\"");
  puts("                   (display with read-lpc-msgs in another shell");
  puts("  clear-all        : clear all EHCs");
  puts("  enable           : enable EHC processing");
  puts("  disable          : disable EHC processing");
  puts("  range <port>, <min>, <max>  : set range of controller to min..max (uint16)");
  puts("  reset <port>     : reset controller at port");
  puts("  clear <port>|all : clear controller at port");
  puts("  config <params>  : configure an EHC");
  puts("    params : <port> <id> [<flag>, ...]");
  puts("      port  : physical port name (p1t, p1r, p2t, ..., p4r),");
  puts("              or equivalent controller ID 1..8");
  puts("      id    : HWSID, logical EHC ID ([ehc]1...[ehc]8), or one of");
  puts("              the built-in controllers (pb, at, r1, r2)");
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
  ret.ctrlId           = (c & 0b0000011100000000) >> 8;
  ret.silent           = (c & 0b0000100000000000) >> 11;
  ret.hwId             = (c & 0b1111000000000000) >> 12;
  return ret;
}

// ===================
#define GET      "get"
#define CLEARALL "clear-all"
#define CLEAR    "clear"
#define ENABLE   "enable"
#define DISABLE  "disable"
#define RANGE    "range"
#define RESET    "reset"
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
uint16_t RESET_DATA[]  = { 0x0F00, 0x0002, 0x0400, 0x0000 };

uint16_t readPortID(const char *string)
{
  if (strncmp(string, "p1t", 3) == 0)
    return 0;
  if (strncmp(string, "p1r", 3) == 0)
    return 1;
  if (strncmp(string, "p2t", 3) == 0)
    return 2;
  if (strncmp(string, "p2r", 3) == 0)
    return 3;
  if (strncmp(string, "p3t", 3) == 0)
    return 4;
  if (strncmp(string, "p3r", 3) == 0)
    return 5;
  if (strncmp(string, "p4t", 3) == 0)
    return 6;
  if (strncmp(string, "p4r", 3) == 0)
    return 7;
  uint16_t id;
  if (sscanf(string, "%hu", &id) != 1)
  {
    puts("config: argument error (port id expected)");
    Usage();
  }
  if (id == 0 || id > 8)
  {
    puts("config: port must be 1...8");
    Usage();
  }
  id--;
  return id;
}

uint16_t readHWSID(const char *string)
{
  if (strncmp(string, "e1", 2) == 0)
    return 0;
  if (strncmp(string, "e2", 2) == 0)
    return 1;
  if (strncmp(string, "e3", 2) == 0)
    return 2;
  if (strncmp(string, "e4", 2) == 0)
    return 3;
  if (strncmp(string, "e5", 2) == 0)
    return 8;
  if (strncmp(string, "e6", 2) == 0)
    return 9;
  if (strncmp(string, "e7", 2) == 0)
    return 10;
  if (strncmp(string, "e8", 2) == 0)
    return 11;
  if (strncmp(string, "pb", 2) == 0)
    return 4;
  if (strncmp(string, "at", 2) == 0)
    return 5;
  if (strncmp(string, "r1", 2) == 0)
    return 6;
  if (strncmp(string, "r2", 2) == 0)
    return 7;
  uint16_t id;
  if (sscanf(string, "%hu", &id) != 1)
  {
    puts("config: argument error (HWSID expected)");
    Usage();
  }
  if (id == 0 || id > 8)
  {
    puts("config: HWSID must be 1...8");
    Usage();
  }
  id--;
  if (id > 3)
    id -= 4;
  return id;
}

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
  {
    writeData(driver, sizeof GET_DATA, &GET_DATA[0]);
    return 0;
  }

  // clear-all
  if (strncmp(argv[1], CLEARALL, sizeof CLEARALL) == 0)
  {
  ClearAll:
    writeData(driver, sizeof CLEARALL_DATA, &CLEARALL_DATA[0]);
    return 0;
  }

  //enable
  if (strncmp(argv[1], ENABLE, sizeof ENABLE) == 0)
  {
    ENDIS_DATA[3] = 0x0001;
    writeData(driver, sizeof ENDIS_DATA, &ENDIS_DATA[0]);
    return 0;
  }

  // disable
  if (strncmp(argv[1], DISABLE, sizeof DISABLE) == 0)
  {
    ENDIS_DATA[3] = 0x0000;
    writeData(driver, sizeof ENDIS_DATA, &ENDIS_DATA[0]);
    return 0;
  }

  // range
  if (strncmp(argv[1], RANGE, sizeof RANGE) == 0)
  {
    if (argc != 5)
    {
      puts("range: too few arguments!");
      Usage();
    }
    RANGE_DATA[2] = readPortID(argv[2]);
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
    return 0;
  }

  // reset
  if (strncmp(argv[1], RESET, sizeof RESET) == 0)
  {
    if (argc != 3)
    {
      puts("range: too few arguments!");
      Usage();
    }
    RESET_DATA[2] = readPortID(argv[2]);
    RESET_DATA[2] |= 0x0400;
    RESET_DATA[3] = 0;
    writeData(driver, sizeof RESET_DATA, &RESET_DATA[0]);
    return 0;
  }

  // clear
  if (strncmp(argv[1], CLEAR, sizeof CLEAR) == 0)
  {
    if (argc != 3)
    {
      puts("range: too few arguments!");
      Usage();
    }
    if (strncmp(argv[2], "all", 3) == 0)
      goto ClearAll;
    else
    {
      RESET_DATA[2] = readPortID(argv[2]);
      RESET_DATA[2] |= 0x0400;
      RESET_DATA[3] = 1;
      writeData(driver, sizeof RESET_DATA, &RESET_DATA[0]);
      return 0;
    }
  }

  // config
  if (strncmp(argv[1], CONFIG, sizeof CONFIG) == 0)
  {
    if (argc < 4)
    {
      puts("config: too few arguments!");
      Usage();
    }
    EHC_ControllerConfig_T config = uint16ToConfig(0);
    config.ctrlId                 = readPortID(argv[2]);
    config.hwId                   = readHWSID(argv[3]);
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
    return 0;
  }

  // unknown
  puts("unknown command");
  Usage();
  return 0;
}
