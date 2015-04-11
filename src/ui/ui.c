#include <linuxice.h>
#include <linux/fb.h>
#include <linux/syscalls.h>
#include <kernelfb.h>
#include <ui.h>
#include <input.h>
#include <command.h>

static kfb_handle_t* kfb_handle;
static const struct kfb_color default_fgcolor = {225, 225, 225, 0},
       default_bgcolor = {0x01, 0x01, 0xaa, 0},
       regchanged_color = {255, 204, 0};
enum UI_REGS {UI_RAX, UI_RBX, UI_RCX, UI_RDX, UI_RSI, UI_RDI, UI_RBP,
              UI_RSP, UI_R8, UI_R9, UI_R10, UI_R11, UI_R12, UI_R13,
              UI_R14, UI_R15,
              UI_CS, UI_DS, UI_ES, UI_FS, UI_GS, UI_SS,
              UI_FLAGS_OF, UI_FLAGS_DF, UI_FLAGS_IF, UI_FLAGS_SF,
              UI_FLAGS_ZF, UI_FLAGS_AF, UI_FLAGS_PF, UI_FLAGS_CF,UI_REGS_NUM
             };

static char* REG_NAME[] = {"RAX", "RBX", "RCX", "RDX", "RSI", "RDI",
                           "RBP", "RSP", "R8 ", "R9 ", "R10", "R11",
                           "R12", "R13", "R14", "R15",
                           "CS", "DS", "ES", "FS", "GS", "SS", 
                           "O", "D", "I", "S", "Z", "A", "P", "C"
                          };
                          
struct {
	struct kfb_point regs[UI_REGS_NUM];
	struct kfb_point view_lt[5];
} ice_layout;

struct {
    struct ice_listview *stack_view, *data_view, *code_view, *output_view;
    struct ice_textedit *cmd_edit;
} ice_ui;

static inline unsigned int get_line_starty(unsigned int lineno) {
    return lineno * DEFAULT_FONT_HEIGHT;
}

unsigned int sprint_addr(char* buffer, unsigned long addr) {
    char t[20];
    int i;
    unsigned int ret = sprintf(t, "%016lX", addr);

    for(i = 0; t[i] != '\0'; i++) {
        if(i > 0 && i % 4 == 0) {
            *buffer++ = '-';
        }

        *buffer++ = t[i];
    }
	*buffer = '\0';
    return ret + 3;
}

static void init_layout(void){
	int i;
	
	//regs
	for(i = 0; i < 16; i++){
		ice_layout.regs[i].y = get_line_starty(1 + i);
		ice_layout.regs[i].x = 0;
	}
	
	ice_layout.regs[UI_CS].x = 0;
	ice_layout.regs[UI_CS].y = get_line_starty(++i);
	
	ice_layout.regs[UI_DS].x = get_current_font_width() * 8;
	ice_layout.regs[UI_DS].y = get_line_starty(i);
	
	ice_layout.regs[UI_FLAGS_OF].x = get_current_font_width() * 16;
	ice_layout.regs[UI_FLAGS_OF].y = get_line_starty(i);
	
	ice_layout.regs[UI_FLAGS_DF].x = get_current_font_width() * 18;
	ice_layout.regs[UI_FLAGS_DF].y = get_line_starty(i);
	
	ice_layout.regs[UI_FLAGS_IF].x = get_current_font_width() * 20;
	ice_layout.regs[UI_FLAGS_IF].y = get_line_starty(i);

	
	ice_layout.regs[UI_ES].x = 0;
	ice_layout.regs[UI_ES].y = get_line_starty(++i);
	
	ice_layout.regs[UI_FS].x = get_current_font_width() * 8;
	ice_layout.regs[UI_FS].y = get_line_starty(i);
	
	ice_layout.regs[UI_FLAGS_SF].x = get_current_font_width() * 16;
	ice_layout.regs[UI_FLAGS_SF].y = get_line_starty(i);
	
	ice_layout.regs[UI_FLAGS_ZF].x = get_current_font_width() * 18;
	ice_layout.regs[UI_FLAGS_ZF].y = get_line_starty(i);
	
	ice_layout.regs[UI_FLAGS_AF].x = get_current_font_width() * 20;
	ice_layout.regs[UI_FLAGS_AF].y = get_line_starty(i);
	
	ice_layout.regs[UI_GS].x = 0;
	ice_layout.regs[UI_GS].y = get_line_starty(++i);
	
	ice_layout.regs[UI_SS].x = get_current_font_width() * 8;
	ice_layout.regs[UI_SS].y = get_line_starty(i);
	
	ice_layout.regs[UI_FLAGS_PF].x = get_current_font_width() * 16;
	ice_layout.regs[UI_FLAGS_PF].y = get_line_starty(i);
	
	ice_layout.regs[UI_FLAGS_CF].x = get_current_font_width() * 18;
	ice_layout.regs[UI_FLAGS_CF].y = get_line_starty(i);
}

