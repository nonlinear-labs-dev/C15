/******************************************************************************/
/** @file	spi_lpc_bb.c
    @date	last modified date/name: 2020-04-20 KSTR
    @brief    	This module takes care about the communication with the LPC via
    SPI.
    @author	Nemanja Nikodijevic [2014-03-02]
*******************************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/workqueue.h>
#include <linux/poll.h>
#include <linux/init.h>
#include <linux/spi/spi.h>
#include <linux/fs.h>       // Write data to sysfs
#include <linux/of_gpio.h>  // nni: added for device tree
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/ratelimit.h>

/* speed can only be f_max/(2^n) -> f_max = 24 MHz */
#define LPC_BB_DRIVER_MAX_SPEED    6000000  // was 3000000
#define LPC_BB_DRIVER_XFER_SIZE    1024
#define LPC_BB_DRIVER_DEV_MAJOR    300
#define LPC_BB_DRIVER_RX_BUFF_SIZE 1024  // must be 2^n, for buffer mask to work !
#define LPC_BB_DRIVER_RX_BUFF_MASK (LPC_BB_DRIVER_RX_BUFF_SIZE - 1)

#define SIGNATURE_HEADER (0xFF0000FF)  // signature of low level transfer, size field in the middle
#define SIGNATURE_BYTE   (0xFF)        // the signature's framing bytes
#define SIGNATURE_SIZE   (4)           // number of bytes for the signature

struct lpc_bb_driver
{
  struct delayed_work work;
  struct device *     dev;
  struct spi_device * spidev;

  int rdy_gpio;
  int prq_gpio;
  int lpc_hb_gpio;
  int lpc_spare_gpio;
  int cs_gpio;
};

// writes are performed using switching dual buffers
static DEFINE_MUTEX(tx_buff_lock);       // buffer protector
static u8 *     tx_buff;                 // [2][LPC_BB_DRIVER_XFER_SIZE]  current buffer is user write buffer, the other is the SPI xfer buffer
static u8       tx_write_buffer;         // selected write buffer, 0 or 1
static unsigned tx_write_buffer_offset;  // current write offset in selected buffer

// reads use a ring buffer
static u8 *rxb;                             // [LPC_BB_DRIVER_XFER_SIZE]   SPI RX buffer
static DEFINE_MUTEX(rx_buff_lock);          // buffer protector
static DECLARE_WAIT_QUEUE_HEAD(rx_wqueue);  // wait queue to control buffer dataflow
static u8 *rx_buff;                         // [LPC_BB_DRIVER_RX_BUFF_SIZE]   user RX ring buffer
static u16 rx_buff_head;
static u16 rx_buff_tail;

static u8 no_hb_count;       // number of LPC heartbeats currently missed
#define MAX_NO_HB_COUNT (3)  // number of missed LPC heartbeats allowed before error / write stall
static u8  busy;
static u8  xfer_repeat;  // flag repeat request (when LPC was offline)
static u8 *txb_repeat;   // saved tx buffer pointer for delayed xfer

static struct workqueue_struct *workqueue;
#define RESTART_WQUEUE_MS (2)  // restart queue after X milliseconds

// **************************************************************************
// check if LPC heartbeat pin toggles after 125us max
//
// We wait for nominally 700us max in nominally 35us steps.
// usleep_range() is not very precise, it might sleep significantly longer
// than the max value given.
// It's only guaranteed that it sleeps AT LEAST for the min time.
// That's we need to poll the heartbeat more frequently and for a longer period
// that required by the 125us toggle rate of the LPC (which is precise as it
// is controlled via timer interrupt), to account for the jitter and not
// report false negatives.
static int lpc_bb_driver_check_hb(int hb_gpio)
{
  int hb_val, i;

  hb_val = gpio_get_value(hb_gpio);
  for (i = 0; i < 20; i++)
  {
    usleep_range(30, 40);
    if (gpio_get_value(hb_gpio) != hb_val)
      return 1;
  }
  return 0;  // no change, LPC is busy/dead
}

