#include <stdio.h>
#include <stdint.h>
#include "cal_to_ref.h"

void Usage(const char* exe)
{
  printf("Usage: %s input-file\n", exe);
}

int main(int argc, char* argv[])
{
  uint16_t ref;
  uint16_t dut;

  if(argc != 2)
  {
    Usage(argv[0]);
    return 3;
  }

  FILE* infile;
  if((infile = fopen(argv[1], "r")) == nullptr)
  {
    printf("ERROR: Cannot open input file \"%s\"\n", argv[1]);
    return 3;  // --> exit
  }

  ctr::CalToRef cal(ctr::CalToRef::Verbose);

  cal.addInValuesStart();
  while(fscanf(infile, "%hu %hu", &ref, &dut) != EOF)
  {
    if(cal.addInValues(ref, dut) >= 3)
      return 3;  // severe error, eg values out of bounds --> exit
  }
  cal.addInValuesEnd();

  cal.doAveraging();

  return 0;
}

// EOF