char temp[2000];

//UI private functions
//
static void draw_borders(void) {
    if(!kfb_handle)
        return;

    //Top horizontal line for the title of register & data area
    kfb_handle->ops->draw_hline(0, kfb_handle->vinfo.xres, get_current_font_height() / 2, default_fgcolor, 1, kfb_handle);

    //Vertical line
    kfb_handle->ops->draw_vline(21 * get_current_font_width(),
                                get_current_font_height() / 2,
                                kfb_handle->vinfo.yres,
                                default_fgcolor, 1, kfb_handle);

    //Horizontal line for stack
    kfb_handle->ops->draw_hline(0,
                                21 * get_current_font_width(),
                                get_line_starty(20) + get_current_font_height() / 2,
                                default_fgcolor, 1, kfb_handle);

    //Horizontal line for disassembly
    kfb_handle->ops->draw_hline(21 * get_current_font_width(),
                                kfb_handle->vinfo.xres,
                                get_line_starty(8) + get_current_font_height() / 2,
                                default_fgcolor, 1, kfb_handle);

    //Horizontal line for command
    kfb_handle->ops->draw_hline(21 * get_current_font_width(),
                                kfb_handle->vinfo.xres,
                                get_line_starty(22) + get_current_font_height() / 2,
                                default_fgcolor, 1, kfb_handle);

    kfb_handle->ops->print_str("Register", 20, -1, 0, default_fgcolor, default_bgcolor, 0, 0, kfb_handle);
    kfb_handle->ops->print_str("Data",  21 * get_current_font_width() + 20, -1, 0, default_fgcolor, default_bgcolor, 0, 0, kfb_handle);
    kfb_handle->ops->print_str("Stack", 20, -1, get_line_starty(20), default_fgcolor, default_bgcolor, 0, 0, kfb_handle);
    kfb_handle->ops->print_str("Disassembly", 21 * get_current_font_width() + 20, -1, get_line_starty(8), default_fgcolor, default_bgcolor, 0, 0, kfb_handle);
    kfb_handle->ops->print_str("Command", 21 * get_current_font_width() + 20, -1, get_line_starty(22), default_fgcolor, default_bgcolor, 0, 0, kfb_handle);
}

static void print_stack(void) {
    unsigned long base = ice.regs->sp;
    char buffer[25];
    int i;

    ui_listview_clear(ice_ui.stack_view);

    sprint_addr(buffer, ice.regs->sp);
    buffer[16] = '\0';
    ui_listview_additem(ice_ui.stack_view, buffer, 0, default_bgcolor, default_fgcolor);


    for(i = 0; i < 10; i++) {
        if((base & 0xFFFFFFFFFFFFF000) != ((base - i * 8) & 0xFFFFFFFFFFFFF000)) {
            sprint_addr(buffer, base - i * 8);
            buffer[16] = '\0';
            ui_listview_additem(ice_ui.stack_view, buffer, 0, default_bgcolor, default_fgcolor);
            base = base - i * 8;
        }

        sprintf(buffer, "%03lX %016lX", (ice.regs->sp - i * 8) & 0x0000000000000FFF, *(unsigned long*)(ice.regs->sp - i * 8));
        ui_listview_additem(ice_ui.stack_view, buffer, 0, default_fgcolor, default_bgcolor);
    }

    ui_listview_refresh(ice_ui.stack_view);
}

static void print_data(void) {
    char buffer[100];
    int i, j;
    char c;
    unsigned long base = print_data;
    unsigned long tmp;

    ui_listview_clear(ice_ui.data_view);

    for(i = 0; i < 10; i++) {
        tmp = base;
        buffer[0] = '\0';

        for(j = 0; j < 8; j++) {
            sprintf(buffer, "%s%02X%02X ", buffer,
                    (*(unsigned int*)tmp) & 0x00000000000000FF, (*(unsigned int*)(base + 1)) & 0x00000000000000FF);
            tmp += 2;
        }

        for(j = 0; j < 16; j++) {
            c = *(char*)(base + j);

            if((c == '\0') || (c == '\n'))
                c = '?';

            sprintf(buffer, "%s%c", buffer, c);
        }

        base += 16;
        ui_listview_additem(ice_ui.data_view, buffer, 0, default_fgcolor, default_bgcolor);
    }

    ui_listview_refresh(ice_ui.data_view);
}

