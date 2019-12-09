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
    printf("FATAL: Cannot open input file \"%s\"\n", argv[1]);
    return 3;  // --> exit
  }

  ctr::CalToRef cal(ctr::CalToRef::VerboseMessages);

  cal.startAddIn();
  while(fscanf(infile, "%hu %hu", &ref, &dut) != EOF)
  {
    if(cal.addInSamplePair(ref, dut) >= 3)
    {
      printf("FATAL: value(s) out of legal range. Program terminated\n");
      return 3;  // severe error, eg values out of bounds --> exit
    }
  }
  cal.endAddIn();

  if(cal.getValidSamples() == 0)
  {
    printf("Not enough valid samples, program terminated without effect\n");
    return 2;
  }

  if(!cal.ProcessData())
  {
    printf("processing failed, program terminated without effect\n");
    return 2;
  }

  cal.OutputData(nullptr);

  return 0;
}

// EOF
