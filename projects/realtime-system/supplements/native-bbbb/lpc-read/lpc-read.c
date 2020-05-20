#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <errno.h>

#include "process-read-msgs.h"

#define MAX_DATA_SIZE (1500)
uint16_t id;
uint16_t len;
uint16_t data[MAX_DATA_SIZE];
int      dataIndex;
uint16_t displayFlags;
FILE *   driver;

void Error(const char *msg)
{
  printf("\nError : %s \nAborting.\n", msg);
  exit(3);
}

// ===================
void makeDriverNonblocking(int const driverFileNo, int const flags)
{
  if (fcntl(driverFileNo, F_SETFL, flags | O_NONBLOCK) < 0)
    Error("fcntl: can't switch to non-blocking");
}

void makeDriverBlocking(int const driverFileNo, int const flags)
{
  if (fcntl(driverFileNo, F_SETFL, flags & (~O_NONBLOCK)) < 0)
    Error("fcntl: can't switch to blocking");
}

int getDriverFlags(int const driverFileNo)
{
  int flags;
  if ((flags = fcntl(driverFileNo, F_GETFL, 0)) < 0)
    Error("fcntl: getflags failed");
  return flags;
}

// ===================
int readWord(uint16_t *const data)
{
  static int bl;
  static int step = 0;
  int        bh;
  int        ret = 0;

  switch (step)
  {
    case 0:
      errno = 0;
      bl    = fgetc(driver);
      if (bl == EOF)
      {
        if (errno && errno != EAGAIN)
          Error(strerror(errno));
      }
      else
        step++;
      break;
    case 1:
      errno = 0;
      bh    = fgetc(driver);
      if (bh == EOF)
      {
        if (errno && errno != EAGAIN)
          Error(strerror(errno));
      }
      else
      {
        *data = ((bh & 0xFF) << 8) | (bl & 0xFF);
        step  = 0;
        ret   = 1;
      }
      break;
  }
  return ret;
}

// ===================
void readMessages(void)
{
  static int step  = 0;
  static int count = 0;
  uint16_t   word;

  switch (step)
  {
    case 0:  // read ID
      if (readWord(&id))
      {
        dataIndex = 0;
        step      = 1;
      }
      break;
    case 1:
      if (readWord(&len))
      {
        count = len;
        if (len)
          step = 2;
        else
          step = 3;
      }
      break;
    case 2:
      if (count)
      {
        if (readWord(&word))
        {
          count--;
          data[dataIndex] = word;
          if (++dataIndex >= MAX_DATA_SIZE)
            Error("LPC Message too long to fit in buffer");
        }
      }
      else
        step = 3;
      break;
    case 3:
      processReadMsgs(id, len, &data[0], displayFlags);
      step = 0;
      break;
  }
}

void writeMessages(void)
{
}

// ===================
void Usage(char const *const string, int const exitCode)
{
  if (string)
    puts(string);
  puts("read-lpc-msgs <options>");
  puts("  <options> is a white-space seperated list of letters, preceeded");
  puts("             by either a + or -, turning the display on or off");
  puts("  default is +a -d");
  puts(" a   All options");
  puts(" r   oveRlay messages of same type");
  puts(" c   diagnostiC status data");
  puts(" d   additional hex Dump, forces -r");
  puts(" e   EHC data");
  puts(" h   Heartbeat");
  puts(" k   Key logging");
  puts(" m   Mute status");
  puts(" n   Notificiation");
  puts(" p   Parameter");
  puts(" s   Sensors raw data");
  puts(" u   hexdump of Unknown messages");
  exit(exitCode);
}

// ===================
int main(int argc, char *argv[])
{
  int driverFileNo;
  int flags;

  printf("\nOutput from /dev/lpc_bb_driver:\n");

  driver = fopen("/dev/lpc_bb_driver", "r+");
  if (!driver)
    Error("cannot open /dev/lpc_bb_driver");

  driverFileNo = fileno(driver);
  if (driverFileNo == -1)
    Error("cannot get fileo of driver");

  flags = getDriverFlags(driverFileNo);
  makeDriverBlocking(driverFileNo, flags);

  displayFlags = NO_HEXDUMP;

  while (argc > 1)
  {
    if (strncmp(argv[1], "-a", 2) == 0)
      displayFlags |= NO_ALL;
    else if (strncmp(argv[1], "+a", 2) == 0)
      displayFlags &= ~NO_ALL;

    else if (strncmp(argv[1], "-r", 2) == 0)
      displayFlags |= NO_OVERLAY;
    else if (strncmp(argv[1], "+r", 2) == 0)
      displayFlags &= ~NO_OVERLAY;

    else if (strncmp(argv[1], "-c", 2) == 0)
      displayFlags |= NO_STATDATA;
    else if (strncmp(argv[1], "+c", 2) == 0)
      displayFlags &= ~NO_STATDATA;

    else if (strncmp(argv[1], "-d", 2) == 0)
      displayFlags |= NO_HEXDUMP;
    else if (strncmp(argv[1], "+d", 2) == 0)
      displayFlags &= ~NO_HEXDUMP;

    else if (strncmp(argv[1], "-e", 2) == 0)
      displayFlags |= NO_EHCDATA;
    else if (strncmp(argv[1], "+e", 2) == 0)
      displayFlags &= ~NO_EHCDATA;

    else if (strncmp(argv[1], "-h", 2) == 0)
      displayFlags |= NO_HEARTBEAT;
    else if (strncmp(argv[1], "+h", 2) == 0)
      displayFlags &= ~NO_HEARTBEAT;

    else if (strncmp(argv[1], "-k", 2) == 0)
      displayFlags |= NO_KEY_LOG;
    else if (strncmp(argv[1], "+k", 2) == 0)
      displayFlags &= ~NO_KEY_LOG;

    else if (strncmp(argv[1], "-m", 2) == 0)
      displayFlags |= NO_MUTESTATUS;
    else if (strncmp(argv[1], "+m", 2) == 0)
      displayFlags &= ~NO_MUTESTATUS;

    else if (strncmp(argv[1], "-n", 2) == 0)
      displayFlags |= NO_NOTIFICATION;
    else if (strncmp(argv[1], "+n", 2) == 0)
      displayFlags &= ~NO_NOTIFICATION;

    else if (strncmp(argv[1], "-p", 2) == 0)
      displayFlags |= NO_PARAMS;
    else if (strncmp(argv[1], "+p", 2) == 0)
      displayFlags &= ~NO_PARAMS;

    else if (strncmp(argv[1], "-s", 2) == 0)
      displayFlags |= NO_SENSORSRAW;
    else if (strncmp(argv[1], "+s", 2) == 0)
      displayFlags &= ~NO_SENSORSRAW;

    else if (strncmp(argv[1], "-u", 2) == 0)
      displayFlags |= NO_UNKNOWN;
    else if (strncmp(argv[1], "+u", 2) == 0)
      displayFlags &= ~NO_UNKNOWN;

    else
      Usage("unknown option", 3);
    argc--;
    argv++;
  }

  if (!(displayFlags & NO_HEXDUMP))
    displayFlags |= NO_OVERLAY;

  while (1)
  {
    readMessages();
    fflush(stdout);
    writeMessages();
  }
  return 0;
}
