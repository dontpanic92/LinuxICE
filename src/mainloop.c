#include <linux/slab.h>
#include <command.h>
#include <linuxice.h>
#include <ui.h>
#include <input.h>

void (*fn_show_stack)(struct task_struct *task, unsigned long *sp);

void run_debugger(void) {
	int i;
    show_mainui();
    ui_refresh();

    ice.regs->flags &= ~TF_MASK;
    
    if(ice.regs->int_no != 1)
		output_print("Break due to INT 0x%lX", ice.regs->int_no);
	
	fn_show_stack = fn_kallsyms_lookup_name("show_stack");
	
	if(fn_show_stack){
		//fn_show_stack(current, ice.regs->sp);
	}else{
		printk(KERN_ALERT "not found show stack\n");
	}
	
    while(1) {
        if(exec_command(get_command())) {
            clear_command();
            break;
        }

        clear_command();
        //for (i = 0; i < 100000000; i++);
        //break;
    }

    unhook_idt(0xef);
    ice.running_state = NOT_RUNNING;
}

