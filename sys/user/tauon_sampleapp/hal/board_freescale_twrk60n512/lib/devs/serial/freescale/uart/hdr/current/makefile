# eCos makefile

# This is a generated file - do not edit

export PREFIX := /home/sqzwork/dev/hg_lepton_os/hg_lepton_k60/lepton/sys/user/tauon_sampleapp/hal/board_freescale_twrk60n512/lib/install
export COMMAND_PREFIX := arm-eabi-
export CC := $(COMMAND_PREFIX)gcc
export OBJCOPY := $(COMMAND_PREFIX)objcopy
export HOST := UNIX
export AR := $(COMMAND_PREFIX)ar

export REPOSITORY := /home/sqzwork/dev/ecos-v3.0/ecos/packages
PACKAGE := devs/serial/freescale/uart/hdr/current
OBJECT_PREFIX := devs_serial_freescale_uart_hdr
CFLAGS := -Wall -Wpointer-arith -Wstrict-prototypes -Wundef -Woverloaded-virtual -Wno-write-strings -mcpu=cortex-m3 -mthumb -g -O0 -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions
LDFLAGS := -mcpu=cortex-m3 -mthumb -Wl,--gc-sections -Wl,-static -Wl,-n -g -nostdlib
VPATH := $(REPOSITORY)/$(PACKAGE)
INCLUDE_PATH := $(INCLUDE_PATH) -I$(PREFIX)/include $(foreach dir,$(VPATH),-I$(dir) -I$(dir)/src -I$(dir)/tests) -I.
MLT := $(wildcard $(REPOSITORY)/$(PACKAGE)/include/pkgconf/mlt*.ldi $(REPOSITORY)/$(PACKAGE)/include/pkgconf/mlt*.h)
TESTS := 

build: headers

headers: mlt_headers $(PREFIX)/include/cyg/io/ser_freescale_uart.h

$(PREFIX)/include/cyg/io/ser_freescale_uart.h: $(REPOSITORY)/$(PACKAGE)/include/ser_freescale_uart.h
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

include $(REPOSITORY)/pkgconf/rules.mak

