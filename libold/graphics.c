#include <graphics.h>
#include <string.h>
#include <sys.h>
#include <console.h>
void graphics_init(){
	screen_wid=*((uint16_t *)(VBE_base_address+0x12));
	screen_hei=*((uint16_t *)(VBE_base_address+0x14));
	graphics_vram = (uint8_t *)(*((uint32_t *)(0x900)));
	graphics_background_color.R = 0x00;
	graphics_background_color.G = 0x00;
	graphics_background_color.B = 0x00;
	set_background(graphics_background_color);
	graphics_cursor_color.R = 0xFF;
	graphics_cursor_color.G = 0xFF;
	graphics_cursor_color.B = 0xFF;
	graphics_default_text_color = graphics_cursor_color;
	current_screen = 0;
}
void desktop_init(){
	graphics_desktop_buffer_size = screen_wid*screen_hei*3;
	graphics_desktop_buffer = malloc(graphics_desktop_buffer_size);
	if(graphics_desktop_buffer == NULL){
		while(1);
	}
	(console+1)->type = SCREEN_DESKTOP;
	(console+1)->flags = 0;
	(console+1)->keyboard_buffer = (char *)DESKTOP_KEYBOARD_BUFFER_ADDRESS;
	for(int i = 0;i<graphics_desktop_buffer_size/4;i++){
		*((uint32_t *)graphics_desktop_buffer+i) = 0x00000000;
	}
}
/*void desktop_screen_save(){
	for(int i = 0;i<graphics_desktop_buffer_size/4;i++){
		*((uint32_t *)graphics_desktop_buffer+i) = *((uint32_t *)graphics_vram+i);
	}
}*/
void desktop_screen_flush(){
	for(int i = 0;i<graphics_desktop_buffer_size/4;i++){
		*((uint32_t *)graphics_vram+i) = *((uint32_t *)graphics_desktop_buffer+i);
	}
}
void change_screen(uint8_t screen){
	if((console+screen)->type == SCREEN_UNUSED){
		return;
	}
	if(screen == current_screen){
		return;
	}
	if((console+screen)->type == SCREEN_CONSOLE){
		current_screen = screen;
		screen_flush();
	}else if((console+screen)->type == SCREEN_DESKTOP){
		current_screen = screen;
		desktop_screen_flush();
	}	
}
void graphics_pixel_write(unsigned int x,unsigned int y,graphics_pixel_t pixel){
	if(x >= screen_wid){
		return;
	}
	if(y >= screen_hei){
		return;
	}
	*(graphics_vram+(y*screen_wid+x)*3) = pixel.B;
	*(graphics_vram+(y*screen_wid+x)*3+1) = pixel.G;
	*(graphics_vram+(y*screen_wid+x)*3+2) = pixel.R;
	//It has 3 bytes.
}
void desktop_buffer_pixel_write(unsigned int x,unsigned int y,graphics_pixel_t pixel){
	if(x >= screen_wid){
		return;
	}
	if(y >= screen_hei){
		return;
	}
	*(graphics_desktop_buffer+(y*screen_wid+x)*3) = pixel.B;
	*(graphics_desktop_buffer+(y*screen_wid+x)*3+1) = pixel.G;
	*(graphics_desktop_buffer+(y*screen_wid+x)*3+2) = pixel.R;
	//It has 3 bytes.
}
void graphics_draw_hline(unsigned int startx,unsigned int starty,unsigned int length,graphics_pixel_t pixel){
	for(int i=0;i<length;i++){
		graphics_pixel_write(startx+i,starty,pixel);
	}
}
void graphics_draw_box(unsigned int startx,unsigned int starty,unsigned int width,unsigned int height,graphics_pixel_t pixel){
	for(int j=0;j<height;j++){
		for(int i=0;i<width;i++){
			graphics_pixel_write(startx+i,starty+j,pixel);
		}
	}
}
void show_char(unsigned int x,unsigned int y,unsigned char c,graphics_pixel_t pixel,bool fill){
	uint8_t cp;
	for(int i = 0;i<text_height;i++){
		for(int j = 0;j<text_width;j++){
			cp = *((uint8_t *)(font_base_address+c*per_char_size+i));
			if(cp&(1<<(text_width-j))){
				graphics_pixel_write(x+j,y+i,pixel);
			}else if(fill){
				graphics_pixel_write(x+j,y+i,graphics_background_color);
			}
		}
	}
}
void show_string(unsigned int x,unsigned int y,unsigned char *s,graphics_pixel_t pixel){
	unsigned int len = strlen(s);
	for(int i=0;i<len;i++){
		show_char(x+i*text_width,y,s[i],pixel,0);
	}
}
void set_background(graphics_pixel_t pixel){
	uint32_t n[3] = {(pixel.B|(pixel.G<<8)|(pixel.R<<16)|(pixel.B<<24)),
	(pixel.G|(pixel.R<<8)|(pixel.B<<16)|(pixel.G<<24)),
	(pixel.R|(pixel.B<<8)|(pixel.G<<16)|(pixel.R<<24))};
	uint32_t *vram_addr = (uint32_t *)graphics_vram;
	uint32_t graphics_vram_end = (uint32_t)graphics_vram+screen_wid*screen_hei*3;
	int i = 0;
	while((uint32_t)vram_addr<graphics_vram_end){
		*(vram_addr) = n[i];
		vram_addr+=1;
		if(i == 2){
			i = 0;
		}else{
			i+=1;
		}
	}
}
void hlinecpy(unsigned int end,unsigned int start,unsigned int number){
	for(int i = 0;i<number;i++){
		for(int j = 0;j<(screen_wid/4);j++){
			*((uint32_t *)(graphics_vram+(start+i)*screen_wid*3+j*12)) = *((uint32_t *)(graphics_vram+(end+i)*screen_wid*3+j*12));
			*((uint32_t *)(graphics_vram+(start+i)*screen_wid*3+4+j*12)) = *((uint32_t *)(graphics_vram+(end+i)*screen_wid*3+4+j*12));
			*((uint32_t *)(graphics_vram+(start+i)*screen_wid*3+8+j*12)) = *((uint32_t *)(graphics_vram+(end+i)*screen_wid*3+8+j*12));
		}
	}
}
