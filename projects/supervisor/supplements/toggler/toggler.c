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
#define TOGGLE       (61u)    // port number of the toggle pin GPIO
#define WAIT_TIME_US (5000u)  // cycle time for the toggling, 5msec (200Hz) nominal, to be lower than the safe max of 10msec (100Hz)
int main(void)
{
  int fd;

  linuxgpio_export(TOGGLE);
  usleep(WAIT_TIME_US / 2u);
  linuxgpio_dir_out(TOGGLE);
  usleep(WAIT_TIME_US / 2u);
  fd = linuxgpio_openfd(TOGGLE);
  usleep(WAIT_TIME_US / 2u);

  while (1)
  {
    if (write(fd, "1", 1) != 1)
      break;
    usleep(WAIT_TIME_US / 2u);
    if (write(fd, "0", 1) != 1)
      break;
    usleep(WAIT_TIME_US / 2u);
  }

  close(fd);
  linuxgpio_unexport(TOGGLE);
  return 0;
}
