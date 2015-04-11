KERNEL_DIR = /lib/modules/$(shell uname -r)/build
EXTRA_CFLAGS = -I$(PWD)/../include -I$(PWD)/../3rd/kernelfb/src/

all:
	$(MAKE) -C ../3rd/kernelfb/
	$(MAKE) -C ../3rd/distorm3/
	$(MAKE) -C $(KERNEL_DIR) $(MAKE_OPTS) M=$(PWD)
	sync
clean:
	$(MAKE) -C ../3rd/kernelfb/ clean
	$(MAKE) -C ../3rd/distorm3/ clean
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean
	rm -f ../src/*.o ../src/.*.o.cmd ../3rd/distorm3/src/*.o ../3rd/distorm3/src/.*.o.cmd

obj-m:=hello.o 

hello-objs:= ../src/command.o ../src/mainloop.o ../src/input.o ../src/hello.o ../src/disassembler.o ../src/asm.o ../src/hook_idt.o ../src/int_handler.o ../src/hook_kbd.o ../src/kallsyms.o ../src/chrdev.o ../src/ui/ui.o ../src/ui/list_view.o ../src/ui/text_edit.o ../3rd/kernelfb/libkfb.a ../3rd/distorm3/libdistorm3.a
