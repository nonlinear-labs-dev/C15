/* 
   ESPI Driver for Buttons & ID lines on Panel Unit and Play Panel
   New features :
     - All physical lines are read out, to allow for readout of 'unused' button lines.
     - "Poll Button" function added, that is, the driver can be requested to read out a button line
       and put the result in the output buffer, the button line doesn't have to change physically
       to generate an event.
       Purpose, possible use cases : 
         * Detect if the hardware is present (via a special button ID 0x77 for the pulling function),
           and which version of it (per Version Code read from the ID lines).
         * Detect if any buttons are stuck, at startup
         * Allow for specific button press pattern detected at startup, initiating
           special features or diagnostic functions
     - "Emulate Button" function added, press and release events can be generated.
       Might be usuful for diagnostic or special features.
           

Button ID layout (hex)

Panel Unit :
  Select.Panel #1  |  Select.Panel #2  |     Edit Panel    |  Select.Panel #3  |  Select.Panel #4
 00 04 08 0C 10 14 | 18 1C 20 24 28 2C | 60             67 | 30 34 38 3C 40 44 | 48 4C 50 54 58 5C
 01 05 09 0D 11 15 | 19 1D 21 25 29 2D | 61 62 63 64 65 66 | 31 35 39 3D 41 45 | 49 4D 51 55 59 5D
 02 06 0A 0E 12 16 | 1A 1E 22 26 2A 2E | 68 69   72  6A 6B | 32 36 3A 3E 42 46 | 4A 4E 52 56 5A 5E
 03 07 0B 0F 13 17 | 1B 1F 23 27 2B 2F | 6C 6D 6E 6F 70 71 | 33 37 3B 3F 43 47 | 4B 4F 53 57 5B 5F
Note: Button ID 72 is for the encoder push-button, on the Edit Panel. Only for hardware >=V7.1

Play Panel :
 78 79     7A 7B


-------------------
Reading the ID from the driver yields:
- Button ID        (bit 7 cleared) ==> button released
- Button ID | 0x80 (bit 7 set)     ==> button pressed


Edit Panel Hardware IDs / Version Code lines  : 73..76
ID  detached     < V7.1    >= 7.1
73  released     released  released
74  released     released  released
75  released     released  pressed
76  released     released  pressed

Play Panel Hardware IDs / Version Code lines  : 7C..7F
ID  detached     < V7.1    >= 7.1
7C  released     released  released
7D  released     released  released
7E  released     released  pressed
7F  released     released  pressed
Unfortunately, a detached Panel Unit gives the same readout as a Panel Unit
of Hardware Version below 7.1. Detach of a >=V7.1 Panel is detectable using
those lines, though.

===>  Therefore there is a special ID 0x77 which can only be accessed by polling,
and which is discarded in the normal detection process.
There is NO way to access the physical line associated to ID 77, which is not
a regular button line anway and hardwired on the PCB (with the same setup as
used for ID 76, see above).

When polling ID 77 :
77  released ==> Panel Unit NOT connected
77  pressed  ==> Panel Unit connected
The input to this function is generated in the Encoder driver "espi_lpc8xx.c"
    
    
-------------------
"Poll Button" Function :
Write the button ID to the driver and read the returned current state of the button from the driver.
Bit 7 of the ID must be cleared !

    
-------------------
"Emulate Button" Function :
Two bytes must be written to the driver in one go.
First, write the button ID to the driver, with bit 7 set. Then write a flag byte, again bit 7 must be set!
The remaining bits of the flag byte are used to determine whether a "release" event is generated (all bits cleared)
or a "pressed" event is generated (any bit set).
Incomplete writes, and writes with the flag byte not having bit 7 set, are discarded.
This avoids command parsing going out of sync.

*/

#include <linux/poll.h>
#include <linux/of_gpio.h>
#include "espi_driver.h"

#define PANEL_UNIT_CONNECTION_ID (0x77)  // the last bit from edit panel ID bits, which is ignored and only used for panel detection by polling

// button states bit field
#define NUMBER_OF_BUTTONS_PER_GENERAL_PANEL  (24)
#define NUMBER_OF_GENERAL_PANELS             (4)
#define NUMBER_OF_BUTTONS_ALL_GENERAL_PANELS (NUMBER_OF_BUTTONS_PER_GENERAL_PANEL * NUMBER_OF_GENERAL_PANELS)
#define NUMBER_OF_BUTTONS_CENTRAL_PANEL      (18 + 1 + 5)  // 18 Buttons, 1 Encoder, 5 ID Lines
#define NUMBER_OF_BUTTONS_SOLED_PANEL        (4 + 4)       // 4 Buttons, 4 ID Lines