static void print_code(void) {
    char *temp1, *temp2;
    ice_disasm_attr attr;

    ui_listview_clear(ice_ui.code_view);

    attr = ice_new_disasm(ice.regs->ip, 100);
    ice_disasm_format(&attr, temp, 2000);
    ice_free_disasm(&attr);

    temp1 = temp;

    while(*temp1 != '\0') {
        temp2 = temp1;

        while(*temp2 != '\n' && *temp2 != '\0')
            temp2++;

        if(*temp2 == '\0') {
            ui_listview_additem(ice_ui.code_view, temp1, 0, default_fgcolor, default_bgcolor);
            break;
        }

        *temp2 = '\0';

        if(temp1 == temp)
            ui_listview_additem(ice_ui.code_view, temp1, 0, default_bgcolor, default_fgcolor);
        else
            ui_listview_additem(ice_ui.code_view, temp1, 0, default_fgcolor, default_bgcolor);

        temp1 = temp2 + 1;
    }

    ui_listview_refresh(ice_ui.code_view);
}

static void update_reg(int reg, unsigned long value, unsigned long prev_value) {
    struct kfb_point p;
    char tmp[20];

    if(!kfb_handle || reg >= UI_REGS_NUM)
        return;

    //p.y = get_line_starty(reg / regs_per_line + 1);
    //p.x = (reg % regs_per_line)	* (21 * get_current_font_width());

	if(reg >= 16)
		sprintf(tmp, "%04lX", value);
	else
		sprintf(tmp, "%016lX", value);
	p = kfb_handle->ops->print_str(REG_NAME[reg], ice_layout.regs[reg].x, -1, ice_layout.regs[reg].y, default_fgcolor, default_bgcolor, 0, 0, kfb_handle);
	p = kfb_handle->ops->print_str(" ", p.x, -1, p.y, default_fgcolor, default_bgcolor, 0, 0, kfb_handle);
	p = kfb_handle->ops->print_str(tmp, p.x, -1, p.y, (prev_value == value) ? default_fgcolor : regchanged_color, default_bgcolor, 0, 0, kfb_handle);
}

static void update_flagsbit(int bit, int mask){
	if(ice.regs->flags & mask){
		kfb_handle->ops->print_char(REG_NAME[bit][0], ice_layout.regs[bit].x, ice_layout.regs[bit].y, (ice.prev_regs.flags & mask) ? default_fgcolor : regchanged_color, default_bgcolor, kfb_handle);
	}else{
		kfb_handle->ops->print_char(REG_NAME[bit][0] + 32, ice_layout.regs[bit].x, ice_layout.regs[bit].y, (ice.prev_regs.flags & mask) ? default_fgcolor : regchanged_color, default_bgcolor, kfb_handle);
	}
}

static void update_cmdline(void) {
    ui_textedit_refresh(ice_ui.cmd_edit);
}

static void update_helpline(const char* cmd) {
    kfb_handle->ops->fill_rect(22 * get_current_font_width(), get_line_starty(29),
                               kfb_handle->vinfo.xres, kfb_handle->vinfo.yres, MAKE_COLOR(0, 0x80, 0), kfb_handle);
    kfb_handle->ops->print_str(get_helpline(cmd), 22 * get_current_font_width(), kfb_handle->vinfo.xres,
                               get_line_starty(29), MAKE_COLOR(0, 0, 0), MAKE_COLOR(0, 0x80, 0), 0, 0, kfb_handle);

    kfb_handle->ops->print_char(':', 22 * get_current_font_width(), get_line_starty(28), default_fgcolor, default_bgcolor, kfb_handle);
}

static void update_regs(void) {
    update_reg(UI_RAX, ice.regs->ax, ice.prev_regs.ax);
    update_reg(UI_RBX, ice.regs->bx, ice.prev_regs.bx);
    update_reg(UI_RCX, ice.regs->cx, ice.prev_regs.cx);
    update_reg(UI_RDX, ice.regs->dx, ice.prev_regs.dx);
    update_reg(UI_RDI, ice.regs->di, ice.prev_regs.di);
    update_reg(UI_RSI, ice.regs->si, ice.prev_regs.si);
    update_reg(UI_RBP, ice.regs->bp, ice.prev_regs.bp);
    update_reg(UI_RSP, ice.regs->sp, ice.prev_regs.sp);
    update_reg(UI_R8, ice.regs->r8, ice.prev_regs.r8);
    update_reg(UI_R9, ice.regs->r9, ice.prev_regs.r9);
    update_reg(UI_R10, ice.regs->r10, ice.prev_regs.r10);
    update_reg(UI_R11, ice.regs->r11, ice.prev_regs.r11);
    update_reg(UI_R12, ice.regs->r12, ice.prev_regs.r12);
    update_reg(UI_R13, ice.regs->r13, ice.prev_regs.r13);
    update_reg(UI_R14, ice.regs->r14, ice.prev_regs.r14);
    update_reg(UI_R15, ice.regs->r15, ice.prev_regs.r15);

    update_reg(UI_CS, ice.regs->cs, ice.prev_regs.cs);
    update_reg(UI_DS, ice.regs->ds, ice.prev_regs.ds);
    update_reg(UI_ES, ice.regs->es, ice.prev_regs.es);
    update_reg(UI_FS, ice.regs->fs, ice.prev_regs.fs);
    update_reg(UI_GS, ice.regs->gs, ice.prev_regs.gs);
    update_reg(UI_SS, ice.regs->ss, ice.prev_regs.ss);    
    
    update_flagsbit(UI_FLAGS_OF, OF_MASK);
    update_flagsbit(UI_FLAGS_AF, AF_MASK);
    update_flagsbit(UI_FLAGS_ZF, ZF_MASK);
    update_flagsbit(UI_FLAGS_IF, IF_MASK);
    update_flagsbit(UI_FLAGS_SF, SF_MASK);
    update_flagsbit(UI_FLAGS_DF, DF_MASK);
    update_flagsbit(UI_FLAGS_PF, PF_MASK);
    update_flagsbit(UI_FLAGS_CF, CF_MASK);
    
    memcpy(&ice.prev_regs, ice.regs, sizeof(ice.prev_regs));
}

