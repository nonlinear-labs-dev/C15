#include "process-read-msgs.h"
#include "shared/lpc-defs.h"
#include "shared/lpc-converters.h"

char paramNameTable[][NUM_HW_SOURCES] = {
  "EHC 1     ",
  "EHC 2     ",
  "EHC 3     ",
  "EHC 4     ",
  "BENDER    ",
  "AFTERTOUCH",
  "RIBBON 1  ",
  "RIBBON 2  ",
  "EHC 5     ",
  "EHC 6     ",
  "EHC 7     ",
  "EHC 8     ",
  "LASTKEY",
};
char paramShortNameTable[][16] = {
  "E1",
  "E2",
  "E3",
  "E4",
  "PB",
  "AT",
  "R1",
  "R2",
  "E5",
  "E6",
  "E7",
  "E8",
  "--",
  "--",
  "--",
  "--",
};

// ==================================================================================

typedef enum
{
  RED,
  GREEN,
  DEFAULT
} color_t;

void setColor(color_t color)
{
  switch (color)
  {
    case RED:
      printf("\033[0;31m");
      break;
    case GREEN:
      printf("\033[0;32m");
      break;
    case DEFAULT:
      printf("\033[0m");
      break;
  }
}

int greenNotRed(int condition)
{
  setColor(condition ? GREEN : RED);
  return condition;
}

void cursorUp(uint8_t lines)
{
  char buffer[12];
  sprintf(buffer, "\033[%dA", lines);
  printf(buffer);
}

// ==================================================================================
void dump(uint16_t const cmd, uint16_t const len, uint16_t *const data, uint16_t const flags)
{
  if (flags & NO_HEXDUMP)
    return;
  printf("hexdump cmd=%04X len=%04X: data=", cmd, len);
  for (int i = 0; i < len; i++)
    printf("%04X ", data[i]);
  printf("\n");
}

// ==================================================================================, uint16_t const flags

void printfResistance(double kOhms)
{
  kOhms *= 10;
  if (kOhms > 99.9)
    printf("%5.1lfk", kOhms);
  else if (kOhms > 0.999)
    printf("%5.2lfk", kOhms);
  else
    printf("%5.0lfR", kOhms * 1000);
}

void printfPercent(double factor)
{
  factor *= 100;
  if (factor > 999.9)
    printf("%5.0lf%%", factor);  // integer value
  else if (factor > 9.99)
    printf("%5.1lf%%", factor);  // one fractional digit
  else
    printf("%5.2lf%%", factor);  // two fractional digits
}

void printfControlVoltage(double const factor)
{
  printf("%5.3lfV", factor * 5.0);
}

void printfDeadZones(uint16_t const deadZones)
{
  printf("%2d %2d", deadZones & 0xFF, deadZones >> 8);
}

void displayCounter(void)
{
  static uint32_t cntr = 0;
  printf("%010u ", cntr++);
}

