#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#include "process-read-msgs.h"

#define PRINT_RAW (01)

FILE *driver;
int   driverFileNo;
int   flags;

#define MAX_DATA_SIZE (1000)
uint16_t id;
uint16_t len;
uint16_t data[MAX_DATA_SIZE];
int      dataIndex;

// ===================
void makeDriverNonblocking(int flags)
{
  if (fcntl(driverFileNo, F_SETFL, flags | O_NONBLOCK) < 0)
    exit(3);
}

void makeDriverBlocking(int flags)
{
  if (fcntl(driverFileNo, F_SETFL, flags & (~O_NONBLOCK)) < 0)
    exit(3);
}

int getDriverFlags()
{
  int flags;
  if ((flags = fcntl(driverFileNo, F_GETFL, 0)) < 0)
    exit(3);
  return flags;
}

// ===================
int readWord(uint16_t *data)
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

void print(uint16_t data)
{
#if PRINT_RAW
  printf("%04X ", data);
  fflush(stdout);
#endif
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
#if PRINT_RAW
      printf("\n");
#endif
      processReadMsgs(id, len, &data[0]);
      step = 0;
      break;
  }
}

void writeMessages(void)
{
}

// ===================
int main(void)
{
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

  flags = getDriverFlags();
  makeDriverBlocking(flags);

  while (1)
  {
    readMessages();
    writeMessages();
  }
  return 0;
}