static void update_outputview(void){
	ui_listview_refresh(ice_ui.output_view);
}

//Public functions
//
int ui_init(void) {
    if(KFB_FAILED == kfb_create("/dev/fb0", &kfb_handle)) {
        return 0;
    }
	
	init_layout();
	
    ui_listview_new(&ice_ui.stack_view, 0, get_line_starty(21),
                    20 * get_current_font_width(), kfb_handle->vinfo.yres, 5, kfb_handle);

    ui_listview_new(&ice_ui.data_view, 22 * get_current_font_width(), get_line_starty(1),
                    kfb_handle->vinfo.xres, get_line_starty(8), 5, kfb_handle);

    ui_listview_new(&ice_ui.code_view, 22 * get_current_font_width(), get_line_starty(9),
                    kfb_handle->vinfo.xres - 12, get_line_starty(22), 5, kfb_handle);
                    
    ui_listview_new(&ice_ui.output_view, 22 * get_current_font_width(), get_line_starty(23),
                    kfb_handle->vinfo.xres - 12, get_line_starty(28), 5, kfb_handle);

    ui_textedit_new(&ice_ui.cmd_edit, 23 * get_current_font_width(), get_line_starty(28),
                    kfb_handle->vinfo.xres - 12, get_line_starty(29) - 1, default_fgcolor, default_bgcolor, kfb_handle);
                    
    ui_listview_additem(ice_ui.output_view, "Hello World", 0, default_fgcolor, default_bgcolor);;
    return 1;
}

void ui_refresh(void) {
    if(kfb_handle)
        kfb_flush(kfb_handle);
}

void ui_clean(void) {
    ui_listview_del(&ice_ui.stack_view);
    ui_listview_del(&ice_ui.data_view);
    ui_listview_del(&ice_ui.code_view);
    ui_listview_del(&ice_ui.output_view);
    ui_textedit_del(&ice_ui.cmd_edit);

    if(kfb_handle)
        kfb_delete(&kfb_handle);
}

void show_mainui(void) {
    if(!kfb_handle)
        return;

    kfb_handle->ops->fill_rect(0, 0, 640, 480, default_bgcolor, kfb_handle);

    draw_borders();
    update_regs();
    print_stack();
    print_code();
    print_data();
    update_cmdline();
    update_outputview();
    update_helpline("");
}

void output_addline(const char* line){
	ui_listview_additem(ice_ui.output_view, line, 0, default_fgcolor, default_bgcolor);
	ui_listview_scrolldown(ice_ui.output_view);
	update_outputview();
	ui_refresh();
}

void output_print(const char* fmt, ...){
	va_list args;
	char buf[200];
	
	va_start(args, fmt);
	vsnprintf(buf, 200, fmt, args);
	va_end(args);
	
	output_addline(buf);
}

const char* get_command(void) {
    bool flag = true;
    char c;

    while(flag) {
        switch(c = get_key()) {
        case ENTER:
            flag = false;
            break;

        case BACKSPACE:
            ui_textedit_delchar(ice_ui.cmd_edit);
            break;

        default:
            ui_textedit_addchar(c, ice_ui.cmd_edit);
            break;
        }

        update_helpline(ice_ui.cmd_edit->text);
        ui_textedit_refresh(ice_ui.cmd_edit);
        ui_refresh();
    }

    return ice_ui.cmd_edit->text;
}

void clear_command(void) {
    ui_textedit_clear(ice_ui.cmd_edit);
    ui_textedit_refresh(ice_ui.cmd_edit);
    ui_refresh();
}