// **************************************************************************
//  3 helper functions for rx buffer access
static inline u16 bytes_in_rx_buffer(void)
{  // the modulo operation on the difference is required because head may have wrapped around
  return (rx_buff_head - rx_buff_tail) & LPC_BB_DRIVER_RX_BUFF_MASK;
}

static inline void advance(u16 *index, u16 how_much)
{  // advance index by given amount, modulo buffer size
  (*index) += how_much;
  (*index) &= LPC_BB_DRIVER_RX_BUFF_MASK;
}

static inline int rx_buff_has_data(void)
{
  return rx_buff_head != rx_buff_tail;
}

// **************************************************************************
//  parse incoming package and transfer to rx buffer
static void lpc_bb_driver_package_parse(void)
{
  static int      blocked       = 0;  // flag blocked state (can't accept new packets)
  static unsigned dropped_count = 0;  // number of packets dropped while being blocked
  u16             rx_bytes_to_read;
  u16             first, second;

  if ((rxb[0] != SIGNATURE_BYTE) || (rxb[3] != SIGNATURE_BYTE))
    return;  // wrong signature
  rx_bytes_to_read = rxb[1] + (rxb[2] << 8);

  if (!rx_bytes_to_read || (rx_bytes_to_read > LPC_BB_DRIVER_RX_BUFF_SIZE - 4))
    return;  // no data, or message can't fit in buffer

  mutex_lock(&rx_buff_lock);
  if (rx_bytes_to_read < (LPC_BB_DRIVER_RX_BUFF_SIZE - bytes_in_rx_buffer()))
  {  // enough free space in buffer
    if (blocked)
    {  // wasn't blocked before so, shot a message and reset counter
      blocked = 0;
      printk_ratelimited(KERN_WARNING "%s() : ... missed %u packets. RX buffer now accepting packets again.\n", __func__, dropped_count);
      dropped_count = 0;
    }

    first  = rx_bytes_to_read;  // assume first chunk will fit ...
    second = 0;                 // .. and no need for second chunk
    if (rx_buff_head + rx_bytes_to_read > LPC_BB_DRIVER_RX_BUFF_SIZE)
    {  // write beyond physical buffer end, so split in two chunks
      first  = LPC_BB_DRIVER_RX_BUFF_SIZE - rx_buff_head;
      second = rx_bytes_to_read - first;
    }
    memcpy(&rx_buff[rx_buff_head], &rxb[SIGNATURE_SIZE], first);
    if (second)
      memcpy(&rx_buff[0], &rxb[SIGNATURE_SIZE + first], second);
    advance(&rx_buff_head, rx_bytes_to_read);

    wake_up_interruptible(&rx_wqueue);
  }
  else
  {                            // buffer is full, packet has to be dropped
    if (++dropped_count == 0)  // increment, ...
      dropped_count--;         // ... but saturate
    if (!blocked)
    {  // was NOT blocked before, so shot a message
      blocked = 1;
      printk_ratelimited(KERN_WARNING "%s() : RX buffer full, dropping packets...\n", __func__);
    }
  }
  mutex_unlock(&rx_buff_lock);
}

// **************************************************************************
static u8 *lpc_bb_driver_get_txb(void)
{
  u32 *pSignature;
  u8 * txb;

  /** request tx buffer */
  mutex_lock(&tx_buff_lock);
  txb                    = tx_buff + tx_write_buffer * LPC_BB_DRIVER_XFER_SIZE;  //[tx_write_buffer];
  tx_write_buffer        = (tx_write_buffer + 1) % 2;                            // switch buffers
  pSignature             = (u32 *) (tx_buff + tx_write_buffer * LPC_BB_DRIVER_XFER_SIZE);
  *pSignature            = SIGNATURE_HEADER;  // set up signature header, with size field = 0
  tx_write_buffer_offset = SIGNATURE_SIZE;    // acount for signature header
  mutex_unlock(&tx_buff_lock);

  return txb;
}

