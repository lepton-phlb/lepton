ECOS_GLOBAL_CFLAGS = -Wall -Wpointer-arith -Wstrict-prototypes -Wundef -Woverloaded-virtual -Wno-write-strings -mcpu=cortex-m3 -mthumb -g -O0 -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions
ECOS_GLOBAL_LDFLAGS = -mcpu=cortex-m3 -mthumb -Wl,--gc-sections -Wl,-static -Wl,-n -g -nostdlib
ECOS_COMMAND_PREFIX = arm-eabi-
