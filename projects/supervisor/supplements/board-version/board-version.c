/*
 * GPIO user space helpers
 *
 * Copyright 2009 Analog Devices Inc.
 * Michael Hennerich (hennerich@blackfin.uclinux.org)
 *
 * Licensed under the GPL-2 or later
 */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define GPIO_DIR_IN  0
#define GPIO_DIR_OUT 1

int linuxgpio_export(unsigned int gpio)
{
  int  fd, len, r;
  char buf[11];

  fd = open("/sys/class/gpio/export", O_WRONLY);
  if (fd < 0)
  {
    perror("Can't open /sys/class/gpio/export");
    return fd;
  }

  len = snprintf(buf, sizeof(buf), "%u", gpio);
  r   = write(fd, buf, len);
  close(fd);

  return r;
}

int linuxgpio_unexport(unsigned int gpio)
{
  int  fd, len, r;
  char buf[11];

  fd = open("/sys/class/gpio/unexport", O_WRONLY);
  if (fd < 0)
  {
    perror("Can't open /sys/class/gpio/unexport");
    return fd;
  }

  len = snprintf(buf, sizeof(buf), "%u", gpio);
  r   = write(fd, buf, len);
  close(fd);

  return r;
}

static int linuxgpio_dir(unsigned int gpio, unsigned int dir)
{
  int  fd, r;
  char buf[60];

  snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%u/direction", gpio);

  fd = open(buf, O_WRONLY);
  if (fd < 0)
  {
    perror("Can't open gpioX/direction");
    return fd;
  }

  if (dir == GPIO_DIR_OUT)
    r = write(fd, "out", 4);
  else
    r = write(fd, "in", 3);

  close(fd);

  return r;
}

int linuxgpio_dir_out(unsigned int gpio)
{
  return linuxgpio_dir(gpio, GPIO_DIR_OUT);
}

int linuxgpio_dir_in(unsigned int gpio)
{
  return linuxgpio_dir(gpio, GPIO_DIR_IN);
}

static int linuxgpio_openfd(unsigned int gpio)
{
  char filepath[60];

  snprintf(filepath, sizeof(filepath), "/sys/class/gpio/gpio%u/value", gpio);
  return (open(filepath, O_RDWR));
}

// -----------------------------------------------------------------------------------------------------
#define BIT0 (46u)  // port number bit 0
#define BIT1 (27u)  // port number bit 1
//#define BIT2         (65u)    // port number bit 2   // (old hardware, not usable with BBB with our device tree)
#define BIT2       (76u)  // port number bit 2   // new V7.2 hardware
#define BIT3       (22u)  // port number bit 3
#define V7_2_TEST1 (80u)  // one side of V7.2 ID-resistor
#define V7_2_TEST2 (77u)  // other side of V7.2 ID-resistor

#define WAIT_TIME_US (10000u)  // wait time for IO-accesses to settle

int main(void)
{
  int fd[6];

  linuxgpio_unexport(BIT0);
  linuxgpio_unexport(BIT1);
  linuxgpio_unexport(BIT2);
  linuxgpio_unexport(BIT3);
  linuxgpio_unexport(V7_2_TEST1);
  linuxgpio_unexport(V7_2_TEST2);

  linuxgpio_export(BIT0);
  linuxgpio_export(BIT1);
  linuxgpio_export(BIT2);
  linuxgpio_export(BIT3);
  linuxgpio_export(V7_2_TEST1);
  linuxgpio_export(V7_2_TEST2);

  linuxgpio_dir_in(BIT0);
  linuxgpio_dir_in(BIT1);
  linuxgpio_dir_in(BIT2);
  linuxgpio_dir_in(BIT3);
  linuxgpio_dir_out(V7_2_TEST1);
  linuxgpio_dir_in(V7_2_TEST2);

  usleep(WAIT_TIME_US);
  fd[0] = linuxgpio_openfd(BIT0);
  fd[1] = linuxgpio_openfd(BIT1);
  fd[2] = linuxgpio_openfd(BIT2);
  fd[3] = linuxgpio_openfd(BIT3);
  fd[4] = linuxgpio_openfd(V7_2_TEST1);
  fd[5] = linuxgpio_openfd(V7_2_TEST2);
  usleep(WAIT_TIME_US);

  int  boardId = 0;
  char c;

#if 01
  // check if hardware is >= V7.2
  if (write(fd[4], "1", 1) != 1)
  {
    perror("Write to GPIO failed");
    goto Error;
  }
  usleep(WAIT_TIME_US);
  if (read(fd[5], &c, 1) != 1)
  {
    perror("Read from GPIO failed");
    goto Error;
  }
  if (c != '1' && c != '0')
  {
    perror("Read from GPIO was neither 0 nor 1");
    printf("c = %02Xh\n", c);
    goto Error;
  }
  if (c == 0)
  {
    boardId = -1;  // < V7.2, board Id cannot be detected
    goto Done;
  }
  close(fd[5]);
  fd[5] = linuxgpio_openfd(V7_2_TEST2);

  if (write(fd[4], "0", 1) != 1)
  {
    perror("Write to GPIO failed");
    goto Error;
  }
  usleep(WAIT_TIME_US);
  if (read(fd[5], &c, 1) != 1)
  {
    perror("Read from GPIO failed");
    goto Error;
  }
  if (c != '1' && c != '0')
  {
    perror("Read from GPIO was neither 0 nor 1");
    printf("c = %02Xh\n", c);
    goto Error;
  }
  if (c == '1')
  {
    boardId = -1;  // < V7.2, board Id cannot be detected
    goto Done;
  }
#endif

  // read in board version bits
  if (read(fd[0], &c, 1) != 1)
    goto Error;
  if (c == '1')
    boardId += 1;
  else if (c != '0')
    goto Error;

  if (read(fd[1], &c, 1) != 1)
    goto Error;
  if (c == '1')
    boardId += 2;
  else if (c != '0')
    goto Error;

  if (read(fd[2], &c, 1) != 1)
    goto Error;
  if (c == '1')
    boardId += 4;
  else if (c != '0')
    goto Error;

  if (read(fd[3], &c, 1) != 1)
    goto Error;
  if (c == '1')
    boardId += 8;
  else if (c != '0')
    goto Error;

  goto Done;
Error:
  boardId = -3;

Done:
  close(fd[0]);
  close(fd[1]);
  close(fd[2]);
  close(fd[3]);
  close(fd[4]);
  close(fd[5]);
  linuxgpio_unexport(BIT0);
  linuxgpio_unexport(BIT1);
  linuxgpio_unexport(BIT2);
  linuxgpio_unexport(BIT3);
  linuxgpio_unexport(V7_2_TEST1);
  linuxgpio_unexport(V7_2_TEST2);

  return boardId;
}
