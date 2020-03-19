#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

FILE *driver;
int driverFileNo;
int flags;

uint16_t id, len, data;

// ===================
void makeDriverNonblocking(int flags)
{
  if(fcntl(driverFileNo, F_SETFL, flags | O_NONBLOCK) < 0)
    exit(3);
}

void makeDriverBlocking(int flags)
{
  if(fcntl(driverFileNo, F_SETFL, flags & (~O_NONBLOCK)) < 0)
    exit(3);
}

int getDriverFlags()
{
  int flags;
  if((flags = fcntl(driverFileNo, F_GETFL, 0)) < 0)
    exit(3);
  return flags;
}

// ===================
int readWord(uint16_t *data)
{
  static int bl;
  static int step = 0;
  int bh;
  int ret = 0;

  switch(step)
  {
    case 0:
      bl = fgetc(driver);
      if(bl != EAGAIN)
        step++;
      break;
    case 1:
      bh = fgetc(driver);
      if(bh != EAGAIN)
      {
        *data = ((bh & 0xFF) << 8) | (bl & 0xFF);
        step = 0;
        ret = 1;
      }
      break;
  }
  return ret;
}

void print(uint16_t data)
{
  printf("%04X ", data);
  fflush(stdout);
}

// ===================
void readMessages(void)
{
  static int step = 0;
  switch(step)
  {
    case 0:  // read ID
      if(readWord(&id))
      {
        print(id);
        step++;
      }
      break;
    case 1:
      if(readWord(&len))
      {
        print(len);
        if(len)
          step++;
        else
          step = 0;
      }
      break;
    case 2:
      if(readWord(&data))
      {
        if(len--)
          print(data);
        else
          step = 0;
      }
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

  driver = fopen("/dev/lpc_bb_driver", "rb");
  if(!driver)
  {
    printf("\nI/O-Error! Aborting.\n");
    return 3;
  }

  driverFileNo = fileno(driver);
  if(driverFileNo == -1)
    exit(3);

  flags = getDriverFlags();
  makeDriverBlocking(flags);

  while(1)
  {
    readMessages();
    writeMessages();
  }
  return 0;
}
