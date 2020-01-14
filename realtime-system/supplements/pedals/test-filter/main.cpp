#include <stdio.h>
#include <stdint.h>
#include <string.h>

int main(void)
{
  uint16_t tip1;
  uint16_t ring1;
  uint16_t tip2;
  uint16_t ring2;
  uint16_t tip3;
  uint16_t ring3;
  char buffer[1024];
  char *buf;
  char MSG_ID[] = "BB_MSG_TYPE_SENSORS_RAW: ";
  int MSG_ID_LEN;
  int ret;
  double min = 1000;
  double max = -1000;
  double min1 = min, max1 = max;
  double x = 0, y = 0, z = 0;

  printf("\n\n");
  while(1)
  {
    buf = buffer;
    gets(buffer);          // yeah I know this dangerous if the line is larger than the buffer...
    if(buffer[0] == 0x1B)  // "cursor up" escape sequence "\033[1A" ?
      buf += 4;            // skip it
    MSG_ID_LEN = strlen(MSG_ID);
    if(strncmp(MSG_ID, buf, MSG_ID_LEN) != 0)  // ID string found ?
      continue;                                // no, get next line
    // input line now looks like: "BB_MSG_TYPE_SENSORS_RAW: 1111 2300 4079 2073 4080 2085 4079 2233 4081 2044 4095    0    0"
    buf += MSG_ID_LEN;  // skip ID string

    // read in
    ret = sscanf(buf, "%*s %hu %hu %hu %hu %hu %hu %*s %*s %*s %*s %*s %*s", &tip1, &ring1, &tip2, &ring2, &tip3,
                 &ring3);
    if(ret != 6)  // scanf failed, just in case ?
      continue;

#if 0
    // calculate floating precentage
    x = double(tip1) / double(ring1) * 100.0;

    if(x < min)
      min = x;
    if(x > max)
      max = x;

    if(max > min)
    {
      min1 = min + (max - min) * 0.03;
      max1 = max - (max - min) * 0.03;
    }

    y = x;
    z = -1;

    if(max1 - min1 > 5)
    {
      if(y < min1)
        z = 0.0;
      else if(y > max1)
        z = 100.0;
      else
      {
        z = (y - min1) / (max1 - min1) * 100;
      }
    }
#endif

    // printf("%s \n", buf);
    // printf("T:%4hu   R:%4hu   T/R:%5.1lf%%   OUT:%5.1lf%%   max:%5.1lf   min:%5.1lf  \n", wiper1, top1, x, z, max, min);
    printf("%5d %5d \n", tip1, ring1);

    printf("\033[1A");
    fflush(stdout);  // flush output, so that piping updates nicely when used via SSH
  }
}
