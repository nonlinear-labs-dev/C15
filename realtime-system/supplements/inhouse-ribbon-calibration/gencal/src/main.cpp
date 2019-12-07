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

  int ret = 0;
  int warnings = 0;
  int notes = 0;

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

  puts("Reading ref and dut raw values...");
  while(fscanf(infile, "%hu %hu", &ref, &dut) != EOF)
  {
    printf("%5hu %5hu\n", ref, dut);
    ret = ctr::addInValues(ref, dut);
    switch(ret)
    {
      case 3:
        return 3;  // severe error, eg values out of bounds --> exit
      case 2:
        ++warnings;
        break;
      case 1:
        ++notes;
        break;
    }
  }
  printf("Reading done, with %d warning(s) and %d note(s)\n", warnings, notes);

  ctr::doAveraging();

  return 0;
}

// EOF
