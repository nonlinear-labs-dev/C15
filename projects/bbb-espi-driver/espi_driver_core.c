/*
 * espi_driver.c - ESPI kernel module for the Emphase
 */

#pragma GCC diagnostic ignored "-Wdate-time"

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/workqueue.h>
#include <linux/init.h>
#include <linux/spi/spi.h>
#include <linux/fs.h>       // Write data to sysfs
#include <linux/of_gpio.h>  // nni: added for device tree
#include <linux/delay.h>    // nni: added for msleep()
#include <linux/platform_device.h>
#include <linux/vmalloc.h>

#include <linux/semaphore.h>
#include <linux/wait.h>
#include <linux/poll.h>

#include <linux/time.h>
#include <linux/hrtimer.h>
#include <linux/hardirq.h>

#include "espi_driver.h"

#define WORKQUEUE_DELAY_MS 2  // orig. code used 8 ms

int sck_hz = 1000000;

static struct workqueue_struct *workqueue;

/*******************************************************************************
    eSPI general driver functions
    @param[in]  port: 0..7
    @param[in]  device: 1..3
                        0 -> all off
*******************************************************************************/
void espi_driver_scs_select(struct espi_driver *spi, s32 port, s32 device)
{
  s32 s;
  u8  i;

  gpio_set_value(spi->gpio_dmx, spi->espi_gpio_dmx_default);  // dmxs disable: avoid glitches

  if (device == 1 || device == 3)
    s = 0;
  else if (device == 2)
    s = 3;
  else if (device == 0)  // device 0: all off
  {
    s      = 0;
    device = 3;
    port   = 7;  // disable all - select unused port
  }
  else
  {
    gpio_set_value(spi->gpio_dmx, !spi->espi_gpio_dmx_default);
    return;
  }

  do
  {
    for (i = 0; i < 3; i++)
    {
      if (port & (1 << i))
        gpio_set_value(spi->gpio_scs[i + s], 1);
      else
        gpio_set_value(spi->gpio_scs[i + s], 0);
    }
    s += 3;
  } while ((s <= 3) && (device == 3));

  gpio_set_value(spi->gpio_dmx, !spi->espi_gpio_dmx_default);  // dmxs enable
}

s32 espi_driver_transfer(struct spi_device *dev, struct spi_transfer *xfer)
{
  struct spi_message msg;
  s32                status;

  xfer->tx_nbits = xfer->rx_nbits = SPI_NBITS_SINGLE;

  spi_message_init(&msg);
  spi_message_add_tail(xfer, &msg);

  status = spi_sync(dev, &msg);

  return status;
}

s32 espi_driver_set_mode(struct spi_device *dev, u16 mode)
{
  s32                 status;
  struct spi_transfer xfer;
  u8                  tx = 0;

  extern int sck_hz;

  xfer.tx_buf        = &tx;
  xfer.rx_buf        = NULL;
  xfer.len           = 1;
  xfer.bits_per_word = 8;
  xfer.delay_usecs   = 0;
  xfer.speed_hz      = sck_hz;

  dev->mode = mode;
  status    = spi_setup(dev);

  espi_driver_transfer(dev, &xfer);
  return status;
}

static void espi_driver_poll(struct delayed_work *p)
{
  queue_delayed_work(workqueue, p, msecs_to_jiffies(WORKQUEUE_DELAY_MS));

  switch (((struct espi_driver *) p)->poll_stage)
  {
    case 0:
    case 2:
    case 4:
    case 6:
      espi_driver_encoder_poll((struct espi_driver *) p);
      espi_driver_pollbuttons((struct espi_driver *) p);
      break;
    case 1:
    case 5:
      espi_driver_leds_poll((struct espi_driver *) p);
      espi_driver_rb_leds_poll((struct espi_driver *) p);
      break;
    case 3:
      espi_driver_ssd1305_poll((struct espi_driver *) p);
      break;
    case 7:
      espi_driver_ssd1322_poll((struct espi_driver *) p);
      break;
  }

  ((struct espi_driver *) p)->poll_stage = (((struct espi_driver *) p)->poll_stage + 1) % 8;
}