// ==================================================================================, uint16_t const flags
void processReadMsgs(uint16_t const cmd, uint16_t const len, uint16_t *const data, uint16_t flags)
{
  int       i;
  uint16_t *p;
  double    last[8], intermediate[8], min[8], max[8], scale[8];
  uint16_t  adc[8], deadzone[8];
  int       same;

  EHC_ControllerConfig_T config[8];
  EHC_ControllerStatus_T status[8];

  static uint32_t lastMessage = ~0;
  static double   makeTime    = 0;

  switch (cmd)
  {
    case LPC_BB_MSG_TYPE_KEY_EMUL:
      if (flags & NO_KEY_LOG)
        return;
      dump(cmd, len, data, flags);
      if (len != 3)
      {
        printf("PARAM : wrong length of %d\n", len);
        return;
      }
      if ((same = !(flags & NO_OVERLAY) && (lastMessage == (((uint32_t) cmd << 16)) + (uint32_t) data[0])))
        cursorUp(1);
      displayCounter();
      double time = (data[1] | (int) data[2] << 16) / 1000.0;
      if (time > 0)
      {
        makeTime = time;
        printf("KEY = %3d, %8.2lfms            ", data[0], time);
      }
      else
      {
        if (same)
          printf("KEY = %3d, %8.2lfms  %8.2lfms", data[0], makeTime, time);
        else
          printf("KEY = %3d,             %8.2lfms", data[0], time);
      }
      if (data[0] <= HW_SOURCE_ID_PEDAL_8)
        printf(" (%5.1lf%%)", 100.0 * data[1] / 16000.0);
      printf("\n");
      lastMessage = (cmd << 16) + data[0];
      return;

    case LPC_BB_MSG_TYPE_PARAMETER:
      if (flags & NO_PARAMS)
        return;
      dump(cmd, len, data, flags);
      if (len != 2)
      {
        printf("PARAM : wrong length of %d\n", len);
        return;
      }
      if (data[0] > NUM_HW_SOURCES)
        return;
      if (!(flags & NO_OVERLAY) && (lastMessage == (((uint32_t) cmd << 16)) + (uint32_t) data[0]))
        cursorUp(1);
      displayCounter();
      if (data[0] <= HW_SOURCE_ID_PEDAL_8)
        printf("PARAM (HWSID %02d) %s = %5d (%5.1lf%%)", data[0], paramNameTable[data[0]], data[1], 100.0 * data[1] / 16000.0);
      else  // key
        printf("PARAM (HWSID %02d) %s : key number=%3d, note number=%3d", data[0], paramNameTable[data[0]], data[1] >> 8, data[1] & 0xFF);
      printf("\n");
      lastMessage = (cmd << 16) + data[0];
      return;

    case LPC_BB_MSG_TYPE_NOTIFICATION:
      if (flags & NO_NOTIFICATION)
        return;
      dump(cmd, len, data, flags);
      if (len != 2)
      {
        printf("NOTIFICATION : wrong length of %d\n", len);
        return;
      }
      if (!(flags & NO_OVERLAY) && (lastMessage == (((uint32_t) cmd << 16)) + (uint32_t) data[0]))
        cursorUp(1);
      displayCounter();
      switch (data[0])
      {
        case LPC_NOTIFICATION_ID_CLEAR_EEPROM:
          printf("NOTIFICATION : EEPROM cleared\n");
          break;
        case LPC_NOTIFICATION_ID_STAT_DATA:
          printf("NOTIFICATION : Status Data sent\n");
          break;
        case LPC_NOTIFICATION_ID_SW_VERSION:
          printf("NOTIFICATION : Software Version: %hu\n", data[1]);
          break;
        case LPC_NOTIFICATION_ID_EHC_DATA:
          printf("NOTIFICATION : EHC data sent: %s\n", data[1] ? "success  " : "failed  ");
          break;
        case LPC_NOTIFICATION_ID_EHC_EEPROMSAVE:
          printf("NOTIFICATION : EHC data save to EEPROM: %s\n", data[1] ? "started successfully" : "postponed/failed   ");
          break;
        case LPC_NOTIFICATION_ID_UNMUTE_STATUS:
          printf("NOTIFICATION : ");
          data[0] = data[1];
          goto ShowMuteStatus;
        default:
          printf("NOTIFICATION : unknown ID=%d, data=%d     \n", data[0], data[1]);
          break;
      }
      lastMessage = (cmd << 16) + data[0];
      return;

    case LPC_BB_MSG_TYPE_STAT_DATA:
      if (flags & NO_STATDATA)
        return;
      dump(cmd, len, data, flags);
      if (len != 10)
      {
        printf("STATUS : wrong length of %d\n", len);
        return;
      }
      if (!(flags & NO_OVERLAY) && (lastMessage == ((uint32_t) cmd << 16)))
        cursorUp(10);
      displayCounter();
      printf("SYSTEM STATUS:\n");
      printf("  M4 ticker      : %10u\n", (unsigned) data[0] + ((unsigned) data[1] << 16));
      printf("  Scheduler      : %5d task overruns\n", data[2]);
      printf("  Scheduler      : %5d tasks max. per time-slice\n", data[3]);
      printf("  Scheduler      : %5dus max. task runtime\n", data[4]);
      printf("  Scheduler      : %5dus max. time-slice\n", data[5]);
      printf("  BBB Msg        : %5d buffer overruns / ESPI send fails\n", data[6]);
      printf("  TCD Msg        : %5d buffer overruns / USB send fails\n", data[7]);
      printf("  M0 ADC Scanner : %5dus max. round trip time\n", data[8]);
      if (data[8] != 0xFFFF)
        printf("  M0 Key Scanner : %5d (overrun flag)\n", data[9]);
      else
        printf("  M0 Key Scanner : -n/a- (overrun flag)\n");
      lastMessage = cmd << 16;
      return;

    case LPC_BB_MSG_TYPE_HEARTBEAT:
      if (flags & NO_HEARTBEAT)
        return;
      dump(cmd, len, data, flags);
      if (len != 4)
      {
        printf("HEARTBEAT : wrong length of %d\n", len);
        return;
      }
      uint64_t heartbeat = 0;
      for (int i = 3; i >= 0; i--)
      {
        heartbeat <<= 16;
        heartbeat |= data[i];
      }
      if (!(flags & NO_OVERLAY) && (lastMessage == ((uint32_t) cmd << 16)))
        cursorUp(1);
      displayCounter();
      printf("HEARTBEAT : %8llu\n", heartbeat);
      lastMessage = cmd << 16;
      return;

    case LPC_BB_MSG_TYPE_SENSORS_RAW:
      if (flags & NO_SENSORSRAW)
        return;
      dump(cmd, len, data, flags);
      if (len != 13)
      {
        printf("RAW SENSORS : wrong length of %d\n", len);
        return;
      }
      if (!(flags & NO_OVERLAY) && (lastMessage == ((uint32_t) cmd << 16)))
        cursorUp(1);
      displayCounter();
      printf("RAW SENSORS: ");
      for (int i = 3; i >= 0; i--)
        printf("%c ", (data[0] & (1 << i)) ? '1' : '0');
      printf(" ");
      for (int i = 1; i < 13; i++)
      {
        printf("%04hu ", data[i]);
        if (i == 8 || i == 10)
          printf("/ ");
      }
      printf("\n");
      lastMessage = cmd << 16;
      return;

    case LPC_BB_MSG_TYPE_MUTESTATUS:
      if (flags & NO_MUTESTATUS)
        return;
      dump(cmd, len, data, flags);
      if (len != 1)
      {
        printf("MUTESTATUS : wrong length of %d\n", len);
        return;
      }
      if (!(flags & NO_OVERLAY) && (lastMessage == ((uint32_t) cmd << 16)))
        cursorUp(1);
      displayCounter();
    ShowMuteStatus:
      printf("MUTESTATUS: valid:%d", data[0] & SUP_UNMUTE_STATUS_IS_VALID ? 1 : 0);
      printf(" jumper:");
      if (data[0] & SUP_UNMUTE_STATUS_JUMPER_OVERRIDE)
        printf("%d", data[0] & SUP_UNMUTE_STATUS_JUMPER_VALUE ? 1 : 0);
      else
        printf("-");
      printf(" software:");
      if (data[0] & SUP_UNMUTE_STATUS_SOFTWARE_OVERRIDE)
        printf("%d", data[0] & SUP_UNMUTE_STATUS_SOFTWARE_VALUE ? 1 : 0);
      else
        printf("-");
      printf(" midi:");
      if (data[0] & SUP_UNMUTE_STATUS_MIDI_DERIVED)
        printf("%d", data[0] & SUP_UNMUTE_STATUS_MIDI_DERIVED_VALUE ? 1 : 0);
      else
        printf("-");
      printf(" physical:");
      if (data[0] & SUP_UNMUTE_STATUS_HARDWARE_IS_VALID)
        printf("%d", data[0] & SUP_UNMUTE_STATUS_HARDWARE_VALUE ? 1 : 0);
      else
        printf("-");
      printf("\n");
      lastMessage = cmd << 16;
      return;

    case LPC_BB_MSG_TYPE_EHC_DATA:
      if (flags & NO_EHCDATA)
        return;
      dump(cmd, len, data, flags);
      if (len != 8 * 9)
      {
        printf("EHC DATA : wrong length of %d\n", len);
        return;
      }
      p = (uint16_t *) data;
      for (i = 0; i < 8; i++)
      {
        config[i]       = EHC_uint16ToConfig(*p++);
        status[i]       = EHC_uint16ToStatus(*p++);
        last[i]         = *p++;
        intermediate[i] = *p++;
        min[i]          = *p++;
        max[i]          = *p++;
        scale[i]        = *p++;
        adc[i]          = *p++;
        deadzone[i]     = *p++;
      }

      if (!(flags & NO_OVERLAY) && (lastMessage == ((uint32_t) cmd << 16)))
        cursorUp(20);
      displayCounter();
      printf("EHC data -------------------------------------------------\n");
      printf("Controller    ");
      for (i = 0; i < 8; i++)
      {
        greenNotRed(status[i].initialized);
        printf("P%d%c(%d)%c", i / 2 + 1, i & 1 ? 'R' : 'T', i, i == 7 ? '\n' : ' ');
      }
      setColor(DEFAULT);

      printf("ADC           ");
      for (i = 0; i < 8; i++)
      {
        printf("%5d %c", adc[i], i == 7 ? '\n' : ' ');
      }
      setColor(DEFAULT);

      // config
      printf("Config        ");
      for (i = 0; i < 8; i++)
      {
        greenNotRed(status[i].initialized);
        printf("%04Xh %c", EHC_configToUint16(config[i]), i == 7 ? '\n' : ' ');
      }
      setColor(DEFAULT);

      printf("  HWSID/SIL   ");
      for (i = 0; i < 8; i++)
      {
        greenNotRed(status[i].initialized);
        printf("%s/%c  %c", paramShortNameTable[config[i].hwId], config[i].silent ? 'S' : '-', i == 7 ? '\n' : ' ');
      }
      setColor(DEFAULT);

      printf("  Pot/pUl/Con ");
      for (i = 0; i < 8; i++)
      {
        greenNotRed(status[i].initialized);
        printf("%c/%c/%c %c", config[i].is3wire ? 'P' : '-', config[i].pullup ? 'U' : '-', config[i].continuous ? 'C' : '-', i == 7 ? '\n' : ' ');
      }
      setColor(DEFAULT);

      printf("  Inv/aRe/AHS ");
      for (i = 0; i < 8; i++)
      {
        greenNotRed(status[i].initialized);
        printf("%c/%c/%d %c", config[i].polarityInvert ? 'I' : '-', config[i].doAutoRanging ? 'R' : '-', config[i].autoHoldStrength, i == 7 ? '\n' : ' ');
      }
      setColor(DEFAULT);

      // status
      printf("Status        ");
      for (i = 0; i < 8; i++)
      {
        greenNotRed(status[i].initialized);
        printf("%04Xh %c", EHC_statusToUint16(status[i]), i == 7 ? '\n' : ' ');
      }
      setColor(DEFAULT);

      printf("  Ini/Plg/rEs ");
      for (i = 0; i < 8; i++)
      {
        greenNotRed(status[i].initialized);
        printf("%c/%c/%c %c", status[i].initialized ? 'I' : '-', status[i].pluggedIn ? 'P' : '-', status[i].isReset ? 'E' : '-', i == 7 ? '\n' : ' ');
      }
      setColor(DEFAULT);

      printf("  Rng/Val/Stl ");
      for (i = 0; i < 8; i++)
      {
        greenNotRed(status[i].initialized);
        printf("%c/%c/%c %c", status[i].isAutoRanged ? 'R' : '-', status[i].outputIsValid ? 'V' : '-', status[i].isSettled ? 'S' : '-', i == 7 ? '\n' : ' ');
      }
      setColor(DEFAULT);

      printf("  rAm/eeS/eeR ");
      for (i = 0; i < 8; i++)
      {
        greenNotRed(status[i].initialized);
        printf("%c/%c/%c %c", status[i].isRamping ? 'A' : '-', status[i].isSaved ? 'S' : '-', status[i].isRestored ? 'R' : '-', i == 7 ? '\n' : ' ');
      }
      setColor(DEFAULT);

      // last
      printf("Last          ");
      for (i = 0; i < 8; i++)
      {
        if (greenNotRed(config[i].hwId != 15 && status[i].initialized && status[i].pluggedIn && status[i].outputIsValid))
          printf("%5.0lf %c", last[i], i == 7 ? '\n' : ' ');
        else
          printf("      %c", i == 7 ? '\n' : ' ');
      }
      setColor(DEFAULT);

      // last %
      printf("Last %%        ");
      for (i = 0; i < 8; i++)
      {
        if (greenNotRed(config[i].hwId != 15 && status[i].initialized && status[i].pluggedIn && status[i].outputIsValid))
          printfPercent(last[i] / 16000);
        else
          printf("      ");
        printf("%c", i == 7 ? '\n' : ' ');
      }
      setColor(DEFAULT);

      // min
      printf("Min           ");
      for (i = 0; i < 8; i++)
      {
        if (greenNotRed(config[i].hwId != 15 && status[i].initialized))
        {
          if (!(status[i].isAutoRanged))
            setColor(DEFAULT);
          printf("%5.0lf %c", min[i], i == 7 ? '\n' : ' ');
        }
        else
          printf("      %c", i == 7 ? '\n' : ' ');
      }
      setColor(DEFAULT);

      // max
      printf("Max           ");
      for (i = 0; i < 8; i++)
      {
        if (greenNotRed(config[i].hwId != 15 && status[i].initialized))
        {
          if (!(status[i].isAutoRanged))
            setColor(DEFAULT);
          printf("%5.0lf %c", max[i], i == 7 ? '\n' : ' ');
        }
        else
          printf("      %c", i == 7 ? '\n' : ' ');
      }
      setColor(DEFAULT);

      // scale
      printf("Scale         ");
      for (i = 0; i < 8; i++)
      {
        if (greenNotRed(config[i].hwId != 15 && status[i].initialized))
        {
          printf("%5.0lf %c", scale[i], i == 7 ? '\n' : ' ');
        }
        else
          printf("      %c", i == 7 ? '\n' : ' ');
      }
      setColor(DEFAULT);

      // dead-zones
      printf("Dead zones(%%) ");
      for (i = 0; i < 8; i++)
      {
        if (greenNotRed(config[i].hwId != 15 && status[i].initialized && config[i].doAutoRanging))
        {
          if (!(status[i].isAutoRanged))
            setColor(DEFAULT);
          printfDeadZones(deadzone[i]);
        }
        else
          printf("      ");
        printf("%c", i == 7 ? '\n' : ' ');
      }
      setColor(DEFAULT);

      // min scaled
      printf("Min (scaled)  ");
      for (i = 0; i < 8; i++)
      {
        if (greenNotRed(config[i].hwId != 15 && status[i].initialized && min[i] < max[i]))
        {
          double tmp = min[i] / scale[i];
          if (!(status[i].isAutoRanged))
            setColor(DEFAULT);
          if (config[i].is3wire)  // pot, output is in %
            printfPercent(tmp);
          else if (!config[i].is3wire && config[i].pullup)  // rheostat, output is in 00.00k to 130.1k Ohms
            printfResistance(tmp);
          else if (!config[i].is3wire && !config[i].pullup)  // control voltage, output is 0.00V to 5.00V Volts
            printfControlVoltage(tmp);
          else
            printf("      ");
        }
        else
          printf("      ");
        printf("%c", i == 7 ? '\n' : ' ');
      }
      setColor(DEFAULT);

      // max scaled
      printf("Max (scaled)  ");
      for (i = 0; i < 8; i++)
      {
        if (greenNotRed(config[i].hwId != 15 && status[i].initialized && min[i] < max[i]))
        {
          double tmp = max[i] / scale[i];
          if (!(status[i].isAutoRanged))
            setColor(DEFAULT);
          if (config[i].is3wire)  // pot, output is in %
            printfPercent(tmp);
          else if (!config[i].is3wire && config[i].pullup)  // rheostat, output is in 00.00k to 130.1k Ohms
            printfResistance(tmp);
          else if (!config[i].is3wire && !config[i].pullup)  // control voltage, output is 0.00V to 5.00V Volts
            printfControlVoltage(tmp);
          else
            printf("      ");
        }
        else
          printf("      ");
        printf("%c", i == 7 ? '\n' : ' ');
      }
      setColor(DEFAULT);

      // intermediate scaled
      printf("Cur (scaled)  ");
      for (i = 0; i < 8; i++)
      {
        if (greenNotRed(config[i].hwId != 15 && status[i].initialized))
        {
          double tmp = intermediate[i] / scale[i];
          if (!(status[i].isAutoRanged))
            setColor(DEFAULT);
          if (config[i].is3wire)  // pot, output is in %
            printfPercent(tmp);
          else if (!config[i].is3wire && config[i].pullup)  // rheostat, output is in 00.00k to 130.1k Ohms
            printfResistance(tmp);
          else if (!config[i].is3wire && !config[i].pullup)  // control voltage, output is 0.00V to 5.00V Volts
            printfControlVoltage(tmp);
          else
            printf("      ");
        }
        else
          printf("      ");
        printf("%c", i == 7 ? '\n' : ' ');
      }
      setColor(DEFAULT);

      lastMessage = cmd << 16;
      return;

    default:
      if (flags & NO_UNKNOWN)
        return;
      printf("UNKNOWN ");
      flags &= ~NO_HEXDUMP;
      dump(cmd, len, data, flags);
      return;
  }
}