// **************************************************************************
static int lpc_bb_driver_transfer(struct spi_device *dev, u8 *txb)
{
  struct spi_transfer xfer;
  struct spi_message  msg;
  int                 status;

  /** prepare and perform the transfer */
  xfer.tx_buf        = txb;
  xfer.rx_buf        = rxb;
  xfer.len           = LPC_BB_DRIVER_XFER_SIZE;
  xfer.speed_hz      = LPC_BB_DRIVER_MAX_SPEED;
  xfer.bits_per_word = 8;
  xfer.delay_usecs   = 0;
  xfer.tx_nbits = xfer.rx_nbits = SPI_NBITS_SINGLE;

  spi_message_init(&msg);
  spi_message_add_tail(&xfer, &msg);

  status = spi_sync(dev, &msg);  // will sleep until tranfer is finished

  return status;
}

// **************************************************************************
static void lpc_bb_driver_poll(struct delayed_work *p)
{
  static int            lpc_offline = 0;
  struct lpc_bb_driver *spi         = (struct lpc_bb_driver *) p;
  int                   ready, poll_request, tx_payload, lpc_heartbeat;
  u8 *                  txb;

  //printk("Workqueue %i",  ((struct spilpcbb *)p)->counter++);
  ready         = gpio_get_value(spi->rdy_gpio);
  poll_request  = gpio_get_value(spi->prq_gpio);
  lpc_heartbeat = lpc_bb_driver_check_hb(spi->lpc_hb_gpio);
  tx_payload    = tx_write_buffer_offset - SIGNATURE_SIZE;

  if (!lpc_heartbeat)
  {
    if (no_hb_count == 0)
      printk_ratelimited(KERN_INFO "%s() LPC heartbeat missing\n", __func__);
    if (no_hb_count < MAX_NO_HB_COUNT)  // increment fail counter, ...
      no_hb_count++;                    // ...  but saturate
    goto exit;
  }

  if (no_hb_count)
    no_hb_count = 0;

  if (ready && (poll_request || tx_payload))
  {
    busy = 0;

    if (xfer_repeat)
      txb = txb_repeat;
    else
      txb = lpc_bb_driver_get_txb();

    gpio_set_value(spi->cs_gpio, 0);
    lpc_bb_driver_transfer(((struct lpc_bb_driver *) p)->spidev, txb);
    gpio_set_value(spi->cs_gpio, 1);

    lpc_heartbeat = lpc_bb_driver_check_hb(spi->lpc_hb_gpio);
    if (lpc_heartbeat)
    {  // LPC still online
      xfer_repeat = 0;
      lpc_bb_driver_package_parse();  // check any incoming data
    }
    else
    {  // LPC offline, re-schedule transfer
      xfer_repeat = 1;
      txb_repeat  = txb;
      printk_ratelimited(KERN_INFO "%s() LPC heartbeat missing, re-scheduling transfer\n", __func__);
    }
  }
  else if (!ready)
    busy = 1;

exit:
  if (no_hb_count >= MAX_NO_HB_COUNT)
  {  // we missed enough LPC heartbeats to consider LPC is offline
    if (!lpc_offline)
    {  // LPC went offline now
      lpc_offline = 1;
      printk_ratelimited(KERN_INFO "%s() LPC went offline\n", __func__);
    }
  }
  else
  {
    if (lpc_offline)
    {  // LPC went online again
      lpc_offline = 0;
      printk_ratelimited(KERN_INFO "%s() LPC went online\n", __func__);
    }
  }

  queue_delayed_work(workqueue, p, msecs_to_jiffies(RESTART_WQUEUE_MS));
}

