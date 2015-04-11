#include <linuxice.h>
#include <input.h>
#include <linux/module.h>
//#include <linux/input.h>

static unsigned char keymap[3][128] = { //From Linice
    {
        // Normal keys
        '?',  ESC,  '1',  '2',  '3',  '4',  '5',  '6',       '7',  '8',  '9',  '0',  '-',  '=',  '\b',  '\t',
        'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',       'o',  'p',  '[',  ']',  ENTER, '?',  'a',   's',
        'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',       '\'', '`',  '?',  '\\', 'z',  'x',  'c',   'v',
        'b',  'n',  'm',  ',',  '.',  '/',  '?',  '*',       '?',  ' ',  '?',  F1,   F2,   F3,   F4,   F5,
        F6,   F7,   F8,   F9,   F10, NUMLOCK, SCROLL, HOME,  UP,  PGUP,  '?',  LEFT, '5', RIGHT, '?',   END,
        DOWN, PGDN, INS,  DEL,  '?',  '?',  '?',  F11,       F12,  '?',  '?',  '?',  '?',  '?',  '?',  '?',
        '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',       '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?'
    },
    {
        // Shift + key
        '?',  ESC,  '!',  '@',  '#',  '$',  '%',  '^',       '&',  '*',  '(',  ')',  '_',  '+',  '\b', '\t',
        'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',       'O',  'P',  '{',  '}',  ENTER, '?',  'A',  'S',
        'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':',       '"',  '~',  '?',  '|',  'Z',  'X',  'C',  'V',
        'B',  'N',  'M',  '<',  '>',  '?',  '?',  '*',       '?',  ' ',  '?',  F1,   F2,   F3,   F4,   F5,
        F6,   F7,   F8,   F9,   F10, NUMLOCK, SCROLL, HOME,  UP,  PGUP,  '?',  LEFT, '5', RIGHT, '?',   END,
        DOWN, PGDN, INS,  DEL,  '?',  '?',  '?',  F11,       F12,  '?',  '?',  '?',  '?',  '?',  '?',  '?',
        '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',       '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?'
    },
    {
        // Alt + key
        0,    0,    0,    '@',  0,    '$',  0,    0,         '{',  '[',  ']',  '}',  '\\', 0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,         0,    0,    0,    '~',  ENTER, 0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,         0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,         0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,         0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,         0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,         0,    0,    0,    0,    0,    0,    0,    0
    }
};

#define KEYQ_MAX	64

struct key_queue {
    volatile unsigned short buf[KEYQ_MAX];
    volatile unsigned int head, tail;
}keyq;

//From linux kernel

#define KEYQ_INC(a) ((a) = ((a)+1) & (KEYQ_MAX-1))
#define KEYQ_DEC(a) ((a) = ((a)-1) & (KEYQ_MAX-1))
#define KEYQ_EMPTY(a) ((a).head == (a).tail)
#define KEYQ_LEFT(a) (((a).tail-(a).head-1)&(KEYQ_MAX-1))
#define KEYQ_LAST(a) ((a).buf[(KEYQ_MAX-1)&((a).head-1)])
#define KEYQ_FULL(a) (!KEYQ_LEFT(a))
#define KEYQ_CHARS(a) (((a).head-(a).tail)&(KEYQ_MAX-1))
#define KEYQ_GETCH(queue,c) \
(void)({c=(queue).buf[(queue).tail];KEYQ_INC((queue).tail);})
#define KEYQ_PUTCH(c,queue) \
(void)({(queue).buf[(queue).head]=(c);KEYQ_INC((queue).head);})

void put_key(unsigned char c) {
    if(KEYQ_FULL(keyq))
        return;

    KEYQ_PUTCH(c, keyq);
}

unsigned char get_key_async(void) {
    unsigned char c;

    if(KEYQ_EMPTY(keyq))
        return 0;

    KEYQ_GETCH(keyq, c);

    return c;
}

unsigned char get_key(void) {
    unsigned char c;

    while(KEYQ_EMPTY(keyq)) {
        __asm__("hlt");
    }

    KEYQ_GETCH(keyq, c);
    //printk(KERN_ALERT "get_key2\n");
    return c;
}

void int_0xef(void);

int kbd_handler(int code, int state) {
    static int func_key = 0;

    if(code == ICE_KEY_LCTRL || code == ICE_KEY_RCTRL) {
        if(state == ICE_KEYDOWN) {
            func_key |= CHAR_CTRL;
        } else if(state == ICE_KEYUP) {
            func_key &= ~CHAR_CTRL;
        }
    }

    if(code == ICE_KEY_LSHIFT || code == ICE_KEY_RSHIFT) {
        if(state == ICE_KEYDOWN) {
            func_key |= CHAR_SHIFT;
        } else if(state == ICE_KEYUP) {
            func_key &= ~CHAR_SHIFT;
        }
    }

    if(code == ICE_KEY_LALT || code == ICE_KEY_RALT) {
        if(state == ICE_KEYDOWN) {
            func_key |= CHAR_ALT;
        } else if(state == ICE_KEYUP) {
            func_key &= ~CHAR_ALT;
        }
    }
    
    if(ice.running_state == NOT_RUNNING) {
        if((func_key & CHAR_CTRL) && (keymap[0][code] == 'q') && state == ICE_KEYDOWN) {
            ice.running_state = ENTERING;
            //ack_APIC_irq();
            hook_idt(0xef, int_0xef);
            return 0;
        } else {
            return 1;
        }
    }else{
        if(state == ICE_KEYDOWN || state > 1) {
            if(func_key & CHAR_SHIFT)
                put_key(keymap[1][code & 0x000000FF]);
            else if(func_key & CHAR_ALT)
                put_key(keymap[2][code & 0x000000FF]);
            else
                put_key(keymap[0][code & 0x000000FF]);
        }
    }

    return 0;
}
