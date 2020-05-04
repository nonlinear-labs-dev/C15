/* 
   ESPI Driver for Encoder on Panel Unit (Edit Panel)
   
   Reading the driver returns the increments (positive values, clockwise rotation)
   or decrements (negative values, counter-clockwise rotation), as signed 8 bit values,
   since the last read operation (accumulative, but saturating to +127 or -128).
   Zero change is not reported.
   
   Writing the driver emulates Encoder action, using the same value definition, and it
   again is accumulative until the value is fetched by a read operation.
   Writing a zero (0) has a special function, it clears the accumulator and doesn't
   put anything in the read buffer.

   --------------------
   
   The driver also is used to continuosly check if the Panel Unit is not connected,
   the low-level read operation in the poll() routine can detect this.

   It exports a "panel_unit_is_online()" boolean function that goes FALSE (0) when
   the driver detects that the Panel Unit is not detected. After it goes online again,
   and keeps being detected online all the time, panel_unit_is_online() returns
   TRUE (!=0) after some specific timeout to wait for the plug-in process to be likely
   completed (1 Second or so).

*/

#include <linux/poll.h>
#include <linux/jiffies.h>

#include "espi_driver.h"
#include "espi_fb.h"

/* Encoder stuff **************************************************************/
static s8 encoder_delta;
static DECLARE_WAIT_QUEUE_HEAD(encoder_wqueue);

#define REPLUGGING_TIMEOUT (1000 * 1000)  // timeout after re-plugging the panel unit, in Microseconds
static s64 timeout = 0;

static void retrigger_offline_timeout(void)
{
  timeout = REPLUGGING_TIMEOUT;
}

// timer stuff, to implement a delay after panel unit is detected before processing starts
static void process_offline_timeout(void)
{
  static u8  first       = 1;
  static u64 last_update = 0;

  u64          now    = get_jiffies_64();
  u64          diff   = now - last_update;
  unsigned int diffUS = jiffies_to_usecs(diff);
  last_update         = now;

  if (first)
  {
    first = 0;
    return;
  }
  timeout -= diffUS;
  if (timeout < 0)
    timeout = 0;
}

int panel_unit_is_online(void)
{
  return timeout <= 0;
}

/*******************************************************************************
    encoder functions
*******************************************************************************/
// By writing 0 any accumulated values can be cleared, to avoid "uncontrolled"
// encoder action after (re-)starting playground
// Other values directly emulate true encoder driver output
static ssize_t encoder_fops_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
  s8  tmp;
  s16 new_delta;

  if (count == 0)
    return 0;
  if (copy_from_user(&tmp, buf, 1))
    return -EFAULT;

  if (tmp)  // regular value ?
  {         //   then add up until fetched
    new_delta = encoder_delta + tmp;
    if (new_delta > 127)
      new_delta = 127;
    if (new_delta < -128)
      new_delta = -128;
    encoder_delta = new_delta;
  }
  else  // clear request
    encoder_delta = 0;

  //printk("emulated encoder delta: %d\n", (s8)encoder_delta);
  wake_up_interruptible(&encoder_wqueue);

  return 1;
}

static ssize_t encoder_fops_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
  ssize_t status;

  /* If in non-blocking mode and no data to read, return */
  if (filp->f_flags & O_NONBLOCK && encoder_delta == 0)
    return -EAGAIN;

  /* Sleep until there is data to read */
  if ((status = wait_event_interruptible(encoder_wqueue, encoder_delta != 0)))
    return status;

  if (copy_to_user(buf, &encoder_delta, 1) != 0)
    return -EFAULT;

  encoder_delta = 0;

  return 1;
}

static unsigned int encoder_fops_poll(struct file *filp, poll_table *wait)
{
  unsigned int mask = 0;
  poll_wait(filp, &encoder_wqueue, wait);

  /* If there is data in buffer, reading is allowed */
  if (encoder_delta != 0)
    mask |= POLLIN | POLLRDNORM;

  return mask;
}

static const struct file_operations encoder_fops = {
  .owner  = THIS_MODULE,
  .write  = encoder_fops_write,
  .read   = encoder_fops_read,
  .open   = nonseekable_open,
  .llseek = no_llseek,
  .poll   = encoder_fops_poll,
};

static struct class *encoder_class;

s32 espi_driver_encoder_setup(struct espi_driver *sb)
{
  encoder_delta = 0;

  if (register_chrdev(ESPI_ENCODER_DEV_MAJOR, "spi", &encoder_fops) < 0)
    pr_err("%s: problem at register_chrdev\n", __func__);

  encoder_class = class_create(THIS_MODULE, "espi-encoder");
  if (IS_ERR(encoder_class))
    pr_err("%s: unable to create class\n", __func__);

  device_create(encoder_class, sb->dev, MKDEV(ESPI_ENCODER_DEV_MAJOR, 0), sb, "espi_encoder");

  return 0;
}

s32 espi_driver_encoder_cleanup(struct espi_driver *sb)
{
  device_destroy(encoder_class, MKDEV(ESPI_ENCODER_DEV_MAJOR, 0));
  class_destroy(encoder_class);
  unregister_chrdev(ESPI_ENCODER_DEV_MAJOR, "spi");

  return 0;
}

void espi_driver_encoder_poll(struct espi_driver *p)
{
  struct spi_transfer xfer;
  u8                  rx_buff[3];
  u8                  tx_buff[3];
  u8                  tmp;
  extern int          sck_hz;
  s16                 new_delta;

  tx_buff[0] = 0xAA;

  xfer.tx_buf        = tx_buff;
  xfer.rx_buf        = rx_buff;
  xfer.len           = 1;
  xfer.bits_per_word = 8;
  xfer.delay_usecs   = 0;
  xfer.speed_hz      = sck_hz;

  espi_driver_scs_select((struct espi_driver *) p, ESPI_EDIT_PANEL_PORT, ESPI_EDIT_ENCODER_DEVICE);
  espi_driver_transfer(((struct espi_driver *) p)->spidev, &xfer);
  espi_driver_scs_select((struct espi_driver *) p, ESPI_EDIT_PANEL_PORT, 0);
  tmp = rx_buff[0];

  tx_buff[0] = 0;
  espi_driver_scs_select((struct espi_driver *) p, ESPI_EDIT_PANEL_PORT, ESPI_EDIT_ENCODER_DEVICE);
  espi_driver_transfer(((struct espi_driver *) p)->spidev, &xfer);
  espi_driver_scs_select((struct espi_driver *) p, ESPI_EDIT_PANEL_PORT, 0);
  tmp &= rx_buff[0];

  process_offline_timeout();
  if (tmp == 0xFF)
  {  // encoder seems to be offline, so assume panel unit is not connected
    retrigger_offline_timeout();
    return;
  }

  if (rx_buff[0] != 0)
  {
    new_delta = encoder_delta + (s8) rx_buff[0];
    if (new_delta > 127)
      new_delta = 127;
    if (new_delta < -128)
      new_delta = -128;
    encoder_delta = new_delta;

    //printk("encoder delta: %d\n", (s8)encoder_delta);
    wake_up_interruptible(&encoder_wqueue);
  }
}