#define BUTTON_BYTES_GENERAL_PANELS ((u16)((NUMBER_OF_BUTTONS_ALL_GENERAL_PANELS + 7) / 8))  // number of bytes needed
#define BUTTON_BYTES_CENTRAL_PANEL  ((u16)((NUMBER_OF_BUTTONS_CENTRAL_PANEL + 7) / 8))       // number of bytes needed
#define BUTTON_BYTES_SOLED_PANEL    ((u16)((NUMBER_OF_BUTTONS_SOLED_PANEL + 7) / 8))         // number of bytes needed

#define BUTTON_STATES_SIZE (BUTTON_BYTES_GENERAL_PANELS + BUTTON_BYTES_CENTRAL_PANEL + BUTTON_BYTES_SOLED_PANEL)
static u8 *button_states;       // bit field holding the last buttons states
static u8 *prev_button_states;  // bit field holding the previous last buttons states

// button id output buffer
#define BUTTON_BUFFER_SIZE 256
static u8 *button_buff;
static u32 btn_buff_head, btn_buff_tail;
static DEFINE_MUTEX(btn_buff_lock);
static DECLARE_WAIT_QUEUE_HEAD(btn_wqueue);

// button id poll requests buffer
#define BUTTON_POLL_BUFFER_SIZE 256
static u8 *button_poll_buff;
static u32 btn_poll_buff_head, btn_poll_buff_tail;
static DEFINE_MUTEX(btn_poll_buff_lock);

// ---- write a button id to the output (read) buffer ----
static void write_id_to_outputbuffer(u8 id)
{
  mutex_lock(&btn_buff_lock);  // keep buttons_fops_read() and espi_driver_pollbuttons() from interfering
  if (((btn_buff_head + 1) % BUTTON_BUFFER_SIZE) != btn_buff_tail)
  {  // space left in output buffer ==> write id into buffer
    button_buff[btn_buff_head] = id;
    btn_buff_head              = (btn_buff_head + 1) % BUTTON_BUFFER_SIZE;
    wake_up_interruptible(&btn_wqueue);
  }
  mutex_unlock(&btn_buff_lock);
  //printk("button state = %x\n", btn_id);
}

// ---- read from driver function ----
// reads the button states on either change event or poll request
static ssize_t buttons_fops_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
  ssize_t status;
  u8      tmp;

  // if in non-blocking mode and no data to read then return "try again"
  if (filp->f_flags & O_NONBLOCK && btn_buff_head == btn_buff_tail)
    return -EAGAIN;

  // sleep until there is data to read
  if ((status = wait_event_interruptible(btn_wqueue, btn_buff_head != btn_buff_tail)))
    return status;

  mutex_lock(&btn_buff_lock);                         // keep espi_driver_pollbuttons() from interfering
  tmp           = button_buff[btn_buff_tail] ^ 0x80;  // invert bit 7, so we get 1 on btn down and 0 on btn up
  btn_buff_tail = (btn_buff_tail + 1) % BUTTON_BUFFER_SIZE;
  mutex_unlock(&btn_buff_lock);

  if (copy_to_user(buf, &tmp, 1) != 0)
    return -EFAULT;
  return 1;  // return one button id at a time
}

// ---- write to driver function ----
// writes poll request or button emulation data
static ssize_t buttons_fops_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
  u32 i;
  u8  tmp[count];

  if (copy_from_user(tmp, buf, count))
    return -EFAULT;

  for (i = 0; i < count; i++)
  {
    if (!(tmp[i] & 0x80))  // "poll button" command (bit 7 is cleared) ?
    {
      mutex_lock(&btn_poll_buff_lock);  // keep espi_driver_pollbuttons() from interfering
      if (((btn_poll_buff_head + 1) % BUTTON_POLL_BUFFER_SIZE) != btn_poll_buff_tail)
      {  // space in poll request buffer --> write poll request
        button_poll_buff[btn_poll_buff_head] = tmp[i];
        btn_poll_buff_head                   = (btn_poll_buff_head + 1) % BUTTON_POLL_BUFFER_SIZE;
      }
      mutex_unlock(&btn_poll_buff_lock);
      continue;
    }
    // now we have "emulate button" command (bit 7 is set)
    if (i + 1 < count)  // a second byte is available ?
    {
      if (tmp[i + 1] & 0x80)                 // if second byte also has bit 7 set
      {                                      //   then put ID in output buffer
        if (tmp[i + 1] & 0x7F)               // emulate a "pressed" button ?
          write_id_to_outputbuffer(tmp[i]);  //  then set ID with bit 7 high (which it is already)
        else
          write_id_to_outputbuffer(tmp[i] & 0x7F);  // else clear bit 7, marking "released")
      }
      i++;       // advance input buffer
      continue;  // and goto next command
    }
    break;  // no second byte left, ignore command and quit
  }

  return count;
}

