/* 
   ESPI Driver for Ribbon Strip LEDS

LED ID layout (decimal)
     Left                                           Center                                           Right
Led # 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32
Upper 01 03 05 07 09 11 13 15 17 19 21 23 25 27 29 31 33 35 37 39 41 43 45 47 49 51 53 55 57 59 61 63 65
Lower 00 02 04 06 08 10 12 14 16 18 20 22 24 26 28 30 32 34 36 38 40 42 44 46 48 50 52 54 56 58 60 62 64

LED intensity value (binary, only Bits 0..1 of value byte are used)
 00 : off
 01 : dim
 10 : normal
 11 : bright
 
Setting up a LED requires 2 bytes written to the driver:
 - LED ID
 - Intensity
 When writing an odd number of bytes, the last byte will be ignored to avoid going out
 of sync. But when sync is lost for other reasons, no attempt is made to resync !
 
The LEDs are force-updated in specific time intervals to care for ESD-induced upsets.
 
 */

#include <linux/string.h>
#include <linux/of_gpio.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>
#include "espi_driver.h"

#define ESD_INTERVAL              (250)  // force update of LEDs every 250ms, to cater for ESD etc glitches
#define NUMBER_OF_LEDS_PER_RIBBON (33)
#define NUMBER_OF_BITS_PER_LED    (2)
#define NUMBER_OF_RIBBONS         (2)
#define TOTAL_NUMBER_OF_BITS      (NUMBER_OF_LEDS_PER_RIBBON * NUMBER_OF_BITS_PER_LED * NUMBER_OF_RIBBONS)
#define RIBBON_LED_STATES_SIZE    ((u16)((TOTAL_NUMBER_OF_BITS + 7) / 8))  // number of bytes needed to store the LED bits
static u8  rb_led_st[RIBBON_LED_STATES_SIZE];
static u8  rb_led_new_st[RIBBON_LED_STATES_SIZE];
static u64 lastUpdate = 0;
static DEFINE_MUTEX(rbled_state_lock);
static u8 force_update = 0;

// ---- write to driver function ----
// writes requested LED states to "new" buffer
static ssize_t rbled_write(struct file *filp, const char __user *buf_user, size_t count, loff_t *f_pos)
{
  char            buf[count];
  ssize_t         status = 0;
  u32             i, led_index;
  u8              val, led_id;
  static const u8 rot[] = { 0, 2, 1, 3 };  // mapping to the bit weighting the hardware requires

  if (copy_from_user(buf, buf_user, count))
    return -EFAULT;

  mutex_lock(&rbled_state_lock);  // keep espi_driver_rb_leds_poll() from interfering
  force_update = 0;
  for (i = 0; (i + 1) < count; i += 2)
  {
    led_id    = buf[i];
    val       = rot[buf[i + 1] & 0x3];  // get mapped bit weighting
    led_index = (RIBBON_LED_STATES_SIZE - 1) - led_id / 4;
    rb_led_new_st[led_index] &= ~(0x3 << ((led_id % 4) * 2));         // clear the 2 bits for the selected LED
    rb_led_new_st[led_index] |= val << ((led_id % 4) * 2);            // add in the state bits
    force_update |= rb_led_new_st[led_index] ^ rb_led_st[led_index];  // mark any changes for update
  }
  mutex_unlock(&rbled_state_lock);

  status = count;
  return status;
}

// file operations structure
static const struct file_operations rbled_fops = {
  .owner  = THIS_MODULE,
  .write  = rbled_write,
  .open   = nonseekable_open,
  .llseek = no_llseek,
};

static struct class *rbled_class;

// ---- driver setup (init) function ----
s32 espi_driver_rb_leds_setup(struct espi_driver *sb)
{
  memset(rb_led_new_st, 0xFF, RIBBON_LED_STATES_SIZE);  // all LEDs ON
  force_update = 1;

  if (register_chrdev(ESPI_RIBBON_LED_DEV_MAJOR, "spi", &rbled_fops) < 0)
    pr_err("%s: problem at register_chrdev\n", __func__);

  rbled_class = class_create(THIS_MODULE, "ribbon-led");
  if (IS_ERR(rbled_class))
    pr_err("%s: unable to create class\n", __func__);

  device_create(rbled_class, sb->dev, MKDEV(ESPI_RIBBON_LED_DEV_MAJOR, 0), sb, "ribbon_led");

  return 0;
}

// ---- driver cleanup (de-init) function ----
s32 espi_driver_rb_leds_cleanup(struct espi_driver *sb)
{
  device_destroy(rbled_class, MKDEV(ESPI_RIBBON_LED_DEV_MAJOR, 0));
  class_destroy(rbled_class);
  unregister_chrdev(ESPI_RIBBON_LED_DEV_MAJOR, "spi");

  return 0;
}

// ---- send any newly changed states down to the LEDs ----
void espi_driver_rb_leds_poll(struct espi_driver *p)
{
  struct spi_transfer xfer;
  u8                  update;

  u64          now    = get_jiffies_64();
  u64          diff   = now - lastUpdate;
  unsigned int diffMS = jiffies_to_msecs(diff);

  extern int sck_hz;

  mutex_lock(&rbled_state_lock);  // keep rbled_write() from interfering
  if (force_update)
    memcpy(rb_led_st, rb_led_new_st, RIBBON_LED_STATES_SIZE);
  force_update |= (diffMS >= ESD_INTERVAL);  // periodic update due ("anti-ESD" update) ?
  update       = force_update;
  force_update = 0;
  mutex_unlock(&rbled_state_lock);

  if (update == 0)
    return;

  lastUpdate = now;

  xfer.tx_buf        = rb_led_st;
  xfer.rx_buf        = NULL;
  xfer.len           = RIBBON_LED_STATES_SIZE;
  xfer.bits_per_word = 8;
  xfer.delay_usecs   = 0;
  xfer.speed_hz      = sck_hz;

  espi_driver_scs_select(p, ESPI_RIBBON_LEDS_PORT, p->ribbon_leds_device);
  espi_driver_transfer(p->spidev, &xfer);
  gpio_set_value(p->gpio_sap, 0);
  gpio_set_value(p->gpio_sap, 1);
  espi_driver_scs_select(p, ESPI_RIBBON_LEDS_PORT, 0);
}

// EOF