// **************************************************************************
//  write user data to tx buffer
static ssize_t lpc_bb_driver_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
  ssize_t status = 0;
  u32 *   pSignature;

  // if the communication is dead, return IO-Error
  if (no_hb_count >= MAX_NO_HB_COUNT)
    return -EIO;

  // if tx buffer is full, return
  if ((tx_write_buffer_offset + count) > LPC_BB_DRIVER_XFER_SIZE)
  {
    if (busy)
      return -EBUSY;
    // NOTE: !! if a transmit with count > LPC_BB_DRIVER_XFER_SIZE is attempted, user code will loop with -EAGAIN forever !!
    return -EAGAIN;
  }

  mutex_lock(&tx_buff_lock);

  status = copy_from_user((tx_buff + tx_write_buffer * LPC_BB_DRIVER_XFER_SIZE + tx_write_buffer_offset), buf, count);
  if (status == 0)
  {  // copy succeeded
    tx_write_buffer_offset += count;
    pSignature = (u32 *) (tx_buff + tx_write_buffer * LPC_BB_DRIVER_XFER_SIZE);
    *pSignature += count << 8;  // update size field in signature header
    status = count;
  }
  else
    status = -EFAULT;

  mutex_unlock(&tx_buff_lock);

  return status;
}

// **************************************************************************
//  read rx buffer and transfer to user data
static ssize_t lpc_bb_driver_read(struct file *filp, char __user *buf_user, size_t count, loff_t *f_pos)
{
  ssize_t status;
  u16     xfer, first, second;

  // if in non-blocking mode and no data to read, return
  if ((filp->f_flags & O_NONBLOCK) && !rx_buff_has_data())
    return -EAGAIN;

  // sleep until there is data to read or interrupt
  if ((status = wait_event_interruptible(rx_wqueue, rx_buff_has_data())))
  {
    if (status == -ERESTARTSYS)
    {  // wait() resumed because of an interrupt signal,
      // so clear remaning buffer contents to avoid picking up unsynced messages next time
      printk_ratelimited(KERN_WARNING "%s() : interrupted during read, clearing remaining buffer\n", __func__);
      mutex_lock(&rx_buff_lock);
      rx_buff_tail = rx_buff_head;
      mutex_unlock(&rx_buff_lock);
    }
    return status;
  }
  // wait resumed with real data

  mutex_lock(&rx_buff_lock);

  xfer = bytes_in_rx_buffer();
  if (count > xfer)  // more requested than available ?
    count = xfer;    //  then shrink to available

  first  = count;  // assume first chunk will fit ...
  second = 0;      // .. and no need for second chunk
  if (rx_buff_tail + count > LPC_BB_DRIVER_RX_BUFF_SIZE)
  {  // read beyond physical buffer end, so split in two chunks
    first  = LPC_BB_DRIVER_RX_BUFF_SIZE - rx_buff_tail;
    second = count - first;
  }
  if (copy_to_user(buf_user, &rx_buff[rx_buff_tail], first) != 0)
    status = -EFAULT;
  else if (second)
    if (copy_to_user(buf_user + first, &rx_buff[0], second) != 0)
      status = -EFAULT;
  advance(&rx_buff_tail, count);

  mutex_unlock(&rx_buff_lock);

  if (status)  // error ?
    return status;

  return (count);
}
/*  
*/

// ---- driver poll function ----
// checks and returns "read data available" and "write allowed"
static unsigned int lpc_bb_driver_fops_poll(struct file *filp, poll_table *wait)
{
  unsigned int mask = 0;  // assume nothing first
  poll_wait(filp, &rx_wqueue, wait);

  // if there is data in buffer, reading is allowed
  if (rx_buff_tail != rx_buff_head)
    mask |= POLLIN | POLLRDNORM;
  return mask;
}

static const struct file_operations lpc_bb_driver_fops = {
  .owner  = THIS_MODULE,
  .write  = lpc_bb_driver_write,
  .read   = lpc_bb_driver_read,
  .open   = nonseekable_open,
  .llseek = no_llseek,
  .poll   = lpc_bb_driver_fops_poll,
};

static struct class *lpc_bb_driver_class;