// ---- driver poll function ----
// checks and returns "read data available" and "write allowed"
static unsigned int buttons_fops_poll(struct file *filp, poll_table *wait)
{

  unsigned int mask = 0;  // assume nothing first

  poll_wait(filp, &btn_wqueue, wait);

  // if there is data in buffer, reading is allowed
  if (btn_buff_tail != btn_buff_head)
    mask |= POLLIN | POLLRDNORM;

  // if there is space in the poll buffer, writing is allowed
  if (((btn_poll_buff_head + 1) % BUTTON_POLL_BUFFER_SIZE) != btn_poll_buff_tail)
    mask |= POLLOUT | POLLWRNORM;

  return mask;
}

// file operation structure
static const struct file_operations buttons_fops = {
  .owner  = THIS_MODULE,
  .read   = buttons_fops_read,   // read button id
  .write  = buttons_fops_write,  // write button poll request id
  .open   = nonseekable_open,
  .llseek = no_llseek,
  .poll   = buttons_fops_poll,
};

static struct class *buttons_class;

// ---- driver setup (init) function ----
s32 espi_driver_buttons_setup(struct espi_driver *sb)
{
  s32 ret;

  // button states bit fields, will be initialized with data on first passes of espi_driver_pollbuttons()
  button_states = kcalloc(BUTTON_STATES_SIZE, sizeof(u8), GFP_KERNEL);
  if (!button_states)
    return -ENOMEM;
  prev_button_states = kcalloc(BUTTON_STATES_SIZE, sizeof(u8), GFP_KERNEL);
  if (!prev_button_states)
    return -ENOMEM;

  // button id output buffer
  button_buff = kcalloc(BUTTON_BUFFER_SIZE, sizeof(u8), GFP_KERNEL);
  if (!button_buff)
    return -ENOMEM;
  btn_buff_head = btn_buff_tail = 0;

  // button id poll request input buffer
  button_poll_buff = kcalloc(BUTTON_POLL_BUFFER_SIZE, sizeof(u8), GFP_KERNEL);
  if (!button_poll_buff)
    return -ENOMEM;
  btn_poll_buff_head = btn_poll_buff_tail = 0;

  ret = register_chrdev(ESPI_BUTTON_DEV_MAJOR, "spi", &buttons_fops);
  if (ret < 0)
    pr_err("%s: problem at register_chrdev\n", __func__);

  buttons_class = class_create(THIS_MODULE, "espi-button");
  if (IS_ERR(buttons_class))
    pr_err("%s: unable to create class\n", __func__);

  device_create(buttons_class, sb->dev, MKDEV(ESPI_BUTTON_DEV_MAJOR, 0), sb, "espi_buttons");

  return 0;
}

// ---- driver cleanup (de-init) function ----
s32 espi_driver_buttons_cleanup(struct espi_driver *sb)
{
  kfree(button_states);
  kfree(prev_button_states);
  kfree(button_buff);
  kfree(button_poll_buff);

  device_destroy(buttons_class, MKDEV(ESPI_BUTTON_DEV_MAJOR, 0));
  class_destroy(buttons_class);
  unregister_chrdev(ESPI_BUTTON_DEV_MAJOR, "spi");

  return 0;
}

