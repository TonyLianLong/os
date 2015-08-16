#ifndef console_h
#define console_h
#include <stdint.h>
#include <graphics.h>
#define CONSOLE_BUFFER_ADDRESS 0x313BF0
#define CONSOLE_BUFFER_LENGTH 0x100
#define CONSOLE_KEYBOARD_BUFFER_ADDRESS 0x313CF0
#define DESKTOP_KEYBOARD_BUFFER_ADDRESS 0x313D70
#define TAB_WIDTH 4
#define CONSOLE_NUMBER 2
#define SCREEN_CONSOLE_ID 0
#define CONSOLE_FLAGS_PRINT 1
#define CONSOLE_FLAGS_DELETE (1<<1)
struct console_buffer_struct {
char c;
uint8_t console_id;
};
typedef struct console_buffer_struct console_buffer_t;
struct console_struct {
uint8_t type;
//0 Unused
//1 Console
//2 Desktop
uint8_t flags;
//7 6 5 4 3 2 1 0
//            D P
//            E R
//            L I
//            E N
//            T T
//            E  
//PRINT:Print when you type
//DELETE:Can delete when you type backscape
uint32_t control_task;
char *keyboard_buffer;
};
typedef struct console_struct console_t;
#define CONSOLE_BUFFER_NUMBER (CONSOLE_BUFFER_LENGTH/sizeof(console_buffer_t))
console_t *console;
void console_init();
uint8_t console_enable_delete(uint8_t screen_id);
uint8_t console_disable_delete(uint8_t screen_id);
void screen_flush();
void delete_the_first_line();
void write_char(graphics_pixel_t pixel);
void print_char(char c);
void print_string(char *s);
void print_hex(uint32_t num);
void print_int(uint32_t num);
void clear_cursor();
void show_cursor();
void cursor_flash();
#endif