// **************************************************************************
static int lpc_bb_driver_probe(struct spi_device *dev)
{
  int                   ret = 0;
  struct lpc_bb_driver *sb;
  struct device_node *  dn = dev->dev.of_node;
  struct device *       tmpd;
  u32 *                 pSignature;

  rxb = tx_buff = rx_buff = NULL;

  sb = devm_kzalloc(&dev->dev, sizeof(struct lpc_bb_driver), GFP_KERNEL);
  if (!sb)
  {
    dev_err(&dev->dev, "%s: unable to devm_kzalloc for spi-lpc-bb\n", __func__);
    ret = -ENOMEM;
    goto exit;  // sb is managed malloc, no kfree() needed when failed
  }

  rxb = kcalloc(LPC_BB_DRIVER_XFER_SIZE, sizeof(u8), GFP_KERNEL);
  if (!rxb)
  {
    dev_err(&dev->dev, "%s: unable to kcalloc for rxb\n", __func__);
    ret = -ENOMEM;
    goto exit;
  }

  tx_buff = kcalloc(2 * LPC_BB_DRIVER_XFER_SIZE, sizeof(u8), GFP_KERNEL);
  if (!tx_buff)
  {
    dev_err(&dev->dev, "%s: unable to kcalloc for txb\n", __func__);
    ret = -ENOMEM;
    goto exit;
  }

  rx_buff = kcalloc(LPC_BB_DRIVER_RX_BUFF_SIZE, sizeof(u8), GFP_KERNEL);
  if (!rx_buff)
  {
    dev_err(&dev->dev, "%s: unable to kcalloc for rx_buff\n", __func__);
    ret = -ENOMEM;
    goto exit;
  }

  /** check DT entries and request gpios */
  sb->rdy_gpio = of_get_named_gpio(dn, "rdy-gpio", 0);
  if (!gpio_is_valid(sb->rdy_gpio))
  {
    dev_err(&dev->dev, "%s: rdy-gpio in the device tree incorrect\n", __func__);
    ret = -EINVAL;
    goto exit;
  }
  sb->prq_gpio = of_get_named_gpio(dn, "prq-gpio", 0);
  if (!gpio_is_valid(sb->prq_gpio))
  {
    dev_err(&dev->dev, "%s: prq-gpio in the device tree incorrect\n", __func__);
    ret = -EINVAL;
    goto exit;
  }
  sb->lpc_hb_gpio = of_get_named_gpio(dn, "lpc-hb-gpio", 0);
  if (!gpio_is_valid(sb->lpc_hb_gpio))
  {
    dev_err(&dev->dev, "%s: lpc-hb-gpio in the device tree incorrect\n", __func__);
    ret = -EINVAL;
    goto exit;
  }
  sb->lpc_spare_gpio = of_get_named_gpio(dn, "lpc-spare-gpio", 0);
  if (!gpio_is_valid(sb->lpc_spare_gpio))
  {
    dev_err(&dev->dev, "%s: lpc-spare-gpio in the device tree incorrect\n", __func__);
    ret = -EINVAL;
    goto exit;
  }
  sb->cs_gpio = of_get_named_gpio(dn, "lpc-cs-gpio", 0);
  if (!gpio_is_valid(sb->cs_gpio))
  {
    dev_err(&dev->dev, "%s: lpc-cs-gpio in the device tree incorrect\n", __func__);
    ret = -EINVAL;
    goto exit;
  }

  if ((ret = devm_gpio_request_one(&dev->dev, sb->rdy_gpio, GPIOF_IN, "rdy_gpio")))
    goto exit;
  if ((ret = devm_gpio_request_one(&dev->dev, sb->prq_gpio, GPIOF_IN, "prq_gpio")))
    goto exit;
  if ((ret = devm_gpio_request_one(&dev->dev, sb->lpc_hb_gpio, GPIOF_IN, "lpc_hb_gpio")))
    goto exit;
  if ((ret = devm_gpio_request_one(&dev->dev, sb->lpc_spare_gpio, GPIOF_OUT_INIT_HIGH, "lpc_spare_gpio")))
    goto exit;
  if ((ret = devm_gpio_request_one(&dev->dev, sb->cs_gpio, GPIOF_OUT_INIT_HIGH, "cs_gpio")))
    goto exit;

  /** prepare the spi controller */
  dev->mode          = SPI_MODE_1;
  dev->max_speed_hz  = LPC_BB_DRIVER_MAX_SPEED;
  dev->bits_per_word = 8;
  if (spi_setup(dev))
  {
    dev_err(&dev->dev, "%s: spi setup incorrect\n", __func__);
    ret = -EINVAL;
    goto exit;
  }

  /** create the device */
  tmpd                   = device_create(lpc_bb_driver_class, &dev->dev, MKDEV(LPC_BB_DRIVER_DEV_MAJOR, 0), sb, "lpc_bb_driver");
  tx_write_buffer        = 0;
  tx_write_buffer_offset = 4;
  pSignature             = (u32 *) tx_buff;
  *pSignature            = SIGNATURE_HEADER;
  rx_buff_head           = 0;
  rx_buff_tail           = 0;
  no_hb_count            = 0;

  sb->spidev = dev;
  sb->dev    = &dev->dev;
  dev_set_drvdata(&dev->dev, (void *) sb);
  dev_info(&dev->dev, "spi registered, item=0x%p\n", (void *) sb);

  INIT_DELAYED_WORK(&(sb->work), (work_func_t) lpc_bb_driver_poll);
  queue_delayed_work(workqueue, &(sb->work), msecs_to_jiffies(RESTART_WQUEUE_MS));  // jiffies / HZ -> ms, so 4ms

exit:
  if (ret)
  {  // some error happened
     // sb is managed malloc, no kfree() needed when failed
    if (rxb)
      kfree(rxb);
    if (tx_buff)
      kfree(tx_buff);
    if (rx_buff)
      kfree(rx_buff);
  }
  return ret;
}

