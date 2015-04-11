LIB	= libkfb.a
OBJS = src/dbg_print.o src/fb_handlers.o src/kernelfb.o src/rgb888_32.o src/vga16fb.o src/general.o src/pixel_font.o

KERNEL_DIR := /lib/modules/$(shell uname -r)/build/include
X86_ASMDIR := /lib/modules/$(shell uname -r)/build/arch/x86/include
X86_ASMGENDIR := /lib/modules/$(shell uname -r)/build/arch/x86/include/generated/
CONFIG_FILE := /lib/modules/$(shell uname -r)/build/include/generated/autoconf.h
KCONFIG_FILE := /lib/modules/$(shell uname -r)/build/include/linux/kconfig.h

CFLAGS	= -fno-common -Wall -D__KERNEL__ -I$(KERNEL_DIR) -I$(X86_ASMDIR) -I$(X86_ASMGENDIR) -include $(CONFIG_FILE) -include $(KCONFIG_FILE) -mcmodel=kernel

AR = ar
ARFLAGS = -rc

libkfb.a:$(OBJS)
	$(AR) $(ARFLAGS) -o $@ $^

clean:
	rm -f *.o *.a
