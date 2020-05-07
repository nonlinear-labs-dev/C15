#include <stdio.h>
#include <stdint.h>
#include <string.h>

int main(void)
{
  uint16_t a;
  uint16_t b;
  char     buffer[1024];
  char*    buf;
  char     MSG_ID[] = "BB_MSG_TYPE_SENSORS_RAW: ";
  int      MSG_ID_LEN;
  int      ret;
  while (1)
  {
    buf = buffer;
    gets(buffer);           // yeah I know this dangerous if the line is larger than the buffer...
    if (buffer[0] == 0x1B)  // "cursor up" escape sequence "\033[1A" ?
      buf += 4;             // skip it
    MSG_ID_LEN = strlen(MSG_ID);
    if (strncmp(MSG_ID, buf, MSG_ID_LEN) != 0)  // ID string found ?
      continue;                                 // no, get next line
    // input line now looks like: "BB_MSG_TYPE_SENSORS_RAW: 1111 2300 4079 2073 4080 2085 4079 2233 4081 2044 4095    0    0"
    buf += MSG_ID_LEN;  // skip ID string

    //printf("%s\n", buf);

    // read in only last two values of 13 total
    ret = sscanf(buf, "%*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %hu %hu", &a, &b);
    if (ret != 2)  // scanf failed, just in case ?
      continue;
    if (a > 3 || b > 3)  // filter out zero (+noise)
    {
      printf("%5hu %5hu\n", a, b);
      fflush(stdout);  // flush output, so that piping updates nicely when used via SSH
    }
  }
}
