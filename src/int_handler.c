
#include <linuxice.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/preempt.h>
#include <linux/fb.h>
#include <ui.h>


/*unsigned long __schedule_addr;

unsigned char r;

void hook_sched(void){
  unsigned long cr0, new_cr0;
  __schedule_addr = fn_kallsyms_lookup_name("schedule_user");
  r = *(unsigned char*)__schedule_addr;

  __asm__ volatile ("movq %%cr0, %0":"=r"(cr0));
  new_cr0 = cr0 & ~CR0_WP;
  __asm__ volatile ("movq %0, %%cr0"::"r"(new_cr0));
  *(unsigned char*)(__schedule_addr) = 0xc3;
  __asm__ volatile ("movq %0, %%cr0"::"r"(cr0));
}

void unhook_sched(void){
  unsigned long cr0, new_cr0;

  __asm__ volatile ("movq %%cr0, %0":"=r"(cr0));
  new_cr0 = cr0 & ~CR0_WP;
  __asm__ volatile ("movq %0, %%cr0"::"r"(new_cr0));
  *(unsigned char*)(__schedule_addr) = r;
  __asm__ volatile ("movq %0, %%cr0"::"r"(cr0));
}*/

static u32 apic_tmict = (u32)-1;

void ice_do_timer(struct ice_regs* regs) {
    if (apic_tmict == (u32)-1){
		apic_tmict = apic_read(APIC_TMICT);
	}
    apic_write(APIC_TMICT, apic_tmict);
    ack_APIC_irq();
    
    if(ice.running_state != RUNNING) {

        __asm__("cli");
        ice.regs = regs;
        ice.running_state = RUNNING;
        __asm__("sti");
        //preempt_disable();
        //hook_sched();
        run_debugger();
        
        //ice.running = false;
        //unhook_sched();
        //preempt_enable_no_resched();
        return;
    }
}

void int_0xef(void);

void ice_do_int3(struct ice_regs* regs) {
    if(ice.running_state != RUNNING) {
        __asm__("cli");
        ice.regs = regs;
        ice.running_state = RUNNING;
        //hook_sched();
        hook_idt(0xef, int_0xef);
        __asm__("sti");
        run_debugger();
        //unhook_sched();
        return;
    }
}

void ice_do_debug(struct ice_regs* regs) {
    if(ice.running_state != RUNNING) {
        __asm__("cli");
        ice.regs = regs;
        ice.running_state = RUNNING;
        hook_idt(0xef, int_0xef);
        __asm__("sti");
        run_debugger();
        return;
    }
}

void int_handler(struct ice_regs* regs) {
    switch(regs->int_no) {
    case 0x1:
        ice_do_debug(regs);
        break;

    case 0xef:
        ice_do_timer(regs);
        break;

    case 0x3:
        ice_do_int3(regs);
        break;
    }
}