// ---- poll the physical IO lines, and also process poll requests ----
// generates button events when a physical button line changes
// but also generates these events when polling a specific button was requested
void espi_driver_pollbuttons(struct espi_driver *p)
{
  struct spi_transfer xfer;
  u8                  rx[BUTTON_STATES_SIZE];  // holds the current state of the button lines
  u8                  i, j, changed_bits, bit_pos, btn_id;

  extern int sck_hz;

  static int initial_passes = 2;  // flag first two passes for initialization instead of change detection

  xfer.tx_buf        = NULL;
  xfer.rx_buf        = rx;
  xfer.len           = BUTTON_BYTES_GENERAL_PANELS;
  xfer.bits_per_word = 8;
  xfer.delay_usecs   = 0;
  xfer.speed_hz      = sck_hz;

  espi_driver_set_mode(((struct espi_driver *) p)->spidev, SPI_MODE_3);

  // read general panels ("selection panels")
  espi_driver_scs_select(p, ESPI_SELECTION_PANEL_PORT, ESPI_SELECTION_BUTTONS_DEVICE);
  gpio_set_value(p->gpio_sap, 0);
  gpio_set_value(p->gpio_sap, 1);
  espi_driver_transfer(p->spidev, &xfer);
  espi_driver_scs_select(p, ESPI_SELECTION_PANEL_PORT, 0);

  // read central (big oled) panel ("edit panel")
  xfer.tx_buf = NULL;
  xfer.rx_buf = rx + BUTTON_BYTES_GENERAL_PANELS;
  xfer.len    = BUTTON_BYTES_CENTRAL_PANEL;
  espi_driver_scs_select(p, ESPI_EDIT_PANEL_PORT, ESPI_EDIT_BUTTONS_DEVICE);
  gpio_set_value(p->gpio_sap, 0);
  gpio_set_value(p->gpio_sap, 1);
  espi_driver_transfer(p->spidev, &xfer);
  espi_driver_scs_select(p, ESPI_EDIT_PANEL_PORT, 0);

  // read small oled panel ("play panel")
  xfer.tx_buf = NULL;
  xfer.rx_buf = rx + BUTTON_BYTES_GENERAL_PANELS + BUTTON_BYTES_CENTRAL_PANEL;
  xfer.len    = BUTTON_BYTES_SOLED_PANEL;
  espi_driver_scs_select(p, ESPI_PLAY_PANEL_PORT, p->play_buttons_device);
  gpio_set_value(p->gpio_sap, 0);
  gpio_set_value(p->gpio_sap, 1);
  espi_driver_transfer(p->spidev, &xfer);
  espi_driver_scs_select(p, ESPI_PLAY_PANEL_PORT, 0);

  // MASKING of unused bits is now removed as we now (for hardware version 7.1)
  // want to read the 'hidden'/unused button lines as well, for ID purposes */
  //   rx[BUTTON_BYTES_GENERAL_PANELS + 1] |= 0x30;
  //   rx[BUTTON_BYTES_GENERAL_PANELS + BUTTON_BYTES_CENTRAL_PANEL] |= 0x0F;

  //  check read states and generate events on changes
  if (initial_passes)
  {
    initial_passes--;
    for (i = 0; i < BUTTON_STATES_SIZE; i++)
    {  // fill last and previous last states with current state
      prev_button_states[i] = button_states[i];
      button_states[i]      = rx[i];
    }
  }
  else
  {
    // A falling ("pressed") edge is to be submitted immediately, provided that the previous two states have been high ("released"),
    // whereas a rising ("released") edge is to be submitted only when the transition to high happened in the previous pass and we
    // continue to read high in the current pass, that is, the rising edge is submitted with a delay of one pass.
    // Overall, this provides a 1-cyle debouncing of the "release" action only, to avoid some (not all, though)
    // false triggers if a button is pressed but has weak (intermittant) contact or is bouncing during intended release.
    // The bit sequences (leftmost bit is oldest) we are looking for (others are to be ingnored)
    //   1 1 0 ==> "pressed" transition
    //   0 1 1 ==> "released" transition
    for (i = 0; i < BUTTON_STATES_SIZE; i++)
    {
      changed_bits = (rx[i] ^ prev_button_states[i]);  // we don't need to check the middle bits, only the edge bits
      if (panel_unit_is_online() && changed_bits)      // new state is different for at least one line in this byte ?
      {
        for (j = 0; j < 8; j++)
        {  // scan through bits
          bit_pos = 1 << j;
          if (!(changed_bits & bit_pos)          // if edge bits are the same state
              || !(button_states[i] & bit_pos))  // OR middle bit is low (pressed)
            continue;                            //   then it can't be any of the bit sequences we are looking for
          btn_id = (i * 8) + (7 - j);
          if (btn_id == PANEL_UNIT_CONNECTION_ID)  // special, discard "return panel_unit_offline"
            continue;
          if (rx[i] & bit_pos)                        // new bit is high (released) ?
            write_id_to_outputbuffer(0x80 | btn_id);  //  then add in bit 7
          else
            write_id_to_outputbuffer(0x00 | btn_id);  //  else leave bit 7 cleared
        }
      }
      prev_button_states[i] = button_states[i];
      button_states[i]      = rx[i];
    }
  }

  // now process any button polling requests
  mutex_lock(&btn_poll_buff_lock);  // keep buttons_fops_write() from interfering
  while (btn_poll_buff_tail != btn_poll_buff_head)
  {  // have more pending requests
    btn_id             = button_poll_buff[btn_poll_buff_tail];
    btn_poll_buff_tail = (btn_poll_buff_tail + 1) % BUTTON_POLL_BUFFER_SIZE;

    if (!(btn_id & 0x80))  // "poll button" command (bit 7 is cleared) ?
    {
      if (btn_id == PANEL_UNIT_CONNECTION_ID)  // special, return "panel_unit_offline" rather than physical line
      {
        if (!panel_unit_is_online())
          btn_id |= 0x80;
      }
      else
      {
        // check if button currently reads high, then add in bit 7
        if (button_states[btn_id / 8] & (1 << (7 - btn_id % 8)))
          btn_id |= 0x80;
      }
      write_id_to_outputbuffer(btn_id);
    }
  }
  mutex_unlock(&btn_poll_buff_lock);

  espi_driver_set_mode(((struct espi_driver *) p)->spidev, SPI_MODE_0);
}

// EOF