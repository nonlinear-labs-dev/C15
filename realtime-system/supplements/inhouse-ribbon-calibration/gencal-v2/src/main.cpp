#include <stdio.h>
#include <stdint.h>
#include "cal_to_ref.h"

void Usage(void)
{
  printf("Usage: gencal <raw-sensor-data-file> <calibration-output-file>\n");
  printf("    <raw-sensor-data-file>    : input, text-file with calibration fixture readout value pairs\n");
  printf("    <calibration-output-file> : output, text-file with human-readable calibration data\n");
}

int main(int const argc, char const* const argv[])
{
  uint16_t ref;
  uint16_t dut;

  if(argc != 3)
  {
    Usage();
    return 3;
  }

  FILE* infile;
  if((infile = fopen(argv[1], "r")) == nullptr)
  {
    printf("FATAL: Cannot open input file \"%s\"\n", argv[1]);
    return 3;  // --> exit
  }
  FILE* outfile;
  if((outfile = fopen(argv[2], "w")) == nullptr)
  {
    printf("FATAL: Cannot open output file \"%s\"\n", argv[1]);
    return 3;  // --> exit
  }

  Calibrator::CalToRef cal(Calibrator::CalToRef::VerboseMessages);

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

  cal.OutputData(outfile);

  return 0;
}

// EOF
