#ifndef _ESPI_DRIVER_H_
#define _ESPI_DRIVER_H_

#include <linux/spi/spi.h>
#include <linux/fb.h>

#define ESPI_LED_DEV_MAJOR        301
#define ESPI_RIBBON_LED_DEV_MAJOR 303
#define ESPI_BUTTON_DEV_MAJOR     302
#define ESPI_ENCODER_DEV_MAJOR    308

#define ESPI_SCS_NUM 6

#define ESPI_EDIT_PANEL_PORT     0
#define ESPI_EDIT_BUTTONS_DEVICE 2
#define ESPI_EDIT_BOLED_DEVICE   1
#define ESPI_EDIT_ENCODER_DEVICE 3

#define ESPI_SELECTION_PANEL_PORT     1
#define ESPI_SELECTION_BUTTONS_DEVICE 2
#define ESPI_SELECTION_LEDS_DEVICE    1

#define ESPI_RIBBON_LEDS_PORT 3

#define ESPI_PLAY_PANEL_PORT   3
#define ESPI_PLAY_SOLED_DEVICE 2

#define ESPI_EPC_CTRL_STATE_PORT 4
#define ESPI_EPC_CONTROL_DEVICE  1
#define ESPI_EPC_STATE_DEVICE    2

#define ESPI_MAIN_CONTROL_PORT   5
#define ESPI_MAIN_CONTROL_DEVICE 1

#define ESPI_LPC_CTRL_DEVICE 5
#define ESPI_LPC_CTRL_PORT   1

struct oleds_fb_par;

struct espi_driver
{
  struct delayed_work work;  // This must be the top entry!
  struct device *     dev;
  struct spi_device * spidev;

  // espi params
  s32 gpio_scs[ESPI_SCS_NUM];
  s32 gpio_sap;
  s32 gpio_dmx;

  //hw ref dependings
  s8 hw_rev_str[3];
  u8 play_buttons_device;
  u8 ribbon_leds_device;
  u8 espi_gpio_dmx_default;

  //framebuffer params
  struct oleds_fb_par *oleds;

  u8 poll_stage;
};

struct oleds_fb_par
{
  u32                 height;
  u32                 width;
  struct fb_info *    info;
  struct espi_driver *espi;
};

int panel_unit_is_online(void);

int jitteredClock(int const sck_hz_mult);

void espi_driver_scs_select(struct espi_driver *spi, s32 port, s32 device);
s32  espi_driver_transfer(struct spi_device *dev, struct spi_transfer *xfer);
s32  espi_driver_set_mode(struct spi_device *dev, u16 mode);

/*** encoder ***/
s32  espi_driver_encoder_setup(struct espi_driver *sb);
s32  espi_driver_encoder_cleanup(struct espi_driver *sb);
void espi_driver_encoder_poll(struct espi_driver *p);
/*** display ***/
s32  espi_driver_oleds_fb_setup(struct espi_driver *sb);
s32  espi_driver_oleds_fb_cleanup(struct espi_driver *sb);
void espi_driver_ssd1305_poll(struct espi_driver *p);
void espi_driver_ssd1322_poll(struct espi_driver *p);
/*** ribbon leds ***/
s32  espi_driver_rb_leds_setup(struct espi_driver *sb);
s32  espi_driver_rb_leds_cleanup(struct espi_driver *sb);
void espi_driver_rb_leds_poll(struct espi_driver *p);
/*** buttons ***/
s32  espi_driver_buttons_setup(struct espi_driver *sb);
s32  espi_driver_buttons_cleanup(struct espi_driver *sb);
void espi_driver_pollbuttons(struct espi_driver *p);
/*** leds ***/
s32  espi_driver_leds_setup(struct espi_driver *sb);
s32  espi_driver_leds_cleanup(struct espi_driver *sb);
void espi_driver_leds_poll(struct espi_driver *p);

#endif
