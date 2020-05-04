obj-m += espi_driver.o

SRC := $(shell pwd)

espi_driver-y := espi_driver_core.o espi_fb.o espi_lpc8xx.o espi_ssd1322.o espi_ssd1305.o espi_buttons.o espi_ribbon_leds.o espi_leds.o

ccflags-y += -Wno-error=date-time -DNL_VERSION=\"$(shell git -C $(M) rev-parse --short HEAD)\"

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install
