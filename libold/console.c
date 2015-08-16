#include <console.h>
#include <sys.h>
#include <interrupt.h>
#include <string.h>
console_buffer_t *console_buffer;
int cursor_x;
int cursor_y;
int now_cursor_x;
int now_cursor_y;
uint8_t *console_char;
uint32_t console_char_length;
uint32_t console_text_width;
uint32_t console_buffer_read;
uint32_t console_buffer_write;
void console_init(){
	cursor_x = 0;
	cursor_y = 0;
	console_buffer = (console_buffer_t *)CONSOLE_BUFFER_ADDRESS;
	console_buffer_read = 0;
	console_buffer_write = 0;
	now_cursor_x = cursor_x;
	now_cursor_y = cursor_y;
	console_text_width = screen_wid/text_width;
	console_char_length = console_text_width*(screen_hei/text_height);
	console_char = malloc(console_char_length);
	console = malloc(CONSOLE_NUMBER*sizeof(console_t));
	for(int i = 0;i<CONSOLE_NUMBER;i++){
		(console + i)->type = 0;
		(console + i)->flags = 0;
		(console + i)->keyboard_buffer = NULL;
		(console + i)->control_task = 0;
	}
	console->type = 1;
	console->flags = CONSOLE_FLAGS_PRINT|CONSOLE_FLAGS_DELETE;
	console->keyboard_buffer = (char *)CONSOLE_KEYBOARD_BUFFER_ADDRESS;
	if(console_char == NULL){
		printk("Console:Malloc Failed\n");
		printk(__FILE__);
		while(1);
	}
	for(int i = 0;i<console_char_length;i++){
		console_char[i] = ' ';
	}
}
uint8_t console_enable_delete(uint8_t screen_id){
	if(screen_id >= CONSOLE_BUFFER_NUMBER){
		return 1;
	}
	if((console + screen_id)->type == SCREEN_CONSOLE){
		(console + screen_id)->flags = ((console + screen_id)->flags)|CONSOLE_FLAGS_DELETE;
	}
	return 0;
}
uint8_t console_disable_delete(uint8_t screen_id){
	if(screen_id >= CONSOLE_BUFFER_NUMBER){
		return 1;
	}
	if((console + screen_id)->type == SCREEN_CONSOLE){
		(console + screen_id)->flags = ((console + screen_id)->flags)&(~(CONSOLE_FLAGS_DELETE));
	}
	return 0;
}
void console_return(){
	cursor_y+=text_height;
	cursor_x = 0;
	if(cursor_y >= screen_hei){
		cursor_y = screen_hei - text_height;
		clear_cursor();
		hlinecpy(text_height,0,screen_hei-text_height);
		for(int i = 0;i<text_height;i++){
			graphics_draw_hline(0,screen_hei-text_height+i,screen_wid,graphics_background_color);
		}
		delete_the_first_line();
		show_cursor();
		now_cursor_x = cursor_x;
		now_cursor_y = cursor_y;
		//screen_flush();
	}else{
		cursor_flash();
	}
}
void screen_flush(){
	char show_c;
	set_background(graphics_background_color);
	for(int i = 0;i<console_text_width;i++){
		for(int j = 0;j<(screen_hei/text_height);j++){
			show_c = console_char[i+j*console_text_width];
			if(show_c != ' '){
				show_char(i*text_width,j*text_height,show_c,graphics_default_text_color,0);
			}
		}
	}
	cursor_flash();
}
void delete_the_first_line(){
	int line_length = screen_wid/text_width;
	int i;
	for(i = 0;i<(console_char_length-line_length);i++){
		console_char[i] = console_char[line_length+i];
	}
	for(i = 0;i<line_length;i++){
		console_char[(console_char_length-line_length)+i] = ' ';
	}
}
void print_char(char c){
	if(console_buffer_read == (console_buffer_write+1)%CONSOLE_BUFFER_LENGTH){
		return;
	}
	(console_buffer+console_buffer_write)->c = c;
	(console_buffer+console_buffer_write)->console_id = 0;
	console_buffer_write++;
	if(console_buffer_write >= CONSOLE_BUFFER_NUMBER){
		console_buffer_write = 0;
	}
}
void write_char(graphics_pixel_t pixel){
	if(console_buffer_read == console_buffer_write){
		return;
	}
	char c = (console_buffer+console_buffer_read)->c;
	console_buffer_read++;
	if(console_buffer_read >= CONSOLE_BUFFER_NUMBER){
		console_buffer_read = 0;
	}
	bool backscape = 0;
	if(c == '\r'){
		return;
	}else if(c == '\n'){
		console_return();
		return;
	}else if(c == '\t'){
		cursor_x = cursor_x+text_width*TAB_WIDTH-cursor_x%(text_width*TAB_WIDTH);
		cursor_flash();
		return;
	}else if(c == 0x8){
		c = 0x20;
		backscape = 1;
		cursor_x-=text_width;
		if(cursor_x < 0){
			cursor_x = screen_wid-screen_wid%text_width-text_width;
			if((cursor_y-text_height)>0){
				cursor_y-=text_height;
			}else{
				cursor_x = 0;
				cursor_y = 0;
			}
		}
	}
	console_char[console_text_width*(cursor_y/text_height)+(cursor_x/text_width)] = c;
	uint8_t cp;
	backup_eflags();
	cli();
	if((console+current_screen)->type == SCREEN_CONSOLE){
		show_char(cursor_x,cursor_y,c,pixel,1);
	}
	if(cursor_x > screen_wid){
		console_return();
		cursor_x = 0;
	}else{
		if(backscape == 0){
			cursor_x+=text_width;
		}
	}
	cursor_flash();
	restore_eflags();
}
void print_string(char *s){
	unsigned int len = strlen(s);
	for(int i=0;i<len;i++){
		print_char(s[i]);
	}
}
void print_hex(uint32_t num){
	char buffer[9];
	itox(num,buffer);
	print_string(buffer);
}
void print_int(uint32_t num){
	char buffer[11];
	itoa(num,buffer);
	print_string(buffer);
}
void clear_cursor(){
	if((console+current_screen)->type == SCREEN_CONSOLE){
		backup_eflags();
		cli();
		graphics_draw_hline(now_cursor_x,now_cursor_y+text_height-1,text_width,graphics_background_color);
		restore_eflags();
	}
}
void show_cursor(){
	if((console+current_screen)->type == SCREEN_CONSOLE){
		backup_eflags();
		cli();
		graphics_draw_hline(cursor_x,cursor_y+text_height-1,text_width,graphics_cursor_color);
		restore_eflags();
	}
}
void cursor_flash(){
	clear_cursor();
	show_cursor();
	now_cursor_x = cursor_x;
	now_cursor_y = cursor_y;
}