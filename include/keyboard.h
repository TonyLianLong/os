#ifndef keyboard_h
#define keyboard_h
#include <stdint.h>
#define KEYBOARD_BUFFER_SIZE 128
#define LEFT_SHIFT_PRESS 0x2A
#define LEFT_SHIFT_RELEASE 0xAA
#define RIGHT_SHIFT_PRESS 0x36
#define RIGHT_SHIFT_RELEASE 0xB6
#define CAPS_LOCK_PRESS 0x3A
#define CAPS_LOCK_RELEASE 0xBA
#define CONTROL_PRESS 0x1D
#define CONTROL_RELEASE 0x9D
#define F1_PRESS 0x3B
#define F1_RELEASE 0xBB
#define F2_PRESS 0x3C
#define F2_RELEASE 0xBC
#define KEYBOARD_CAPS_LOCK_LED (1<<2)
#define KEYBOARD_NUM_LOCK_LED (1<<1)
#define KEYBOARD_SCROLL_LOCK_LED 1
uint8_t keyboard_LED;
uint8_t shift_pressed;
uint8_t CAPS_LOCK;
uint8_t control_pressed;
void init_keyboard();
void wait_KBC();
void set_keyboard_LED(uint8_t LED);
void init_mouse();
char get_char(uint8_t screen);
void keyboard_exec();
void mouse_exec();
#endif