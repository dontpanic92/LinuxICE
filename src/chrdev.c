#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/kallsyms.h>
#include <linuxice.h>

long ice_dev_ioctl(struct file* filp, unsigned int cmd, unsigned long param);

#define ICE_IOC_MAGIC              0x92
#define ICE_IOCTL_KDISASM          _IOWR(ICE_IOC_MAGIC, 1, struct ice_kernel_disasm)
#define ICE_IOCTL_PRINTIDT          _IOWR(ICE_IOC_MAGIC, 2, struct ice_kernel_disasm)

struct ice_kernel_disasm {
    char* addr;
    unsigned long addr_length;
    unsigned long code_length;
    unsigned long buf_size;
    unsigned long buf_size_needed;
    char* buf;
};

static struct file_operations ice_fopt = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = ice_dev_ioctl
    // open:       ice_chrdev_open,
    // release:    ice_chrdev_release,
};

static int dev_number;
static struct cdev ice_cdev;
static struct class *ice_class;

void register_dev(void) {
    int err;

    dev_number =  MKDEV(1, 0);

    register_chrdev_region(dev_number, 1, "linuxice");

    cdev_init(&ice_cdev, &ice_fopt);
    ice_cdev.owner = THIS_MODULE;

    err = cdev_add(&ice_cdev, dev_number, 1);

    if(err != 0)
        printk(KERN_ALERT "linuxice: add dev failed.\n");

    ice_class = class_create(THIS_MODULE, "ice_class");

    if(IS_ERR(ice_class)) {
        printk(KERN_ALERT "linuxice: create class failed.\n");
    }

    device_create(ice_class, NULL, dev_number, NULL, "linuxice");
}

void unregister_dev(void) {
    device_destroy(ice_class, dev_number);
    class_destroy(ice_class);
    cdev_del(&ice_cdev);

    unregister_chrdev_region(dev_number, 1);
}


long ice_dev_ioctl(struct file* filp, unsigned int cmd, unsigned long param) {
    struct ice_kernel_disasm ioctl_param;
    char addr[KSYM_NAME_LEN + 1];
    unsigned long addr_length;
    char* temp = NULL;
    ice_disasm_attr attr;
    unsigned long ret;
    int strtoul_ret;
    unsigned char idtr[10];
    idt_descriptor* idt;


    switch(cmd) {
    case ICE_IOCTL_KDISASM:
        ret = copy_from_user(&ioctl_param, (void*)param, sizeof(struct ice_kernel_disasm));

        addr_length = (ioctl_param.addr_length < sizeof(addr) - 1) ? ioctl_param.addr_length : sizeof(addr) - 1;
        ret = copy_from_user(addr, ioctl_param.addr, ioctl_param.addr_length);
        addr[addr_length] = '\0';

        if(ioctl_param.buf_size > 0)
            temp = kmalloc(ioctl_param.buf_size, GFP_ATOMIC);

        strtoul_ret = strict_strtoul(addr, 16, &ret);

        if(strtoul_ret != 0)
            ret = fn_kallsyms_lookup_name(addr);

        attr = ice_new_disasm((void*)ret, ioctl_param.code_length);

        ioctl_param.buf_size_needed = ice_disasm_format(&attr, temp, ioctl_param.buf_size);

        if(ioctl_param.buf_size > 0)
            ret = copy_to_user(ioctl_param.buf, temp, ioctl_param.buf_size);

        ice_free_disasm(&attr);

        if(ioctl_param.buf_size > 0)
            kfree(temp);

        ret = copy_to_user((void*)param, &ioctl_param, sizeof(struct ice_kernel_disasm));
        break;

    case ICE_IOCTL_PRINTIDT:
        __asm__("sidt %0":"=m"(idtr));
        idt = (idt_descriptor*)(*(unsigned long*)(&idtr[2]));
        printk(KERN_ALERT "idt=%p\n", idt);
        break;
    }

    return 0;
}
