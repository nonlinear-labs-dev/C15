#include <stdio.h>
#include <stdint.h>

int main(void)
{
  uint16_t a;
  uint16_t b;
  char buffer[1024];
  int i;
  int ret;
  while(1)
  {
    gets(buffer);
    switch(buffer[0])
    {
      case 0x1B:
        i = 4;
        break;
      case '0':
      case '1':
        i = 0;
        break;
      default:
        i = -1;
        break;
    }
    if(i == -1)
      continue;
    //printf("%s\n", &buffer[i]);
    ret = sscanf(&buffer[i], "%*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %hu %hu", &a, &b);
    if(ret != 2)
      continue;
    if(a > 2 || b > 2)
    {
      printf("%5hu %5hu\n", a, b);
      fflush(stdout);
    }
  }
}