/*******************************************************************************
    eSPI driver functions
*******************************************************************************/
static s32 espi_driver_probe(struct spi_device *dev)
{
  s32                 nscs, i, ret = 0;
  struct espi_driver *sb;
  struct device_node *dn = dev->dev.of_node;

  sb = devm_kzalloc(&dev->dev, sizeof(struct espi_driver), GFP_KERNEL);
  if (!sb)
  {
    dev_err(&dev->dev, "%s: unable to kzalloc for espi_driver\n", __func__);
    return -ENOMEM;
  }

  /****************************************** added by nni  { */

  /** check DT entries */
  nscs = of_gpio_named_count(dn, "scs-gpios");
  if (nscs != ESPI_SCS_NUM || nscs == -ENOENT)
  {
    dev_err(&dev->dev, "%s: number of scs-gpios in the device tree incorrect\n", __func__);
    return -ENOENT;
  }
  /** fetch gpio numbers */
  for (i = 0; i < ESPI_SCS_NUM; i++)
  {
    sb->gpio_scs[i] = of_get_named_gpio(dn, "scs-gpios", i);
    if (!gpio_is_valid(sb->gpio_scs[i]))
    {
      dev_err(&dev->dev, "%s: scs-gpios[%d] in the device tree incorrect\n", __func__, i);
      return -EINVAL;
    }
  }
  sb->gpio_sap = of_get_named_gpio(dn, "sap-gpio", 0);
  if (!gpio_is_valid(sb->gpio_sap))
  {
    dev_err(&dev->dev, "%s: sap-gpio in the device tree incorrect\n", __func__);
    return -EINVAL;
  }
  sb->gpio_dmx = of_get_named_gpio(dn, "dmxs-gpio", 0);
  if (!gpio_is_valid(sb->gpio_dmx))
  {
    dev_err(&dev->dev, "%s: dmxs-gpio in the device tree incorrect\n", __func__);
    return -EINVAL;
  }
  /** request gpios */
  for (i = 0; i < ESPI_SCS_NUM; i++)
  {
    ret = devm_gpio_request_one(&dev->dev, sb->gpio_scs[i], GPIOF_OUT_INIT_LOW, "gpio_scs");
    if (ret)
    {
      dev_err(&dev->dev, "%s: failed to request gpio: %d\n", __func__, sb->gpio_scs[i]);
      return ret;
    }
  }
  ret = devm_gpio_request_one(&dev->dev, sb->gpio_sap, GPIOF_OUT_INIT_HIGH, "gpio_sap");
  ret = devm_gpio_request_one(&dev->dev, sb->gpio_dmx, GPIOF_OUT_INIT_HIGH, "gpio_dmx");

  sb->play_buttons_device   = 3;
  sb->ribbon_leds_device    = 1;
  sb->espi_gpio_dmx_default = 0;

  sb->spidev = dev;
  sb->dev    = &dev->dev;
  dev_set_drvdata(&dev->dev, (void *) sb);
  dev_info(&dev->dev, "spi registered, item=0x%p\n", (void *) sb);

  espi_driver_scs_select(sb, ESPI_PLAY_PANEL_PORT, 0);

  sb->poll_stage = 0;

  espi_driver_buttons_setup(sb);
  espi_driver_leds_setup(sb);
  espi_driver_rb_leds_setup(sb);
  espi_driver_oleds_fb_setup(sb);
  espi_driver_encoder_setup(sb);

  INIT_DELAYED_WORK(&(sb->work), (work_func_t) espi_driver_poll);
  queue_delayed_work(workqueue, &(sb->work), msecs_to_jiffies(WORKQUEUE_DELAY_MS));

  dev_info(&dev->dev, "espi driver: \n");
  dev_info(&dev->dev, "  sck_hz=%i Hz\n", sck_hz);
  dev_info(&dev->dev, "  espi_hw_ref=%s\n", sb->hw_rev_str);
  dev_info(&dev->dev, "  build: %s - %s \n", __DATE__, __TIME__);

  return ret;
}

static s32 espi_driver_remove(struct spi_device *spi)
{
  struct espi_driver *sb = (struct espi_driver *) dev_get_drvdata(&spi->dev);

  printk("espi_driver_remove\n");

  cancel_delayed_work(&(sb->work));

  espi_driver_encoder_cleanup(sb);
  espi_driver_oleds_fb_cleanup(sb);
  espi_driver_rb_leds_cleanup(sb);
  espi_driver_leds_cleanup(sb);
  espi_driver_buttons_cleanup(sb);

  return 0;
}

static struct spi_driver espi_driver_driver = {
  .driver = {
      .name  = "espi_driver",
      .bus   = &spi_bus_type,
      .owner = THIS_MODULE,
  },
  .probe  = espi_driver_probe,
  .remove = espi_driver_remove,
};

module_param(sck_hz, int, 0644);
MODULE_PARM_DESC(sck_hz, "Speed in Hz of the eSPI bus");

static s32 __init espi_driver_init(void)
{
  s32 ret;

  workqueue = create_workqueue("espi_driver queue");
  if (workqueue == NULL)
  {
    pr_err("%s: unable to create workqueue\n", __func__);
    return -1;
  }

  ret = spi_register_driver(&espi_driver_driver);
  if (ret)
    pr_err("%s: problem at spi_register_driver\n", __func__);

  return ret;
}
module_init(espi_driver_init);

// **************************************************************************
static void __exit espi_driver_exit(void)
{
  printk("espi_driver_exit --\n");

  spi_unregister_driver(&espi_driver_driver);
}
module_exit(espi_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nemanja Nikodijevic");
MODULE_DESCRIPTION("espi_driver");
MODULE_VERSION(__DATE__ " " __TIME__ " SHA1=" NL_VERSION);
