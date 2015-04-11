#ifndef __LINUXICE_INPUT__
#define __LINUXICE_INPUT__

//From Linice
// Define pseudo-ascii codes for control characters:

#define UP              2
#define DOWN            3
#define PGUP            4
#define PGDN            5
#define LEFT            6
#define RIGHT           7

#define BACKSPACE       8               // ASCII '\b'
#define TAB             9               // ASCII '\t'
#define ENTER           10              // ASCII '\n'

#define F1              11
#define F2              12
#define F3              13
#define F4              14
#define F5              15
#define F6              16
#define F7              17
#define F8              18
#define F9              19
#define F10             20
#define F11             21
#define F12             22

#define NUMLOCK         23
#define SCROLL          24
#define INS             25
#define DEL             26

#define ESC             27

#define HOME            28
#define END             29


#define UNUSED1         1
#define UNUSED2         30
#define UNUSED3         31

#define ICE_KEYDOWN		1
#define ICE_KEYUP		0

#define ICE_KEY_LCTRL	29
#define ICE_KEY_RCTRL	97
#define ICE_KEY_LSHIFT	42
#define ICE_KEY_RSHIFT	54
#define ICE_KEY_LALT	56
#define ICE_KEY_RALT	100

#define CHAR_SHIFT      0x1000
#define CHAR_ALT        0x2000
#define CHAR_CTRL       0x4000

void put_key(unsigned char c);
unsigned char get_key_async(void);
unsigned char get_key(void);
int kbd_handler(int code, int state);

#endif 
