#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#pragma GCC diagnostic ignored "-Wunused-function"

#include "shared/lpc-defs.h"
#include "shared/lpc-converters.h"

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
  puts("  clear-all        : clear all controllers");
  puts("  enable           : enable EHC processing");
  puts("  disable          : disable EHC processing");
  puts("  range <port> <min> <max>  : set range of controller to min..max (uint16)");
  puts("  deadzone <port> <low> <high>  : set dead-zones of controller to low, high % (<= 20)");
  puts("  reset <port>     : reset controller at port");
  puts("  force <port>     : force output update from controller at port");
  puts("  clear <port>|all : clear controller at port");
  puts("  config <params>  : configure an EHC");
  puts("    params : <port> <id> [<flag>, ...]");
  puts("      port  : physical port name (p1t, p1r, p2t, ..., p4r),");
  puts("              or equivalent controller ID 0..7");
  puts("      id    : HWSID, logical EHC ID ([e]1...[e]8), or one of");
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
#define GET      "get"
#define CLEARALL "clear-all"
#define CLEAR    "clear"
#define ENABLE   "enable"
#define DISABLE  "disable"
#define RANGE    "range"
#define DEADZONE "deadzone"
#define RESET    "reset"
#define FORCE    "force"
#define CONFIG   "config"

uint16_t GET_DATA[]      = { 0x0A00, 0x0001, 0x0002 };
uint16_t CLEARALL_DATA[] = {
  // clang-format off
  LPC_BB_MSG_TYPE_EHC_CONFIG, 0x0002, LPC_EHC_COMMAND_SET_CONTROL_REGISTER, 0xF800,
  LPC_BB_MSG_TYPE_EHC_CONFIG, 0x0002, LPC_EHC_COMMAND_SET_CONTROL_REGISTER, 0xF900,
  LPC_BB_MSG_TYPE_EHC_CONFIG, 0x0002, LPC_EHC_COMMAND_SET_CONTROL_REGISTER, 0xFA00,
  LPC_BB_MSG_TYPE_EHC_CONFIG, 0x0002, LPC_EHC_COMMAND_SET_CONTROL_REGISTER, 0xFB00,
  LPC_BB_MSG_TYPE_EHC_CONFIG, 0x0002, LPC_EHC_COMMAND_SET_CONTROL_REGISTER, 0xFC00,
  LPC_BB_MSG_TYPE_EHC_CONFIG, 0x0002, LPC_EHC_COMMAND_SET_CONTROL_REGISTER, 0xFD00,
  LPC_BB_MSG_TYPE_EHC_CONFIG, 0x0002, LPC_EHC_COMMAND_SET_CONTROL_REGISTER, 0xFE00,
  LPC_BB_MSG_TYPE_EHC_CONFIG, 0x0002, LPC_EHC_COMMAND_SET_CONTROL_REGISTER, 0xFF00,
  // clang-format on
};
uint16_t ENDIS_DATA[] = { LPC_BB_MSG_TYPE_SETTING, 0x0002, LPC_SETTING_ID_ENABLE_EHC, 0x0000 };
uint16_t CMD_DATA[]   = { LPC_BB_MSG_TYPE_EHC_CONFIG, 0x0002, 0x0000, 0x0000 };

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
    puts("config: argument error (controller id expected)");
    Usage();
  }
  if (id > 7)
  {
    puts("config: controller id must be 0...7");
    Usage();
  }
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
    puts("config: argument error (EHC # expected)");
    Usage();
  }
  if (id == 0 || id > 8)
  {
    puts("config: HWSID as EHC # must be 1...8");
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
    CMD_DATA[2] = readPortID(argv[2]);
    CMD_DATA[2] |= LPC_EHC_COMMAND_SET_RANGE_MIN;
    if (sscanf(argv[3], "%hu", &CMD_DATA[3]) != 1)
    {
      puts("range: argument error (uint16 expected)");
      Usage();
    }
    writeData(driver, sizeof CMD_DATA, &CMD_DATA[0]);
    CMD_DATA[2] &= 0x00FF;
    CMD_DATA[2] |= LPC_EHC_COMMAND_SET_RANGE_MAX;
    if (sscanf(argv[4], "%hu", &CMD_DATA[3]) != 1)
    {
      puts("range: argument error (uint16 expected)");
      Usage();
    }
    writeData(driver, sizeof CMD_DATA, &CMD_DATA[0]);
    return 0;
  }

  // dead zones
  if (strncmp(argv[1], DEADZONE, sizeof DEADZONE) == 0)
  {
    if (argc != 5)
    {
      puts("deadzone: too few arguments!");
      Usage();
    }
    CMD_DATA[2] = readPortID(argv[2]);
    CMD_DATA[2] |= LPC_EHC_COMMAND_SET_DEAD_ZONES;
    uint16_t low = 0, high = 0;
    if (sscanf(argv[3], "%hu", &low) != 1 || low > 20)
    {
      puts("deadzone: argument error (uint16 and <= 20 expected)");
      Usage();
    }
    if (sscanf(argv[4], "%hu", &high) != 1 || high > 20)
    {
      puts("deadzone: argument error (uint16 and <= 20 expected)");
      Usage();
    }
    CMD_DATA[3] = ((uint16_t) high << 8) + low;
    writeData(driver, sizeof CMD_DATA, &CMD_DATA[0]);
    return 0;
  }

  // reset
  if (strncmp(argv[1], RESET, sizeof RESET) == 0)
  {
    if (argc != 3)
    {
      puts("reset: too few arguments!");
      Usage();
    }
    CMD_DATA[2] = readPortID(argv[2]);
    CMD_DATA[2] |= LPC_EHC_COMMAND_RESET_DELETE;
    CMD_DATA[3] = 0;
    writeData(driver, sizeof CMD_DATA, &CMD_DATA[0]);
    return 0;
  }

  // clear
  if (strncmp(argv[1], CLEAR, sizeof CLEAR) == 0)
  {
    if (argc != 3)
    {
      puts("clear: too few arguments!");
      Usage();
    }
    if (strncmp(argv[2], "all", 3) == 0)
      goto ClearAll;
    else
    {
      CMD_DATA[2] = readPortID(argv[2]);
      CMD_DATA[2] |= LPC_EHC_COMMAND_RESET_DELETE;
      CMD_DATA[3] = 1;
      writeData(driver, sizeof CMD_DATA, &CMD_DATA[0]);
      return 0;
    }
  }

  // force
  if (strncmp(argv[1], FORCE, sizeof FORCE) == 0)
  {
    if (argc != 3)
    {
      puts("force: too few arguments!");
      Usage();
    }
    CMD_DATA[2] = readPortID(argv[2]);
    CMD_DATA[2] |= LPC_EHC_COMMAND_FORCE_OUTPUT;
    CMD_DATA[3] = 0;
    writeData(driver, sizeof CMD_DATA, &CMD_DATA[0]);
    return 0;
  }

  // config
  if (strncmp(argv[1], CONFIG, sizeof CONFIG) == 0)
  {
    if (argc < 4)
    {
      puts("config: too few arguments!");
      Usage();
    }
    EHC_ControllerConfig_T config = EHC_uint16ToConfig(0);
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
    CMD_DATA[2] = 0x0100;
    CMD_DATA[3] = EHC_configToUint16(config);
    writeData(driver, sizeof CMD_DATA, &CMD_DATA[0]);
    return 0;
  }

  // unknown
  puts("unknown command");
  Usage();
  return 0;
}
