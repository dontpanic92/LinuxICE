#ifndef __LINUXICE__
#define __LINUXICE__
#include <linux/types.h>

struct ice_regs{
	unsigned long bp;
    unsigned long gs;
    unsigned long fs;
    unsigned long es;
    unsigned long ds;
	unsigned long r15;
	unsigned long r14;
	unsigned long r13;
	unsigned long r12;
	unsigned long r11;
	unsigned long r10;
	unsigned long r9;
	unsigned long r8;
	unsigned long si;
	unsigned long di;
	unsigned long dx;
	unsigned long cx;
	unsigned long bx;
	unsigned long ax;
	unsigned long int_no;
	unsigned long error_code;
	unsigned long ip;
	unsigned long cs;
	unsigned long flags;
	unsigned long sp;
	unsigned long ss;
};

typedef struct __idt_descriptor
{
  unsigned short low_offset, selector;
  unsigned char  reserved1, flag;
  unsigned short middle_offset;
  unsigned int   high_offset;
  unsigned int   reserved2;
}idt_descriptor;

typedef struct __ice_disasm_attr{
  void  *inst;
  unsigned int  count;
  unsigned long offset;
  unsigned int  size;
}ice_disasm_attr;

struct ice_struct{
	struct ice_regs*    regs;
    struct ice_regs     prev_regs;
	//bool running;
    int                 running_state;
};

extern volatile struct ice_struct ice;

//-----------------------------------------------------------------------------
// ICE State
//-----------------------------------------------------------------------------
#define NOT_RUNNING 0
#define ENTERING    1   //System will be paused in the next timer interrupt.
#define RUNNING     2



#define GET_INT_HANDLER_OFFSET(idt_desc) \
(((unsigned long)(idt_desc.high_offset) << 32) + \
(((unsigned long)idt_desc.middle_offset << 16) & 0x00000000FFFF0000) + \
(idt_desc.low_offset))

#define SET_INT_HANDLER_OFFSET(idt_desc, offset) \
do{\
 __asm__ ("cli"::);\
idt_desc.high_offset = (unsigned int)((offset >> 32) & 0x00000000FFFFFFFF);\
idt_desc.middle_offset = (unsigned short)((offset >> 16) & 0x000000000000FFFF);\
idt_desc.low_offset = (unsigned short)(offset & 0x000000000000FFFF);\
idt_desc.selector = 16;\
__asm__ ("sti"::);\
}while(0)


#define CR0_WP 0x0000000000010000
//-----------------------------------------------------------------------------
// EFLAGS Bits
//-----------------------------------------------------------------------------

#define CF_BIT              0
#define PF_BIT              2
#define AF_BIT              4
#define ZF_BIT              6
#define SF_BIT              7
#define TF_BIT              8
#define IF_BIT              9
#define DF_BIT              10
#define OF_BIT              11
#define IOPL_BIT0           12
#define IOPL_BIT1           13
#define NT_BIT              14
#define RF_BIT              16
#define VM_BIT              17
#define AC_BIT              18
#define VIF_BIT             19
#define VIP_BIT             20
#define ID_BIT              21

#undef TF_MASK
#undef IF_MASK
#undef IOPL_MASK
#undef NT_MASK
#undef VM_MASK
#undef AC_MASK
#undef VIF_MASK
#undef VIP_MASK
#undef ID_MASK

#define CF_MASK             (1 << CF_BIT)
#define PF_MASK             (1 << PF_BIT)
#define AF_MASK             (1 << AF_BIT)
#define ZF_MASK             (1 << ZF_BIT)
#define SF_MASK             (1 << SF_BIT)
#define TF_MASK             (1 << TF_BIT)
#define IF_MASK             (1 << IF_BIT)
#define DF_MASK             (1 << DF_BIT)
#define OF_MASK             (1 << OF_BIT)
#define IOPL_MASK           (3 << IOPL_BIT0)
#define NT_MASK             (1 << NT_BIT)
#define RF_MASK             (1 << RF_BIT)
#define VM_MASK             (1 << VM_BIT)
#define AC_MASK             (1 << AC_BIT)
#define VIF_MASK            (1 << VIF_BIT)
#define VIP_MASK            (1 << VIP_BIT)
#define ID_MASK             (1 << ID_BIT)

typedef unsigned long (*PFN_KALLSYMS_LOOKUP_NAME)(const char *);
typedef const char *(*PFN_KALLSYMS_LOOKUP)(unsigned long, unsigned long *, unsigned long *, char **,  char *);
typedef int (*PFN_SPRINT_SYMBOL)(char *buffer, unsigned long address);

extern PFN_KALLSYMS_LOOKUP_NAME fn_kallsyms_lookup_name;
extern PFN_KALLSYMS_LOOKUP   fn_kallsyms_lookup;
extern PFN_SPRINT_SYMBOL   fn_sprint_symbol;

void get_symbol(unsigned long addr, char* buf);
ice_disasm_attr ice_new_disasm(void* offset, unsigned int size);
unsigned long ice_disasm_format(const ice_disasm_attr* attr, char* buf, unsigned int buf_size);
void ice_free_disasm(ice_disasm_attr* attr);

void hook_idt(unsigned int int_no, void* fake_handler);
void unhook_idt(unsigned int int_no);
void restore_idt(void);

void hook_kbd(void);
void unhook_kbd(void);

void kallsyms_setup(void);

void register_dev(void);
void unregister_dev(void);

void run_debugger(void);
#endif
