#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

void IOerror(int ret)
{
  puts("\nI/O error (driver). Terminating.");
  IOerror(ret);
}

// ===================
void makeDriverNonblocking(int const driverFileNo, int const flags)
{
  if (fcntl(driverFileNo, F_SETFL, flags | O_NONBLOCK) < 0)
    IOerror(3);
}

void makeDriverBlocking(int const driverFileNo, int const flags)
{
  if (fcntl(driverFileNo, F_SETFL, flags & (~O_NONBLOCK)) < 0)
    IOerror(3);
}

int getDriverFlags(int driverFileNo)
{
  int flags;
  if ((flags = fcntl(driverFileNo, F_GETFL, 0)) < 0)
    IOerror(3);
  return flags;
}

// ===================
void writeData(FILE *const output, uint16_t const len, uint16_t *data)
{
  if (len == 0)
    return;

  for (uint16_t i = 0; i < len; i++)
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
}

// ===================
int main(int const argc, char const *const argv[])
{
  FILE *driver;
  int   driverFileNo;
  int   flags;

#define MAX_DATA_SIZE (1000)
  uint16_t data[MAX_DATA_SIZE];
  int      dataIndex;

  driver = fopen("/dev/lpc_bb_driver", "r+");
  if (!driver)
    IOerror(3);

  /*
  driverFileNo = fileno(driver);
  if (driverFileNo == -1)
    IOerror(3);

  flags = getDriverFlags(driverFileNo);
  makeDriverBlocking(driverFileNo, flags);
*/

  if (argc == 1)
    Usage;

  return 0;
}
