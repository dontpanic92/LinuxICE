#include <linux/kernel.h>
#include <linuxice.h>
#include <linux/kallsyms.h>
#include <linux/slab.h>

idt_descriptor ice_idt[256] __page_aligned_bss;
static idt_descriptor ori_idt[256];
unsigned char ori_idtr[10];

static bool is_inited = false;

static void hook_idt_init(void) {
    idt_descriptor *idt;
    unsigned char idtr[10];

    __asm__ volatile("sidt %0":"=m"(idtr));
    memcpy(ori_idtr, idtr, sizeof(ori_idtr));

    idt = (idt_descriptor*)(*(unsigned long*)(&idtr[2]));
    memcpy(ori_idt, idt, sizeof(idt_descriptor) * 256);
    memcpy(ice_idt, idt, sizeof(idt_descriptor) * 256);

    *((unsigned long *) &idtr[2]) = (unsigned long)ice_idt;
    __asm__("lidt %0"::"m"(idtr));

    is_inited = true;
}

void hook_idt(unsigned int int_no, void* fake_handler) {
    if(!is_inited)
        hook_idt_init();

    if(int_no < 256) {
        SET_INT_HANDLER_OFFSET(ice_idt[int_no], (unsigned long)fake_handler);
    }
}
void unhook_idt(unsigned int int_no) {
    if(!is_inited)
        return;

    if(int_no < 256) {
        ice_idt[int_no] = ori_idt[int_no];
    }
}

void restore_idt(void) {
    if(is_inited == true)
        __asm__("lidt %0"::"m"(ori_idtr));

    is_inited = false;
}
