#include <linuxice.h>
#include <linux/input.h>
#include <input.h>

//static unsigned long addr_kbd_event;
//static unsigned long addr_kbd_handler;

static unsigned char ori_entry[5];
void int_0xef(void);

typedef void (*PFN_KBD_EVENT)(struct input_handle *handle, unsigned int event_type,
                              unsigned int event_code, int value);
static PFN_KBD_EVENT kbd_event_addr;

volatile int e = 0;

static void fake_kbd_event(struct input_handle *handle, unsigned int event_type,
                           unsigned int event_code, int value) {
    /*if(ice.running == false){
    	if(event_type == EV_KEY){
    		if(event_code == 2 && value == 1){
    			hook_idt(0xef, int_0xef);
    		}else{
    			unhook_kbd();
    			kbd_event_addr(handle, event_type, event_code, value);
    			hook_kbd();
    		}
    	}
    }else{
    	if (event_type == EV_KEY){
    		kbd_handler(event_code, value);
    	}
    }*/
    if(ice.running_state != NOT_RUNNING){
        if(event_type == EV_KEY)
            kbd_handler(event_code, value);
        return;
    }
    
    if(event_type != EV_KEY || kbd_handler(event_code, value) == 1) {
        unhook_kbd();
        kbd_event_addr(handle, event_type, event_code, value);
        hook_kbd();
    }
    
}

void hook_kbd(void) {
    unsigned long cr0, new_cr0;
    unsigned int tmp;
    kbd_event_addr = (PFN_KBD_EVENT)fn_kallsyms_lookup_name("kbd_event");
    memcpy(ori_entry, (void*)kbd_event_addr, 5);

    __asm__ volatile("movq %%cr0, %0":"=r"(cr0));
    new_cr0 = cr0 & ~CR0_WP;
    __asm__ volatile("movq %0, %%cr0"::"r"(new_cr0));
    __asm__("cli"::);
    *(unsigned char*)(kbd_event_addr) = 0xe9;
    *(unsigned int*)(kbd_event_addr + 1) = (unsigned int)(fake_kbd_event - kbd_event_addr - 5);
    __asm__("sti"::);
    __asm__ volatile("movq %0, %%cr0"::"r"(cr0));
}


void unhook_kbd(void) {
    unsigned long cr0, new_cr0;

    __asm__ volatile("movq %%cr0, %0":"=r"(cr0));
    new_cr0 = cr0 & ~CR0_WP;
    __asm__ volatile("movq %0, %%cr0"::"r"(new_cr0));
    __asm__("cli"::);
    memcpy((void*)kbd_event_addr, ori_entry, 5);
    __asm__("sti"::);
    __asm__ volatile("movq %0, %%cr0"::"r"(cr0));
}
