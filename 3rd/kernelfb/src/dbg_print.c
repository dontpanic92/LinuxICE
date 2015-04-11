
#include <stdarg.h>
#include <stddef.h>
#include <linux/kernel.h>
#include <linux/slab.h>
static char* str[] = {"<Error>", "<Warning>", "<Info>"};

int kfb_dbg_print(int level, const char* fmt, ...){
	int r;
	printk(KERN_ALERT "KernelFB %s: ", str[level]);
	va_list args;
	va_start(args, fmt);
	r = vprintk_emit(0, 1, NULL, 0, fmt, args);
	va_end(args);
	return r;
}

int kfb_printe(const char* fmt, ...){
	int r;
	printk(KERN_ALERT "KernelFB %s: ", str[0]);
	va_list args;
	va_start(args, fmt);
	r = vprintk_emit(0, 1, NULL, 0, fmt, args);
	va_end(args);
	return r;
}
