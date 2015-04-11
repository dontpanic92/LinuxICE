LIB	= libdistorm3.a
OBJS	= src/mnemonics.o src/wstring.o src/textdefs.o src/prefix.o src/operands.o src/insts.o src/instructions.o src/distorm.o src/decoder.o

KERNEL_DIR := /lib/modules/$(shell uname -r)/build/include
X86_ASMDIR := /lib/modules/$(shell uname -r)/build/arch/x86/include
X86_ASMGENDIR := /lib/modules/$(shell uname -r)/build/arch/x86/include/generated/
CONFIG_FILE := /lib/modules/$(shell uname -r)/build/include/generated/autoconf.h
KCONFIG_FILE := /lib/modules/$(shell uname -r)/build/include/linux/kconfig.h

CFLAGS	= -fno-common -Wall -D__KERNEL__ -I$(KERNEL_DIR) -I$(X86_ASMDIR) -I$(X86_ASMGENDIR) -include $(CONFIG_FILE) -include $(KCONFIG_FILE) -mcmodel=kernel

AR = ar
ARFLAGS = -rc

libdistorm3.a:$(OBJS)
	$(AR) $(ARFLAGS) -o $@ $^

clean:
	rm -f *.o *.a