// **************************************************************************
static int lpc_bb_driver_remove(struct spi_device *spi)
{
  struct lpc_bb_driver *sb = (struct lpc_bb_driver *) dev_get_drvdata(&spi->dev);

  cancel_delayed_work(&(sb->work));

  device_destroy(lpc_bb_driver_class, MKDEV(LPC_BB_DRIVER_DEV_MAJOR, 0));
  class_destroy(lpc_bb_driver_class);
  unregister_chrdev(LPC_BB_DRIVER_DEV_MAJOR, "spi");

  kfree(rxb);
  kfree(rx_buff);
  kfree(tx_buff);

  return 0;
}

// **************************************************************************
static struct spi_driver lpc_bb_driver_driver = {
  .driver = {
      .name  = "lpc_bb_driver",
      .bus   = &spi_bus_type,
      .owner = THIS_MODULE,
  },
  .probe  = lpc_bb_driver_probe,
  .remove = lpc_bb_driver_remove,
};

// **************************************************************************
static int __init lpc_bb_driver_init(void)
{
  int ret;

  printk(KERN_INFO "%s() VERSION: 2020-04-20\n", __func__);

  ret = register_chrdev(LPC_BB_DRIVER_DEV_MAJOR, "spi", &lpc_bb_driver_fops);
  if (ret < 0)
    pr_err("%s: problem at register_chrdev\n", __func__);

  lpc_bb_driver_class = class_create(THIS_MODULE, "lpc_bb_driver");
  if (IS_ERR(lpc_bb_driver_class))
    pr_err("%s: unable to create class\n", __func__);

  workqueue = create_workqueue("lpc_bb_driver_queue");
  if (workqueue == NULL)
  {
    pr_err("%s: unable to create workqueue\n", __func__);
    return -1;
  }

  ret = spi_register_driver(&lpc_bb_driver_driver);
  if (ret)
    pr_err("%s: problem at spi_register_driver\n", __func__);

  busy        = 0;
  no_hb_count = 0;
  xfer_repeat = 0;

  return ret;
}
module_init(lpc_bb_driver_init);

// **************************************************************************
static void __exit lpc_bb_driver_exit(void)
{
  spi_unregister_driver(&lpc_bb_driver_driver);
}
module_exit(lpc_bb_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nemanja Nikodijevic/Klaus Strohhaecker");
MODULE_DESCRIPTION("lpc_bb_driver");
MODULE_VERSION("2020-04-20");
