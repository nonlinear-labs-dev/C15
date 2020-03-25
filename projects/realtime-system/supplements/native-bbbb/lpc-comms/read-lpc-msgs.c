#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "process-read-msgs.h"

#define MAX_DATA_SIZE (1000)
uint16_t id;
uint16_t len;
uint16_t data[MAX_DATA_SIZE];
int      dataIndex;
uint16_t displayFlags;
FILE *   driver;
int      dump;

// ===================
void makeDriverNonblocking(int const driverFileNo, int const flags)
{
  if (fcntl(driverFileNo, F_SETFL, flags | O_NONBLOCK) < 0)
    exit(3);
}

void makeDriverBlocking(int const driverFileNo, int const flags)
{
  if (fcntl(driverFileNo, F_SETFL, flags & (~O_NONBLOCK)) < 0)
    exit(3);
}

int getDriverFlags(int const driverFileNo)
{
  int flags;
  if ((flags = fcntl(driverFileNo, F_GETFL, 0)) < 0)
    exit(3);
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
      bl = fgetc(driver);
      if (bl == EOF)
      {
        if (errno != EAGAIN)
          exit(3);
      }
      else
        step++;
      break;
    case 1:
      bh = fgetc(driver);
      if (bh == EOF)
      {
        if (errno != EAGAIN)
          exit(3);
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

void print(uint16_t const data)
{
  if (dump)
    printf("%04X ", data);
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
        print(id);
        dataIndex = 0;
        step      = 1;
      }
      break;
    case 1:
      if (readWord(&len))
      {
        print(len);
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
          print(data[dataIndex]);
          if (++dataIndex >= MAX_DATA_SIZE)
            exit(3);
        }
      }
      else
        step = 3;
      break;
    case 3:
      if (dump)
        printf("\n");
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
  puts(" -d   no raw hex dump");
  puts(" -e   no EHC data");
  puts(" -h   no heartbeats");
  puts(" -m   no mute status");
  puts(" -n   no notificiations");
  puts(" -p   no parameters");
  puts(" -s   no sensors raw data");
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
  {
    printf("\nI/O-Error! Aborting.\n");
    return 3;
  }

  driverFileNo = fileno(driver);
  if (driverFileNo == -1)
    exit(3);

  flags = getDriverFlags(driverFileNo);
  makeDriverBlocking(driverFileNo, flags);

  displayFlags = 0;
  dump         = 1;

  while (argc > 1)
  {
    if (strncmp(argv[1], "-d", 2) == 0)
      dump = 0;
    else if (strncmp(argv[1], "-e", 2) == 0)
      displayFlags |= NO_EHCDATA;
    else if (strncmp(argv[1], "-h", 2) == 0)
      displayFlags |= NO_HEARTBEAT;
    else if (strncmp(argv[1], "-m", 2) == 0)
      displayFlags |= NO_MUTESTATUS;
    else if (strncmp(argv[1], "-n", 2) == 0)
      displayFlags |= NO_NOTIFICATION;
    else if (strncmp(argv[1], "-p", 2) == 0)
      displayFlags |= NO_PARAMS;
    else if (strncmp(argv[1], "-s", 2) == 0)
      displayFlags |= NO_SENSORSRAW;
    else
      Usage(NULL, 3);
    argc--;
    argv++;
  }

  while (1)
  {
    readMessages();
    fflush(stdout);
    writeMessages();
  }
  return 0;
}
