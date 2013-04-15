ECOS_GLOBAL_CFLAGS = -mcpu=arm926ej-s -Wall -Wpointer-arith -Wstrict-prototypes -Winline -Wundef -Woverloaded-virtual -g -O0 -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -fvtable-gc -finit-priority -mthumb-interwork -mabi=aapcs-linux
ECOS_GLOBAL_LDFLAGS = -mcpu=arm926ej-s -Wl,--gc-sections -Wl,-static -g -nostdlib -mthumb-interwork
ECOS_COMMAND_PREFIX = arm-eabi-
