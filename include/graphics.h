#ifndef graphics_h
#define graphics_h
#define VBE_base_address 0x700
#define font_base_address (0x7E00)
#define per_char_size 12
#define text_width 8
#define text_height 12
#define SCREEN_UNUSED 0
#define SCREEN_CONSOLE 1
#define SCREEN_DESKTOP 2
#define SCREEN_DESKTOP_ID 1
#include <stdbool.h>
#include <stdint.h>
struct graphics_pixel_struct {
unsigned char B;
unsigned char G;
unsigned char R;
};
typedef struct graphics_pixel_struct graphics_pixel_t;
uint16_t screen_wid;
uint16_t screen_hei;
graphics_pixel_t graphics_background_color;
graphics_pixel_t graphics_cursor_color;
graphics_pixel_t graphics_default_text_color;
uint8_t *graphics_vram;
uint8_t current_screen;
uint32_t graphics_desktop_buffer_size;
uint8_t *graphics_desktop_buffer;
void graphics_init();
void desktop_init();
void desktop_screen_flush();
void desktop_buffer_pixel_write(unsigned int x,unsigned int y,graphics_pixel_t pixel);
void change_screen(uint8_t screen);
void graphics_pixel_write(unsigned int x,unsigned int y,graphics_pixel_t pixel);
void graphics_draw_hline(unsigned int startx,unsigned int starty,unsigned int length,graphics_pixel_t pixel);
void graphics_draw_box(unsigned int startx,unsigned int starty,unsigned int width,unsigned int height,graphics_pixel_t pixel);
void show_char(unsigned int x,unsigned int y,unsigned char c,graphics_pixel_t pixel,bool fill);
void show_string(unsigned int x,unsigned int y,unsigned char *s,graphics_pixel_t pixel);
void set_background(graphics_pixel_t pixel);
void hlinecpy(unsigned int start,unsigned int end,unsigned int number);
#endif