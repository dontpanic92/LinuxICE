#include <linux/init.h>
#include <linux/module.h>
#include <linuxice.h>
#include <distorm3/distorm.h>
#include <linux/slab.h>
#include <asm/io_apic.h>
#include <ui.h>

void int_0x3(void);
void int_0x31(void);
void int_0x1(void);
volatile struct ice_struct ice;

static int __init hello_init(void) {
    kallsyms_setup();
    register_dev();

    //for(i = 0; i < 0x20;)
    //i = io_apic_read(0, )
    ui_init();
    hook_kbd();

    hook_idt(0x1, int_0x1);
    hook_idt(0x3, int_0x3);
    //hook_idt(0x31, int_0x31);
    //hook_idt(0xef, int_0xef);
    ice.running_state = NOT_RUNNING;
    return 0;
}

static void __exit hello_exit(void) {
    unregister_dev();
    restore_idt();
    unhook_kbd();
    ui_clean();
}



MODULE_AUTHOR("Anthony Li");
MODULE_LICENSE("GPL");
module_init(hello_init);
module_exit(hello_exit);
