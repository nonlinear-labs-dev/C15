#include <stdio.h>
#include <stdint.h>
#include <string.h>

void Usage(void)
{
  printf("Usage: make-cal-bin <calibration-file> <binary-output-file>\n");
  printf("    <calibration-file>   : input, text-file with calibration fixture readout value pairs\n");
  printf(
      "    <binary-output-file> : output, binary file for sending cal data to LPC directly via /dev/lpc_bb_driver\n");
}

int main(int const argc, char const* const argv[])
{
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
  if((outfile = fopen(argv[2], "wb")) == nullptr)
  {
    printf("FATAL: Cannot open output file \"%s\"\n", argv[1]);
    return 3;  // --> exit
  }

  class RibbonData
  {
   public:
    int valid = 0;
    uint16_t X[34];
    uint16_t Y[33];
  };

  RibbonData ribbon[2];

  char buffer[1024];
  enum Action
  {
    NONE = -1,
    RIBBON1 = 0,
    RIBBON2 = 1
  };

  while(!feof(infile))
  {
    char* buf = buffer;
    if(fgets(buf, 1024, infile) == nullptr)
      break;
    Action action = NONE;
    if(strncmp("Ribbon1=", buf, 8) == 0)
      action = RIBBON1;
    else if(strncmp("Ribbon2=", buf, 8) == 0)
      action = RIBBON2;

    switch(action)
    {
      case NONE:
        break;
      case RIBBON1:
      case RIBBON2:
      {
        int select = int(action) - int(RIBBON1);
        buf += 8;
        for(int i = 0; buf[i] != 0; i++)
          if(buf[i] == '\n')
          {
            buf[i] = 0;
            break;
          }
        FILE* ribfile;
        if((ribfile = fopen(buf, "r")) == nullptr)
        {
          printf("FATAL: Cannot open ribbon file \"%s\"\n", buf);
          return 3;  // --> exit
        }

        {
          char buffer[1024];
          char* buf = buffer;
          while(!feof(ribfile))
          {
            buf = buffer;
            if(fgets(buf, 1024, ribfile) == nullptr)
              break;
            if(strncmp("[Calibration-X]", buf, 15) == 0)
            {
              fgets(buf, 1024, ribfile);
              for(int k = 0; buf[k] != 0; k++)
              {
                if(buf[k] == ',' || buf[k] == '\n')
                {
                  buf[k] = ' ';
                }
              }

              for(int i = 0; i < 34; i++)
              {
                if(sscanf(buf, "%hu", &(ribbon[select].X[i])) != 1)
                {
                  printf("FATAL: Cannot read 34 X values ribbon file\n");
                  return 3;  // --> exit
                }
                while(*buf != ' ' && *buf != 0)
                  buf++;
                if(*buf == ' ')
                  buf++;
              }

              ribbon[select].valid++;
            }
            else if(strncmp("[Calibration-Y]", buf, 15) == 0)
            {
              fgets(buf, 1024, ribfile);
              for(int k = 0; buf[k] != 0; k++)
              {
                if(buf[k] == ',' || buf[k] == '\n')
                {
                  buf[k] = ' ';
                }
              }

              for(int i = 0; i < 33; i++)
              {
                if(sscanf(buf, "%hu", &(ribbon[select].Y[i])) != 1)
                {
                  printf("FATAL: Cannot read 33 Y values ribbon file\n");
                  return 3;  // --> exit
                }
                while(*buf != ' ' && *buf != 0)
                  buf++;
                if(*buf == ' ')
                  buf++;
              }

              ribbon[select].valid++;
            }
          }
        }
        fclose(ribfile);
      }
      break;
    }
  }
  fclose(infile);

  if(ribbon[0].valid < 2 || ribbon[1].valid < 2)
  {
    printf("FATAL: failed to find/analyse two complete calibration sets\n");
    return 3;  // --> exit
  }

  uint16_t header[] = {
    0x0D00  // BB_MSG_TYPE_RIBBON_CAL
    ,
    2 * 33 + 2 * 34  // message size
  };
  if(fwrite(header, 2, 2, outfile) != 2)
  {
    printf("FATAL: Error writing binary file\n");
    return 3;  // --> exit
  }
  if(fwrite(ribbon[0].X, 2, 34, outfile) != 34)
  {
    printf("FATAL: Error writing binary file\n");
    return 3;  // --> exit
  }
  if(fwrite(ribbon[0].Y, 2, 33, outfile) != 33)
  {
    printf("FATAL: Error writing binary file\n");
    return 3;  // --> exit
  }
  if(fwrite(ribbon[1].X, 2, 34, outfile) != 34)
  {
    printf("FATAL: Error writing binary file\n");
    return 3;  // --> exit
  }
  if(fwrite(ribbon[1].Y, 2, 33, outfile) != 33)
  {
    printf("FATAL: Error writing binary file\n");
    return 3;  // --> exit
  }

  fclose(outfile);

  printf("Success!\n");
  return 0;
}

// EOF
