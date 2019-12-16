#include <stdio.h>
#include "interpol.h"

void Usage(void)
{
  printf("Usage: gencal <raw-sensor-data-file> <calibration-output-file>\n");
  printf("    <raw-sensor-data-file>    : input, text-file with calibration fixture readout value pairs\n");
  printf("    <calibration-output-file> : output, text-file with human-readable calibration data\n");
}

int main(void)
{
  float const X[] = { 1, 2, 3, 4 };
  float const Y[] = { 5, 6, 7, 8 };
  unsigned const POINTS = sizeof X / sizeof X[0];
  auto table = new Interpol::Interpol(POINTS, X, Y);

  printf("%f\n", double(table->lookUpAndInterpolate(0.5)));

  delete(table);
  return 0;
}

// EOF
