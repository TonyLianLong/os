#include <keyboard.h>
#include <interrupt.h>
#include <graphics.h>
#include <console.h>
#include <string.h>
#include <sys.h>

int read_buffer_pointer;
int write_buffer_pointer;
const char keycode_list[] = {
0,0,'1','2','3','4','5','6','7','8','9','0','-','+',8,
'\t','q','w','e','r','t','y','u','i','o','p','[',']',
'\n',0,'a','s','d','f','g','h','j','k','l',';','\'',
'`',0,'\\','z','x','c','v','b','n','m',',','.','/',0,0,0,0x20
};
const char shift_keycode_list[] = {
0,0,'!','@','#','$','%','^','&','*','(',')','_','+',8,
'\t','Q','W','E','R','T','Y','U','I','O','P','{','}',
'\n',0,'A','S','D','F','G','H','J','K','L',':','\"',
'~',0,'|','Z','X','C','V','B','N','M','<','>','?',0,0,0,0x20
};
void keyboard_exec(){
	char text;
	uint8_t keycode = read_port(0x60);
	graphics_pixel_t pixel = {.R = 0xFF,.G = 0xFF,.B = 0xFF};
	if((keycode == LEFT_SHIFT_PRESS)||(keycode == RIGHT_SHIFT_PRESS)){
		shift_pressed = 1;
	}else if((keycode == LEFT_SHIFT_RELEASE)||(keycode == RIGHT_SHIFT_RELEASE)){
		shift_pressed = 0;
	}else if(keycode == CAPS_LOCK_RELEASE){
		CAPS_LOCK = !CAPS_LOCK;
		if(CAPS_LOCK){
			set_keyboard_LED(keyboard_LED|KEYBOARD_CAPS_LOCK_LED);
		}else{
			set_keyboard_LED(keyboard_LED&(~KEYBOARD_CAPS_LOCK_LED));
		}
	}else if(keycode == CONTROL_PRESS){
		control_pressed = 1;
	}else if(keycode == CONTROL_RELEASE){
		control_pressed = 0;
	}else if(control_pressed && keycode == F1_PRESS){
		change_screen(0);
	}else if(control_pressed && keycode == F2_PRESS){
		change_screen(1);
	}else if(keycode<(sizeof(keycode_list)/sizeof(char))){
		text = keycode_list[keycode];
		if((text >= 'a') && (text <= 'z')){
			if(shift_pressed ^ CAPS_LOCK){
				text = text-'a'+'A';
			}
		}else if(shift_pressed && (keycode&0x7F)<sizeof(shift_keycode_list)/sizeof(char)){
			text = shift_keycode_list[keycode&0x7F];
		}
		if((write_buffer_pointer+1)%KEYBOARD_BUFFER_SIZE != read_buffer_pointer){
			if(((console+current_screen)->type == SCREEN_CONSOLE) && (((console+current_screen)->flags)&CONSOLE_FLAGS_PRINT)){
				if(text == 0x8){
					if(((console+SCREEN_CONSOLE_ID)->flags)&CONSOLE_FLAGS_DELETE){
						print_char(0x8);
					}
				}else{
					print_char(text);
				}
			}
			if((console+current_screen)->keyboard_buffer != NULL){
				*((console+current_screen)->keyboard_buffer+write_buffer_pointer) = text;
			}
			write_buffer_pointer++;
			if(write_buffer_pointer >= KEYBOARD_BUFFER_SIZE){
				write_buffer_pointer = 0;
			}
		}
	}
}
void init_keyboard(){
	read_buffer_pointer = 0;
	write_buffer_pointer = 0;
	shift_pressed = 0;
	CAPS_LOCK = 0;
	keyboard_LED = 0;
	write_idt_entry(0x31,&keyboard_interrupt,INTERRUPT_GATE|INTERRUPT_NOT_FOR_PAGING);
}
char get_char(uint8_t screen){
	char c;
	if(read_buffer_pointer != write_buffer_pointer){
		c = *((console+screen)->keyboard_buffer+read_buffer_pointer);
		read_buffer_pointer++;
		if(read_buffer_pointer >= KEYBOARD_BUFFER_SIZE){
			read_buffer_pointer = 0;
		}
		return c;
	}else{
		return 0;
	}
}
int get_string(char *s,int length,uint8_t screen){
	char c;
	int i = 0;
	if(length){
		return i;
	}
	while((c = get_char(screen)) != 0){
		*(s+i) = c;
		i++;
		if(i >= length){
			break;
		}
	}
	return i;
}
void wait_KBC(){
	while(read_port(0x64)&0x2){
	}
}
void set_keyboard_LED(uint8_t LED){
	uint8_t status;
	set_LED_start:
	keyboard_LED = LED;
	wait_KBC();
	write_port(0x60,0xED);
	wait_KBC();
	write_port(0x60,LED);
	wait_KBC();
	while(1){
		status = read_port(0x60);
		if(status == 0xFA){
			break;
		}else if(status == 0xFE){
			goto set_LED_start;
			break;
		}
	}
}
uint8_t mouse_mode = 0;
uint8_t mouseclick = 0;
uint32_t mousex;
uint32_t mousey;
void mouse_exec(){
	uint8_t data = read_port(0x60);
	switch(mouse_mode){
		case 0:
			if(data == 0xFA){
				mouse_mode = 1;
			}
		break;
		case 1:
			mouseclick = data&(0b111);
			mouse_mode = 2;
		break;
		case 2:
			mousex+=(int8_t)data;
			if(mousex > screen_wid){
				mousex = screen_wid;
			}
			mouse_mode = 3;
		break;
		case 3:
			mousey+=(int8_t)data;
			if(mousey > screen_hei){
				mousey = screen_hei;
			}
			mouse_mode = 1;
		break;
	}
}
void init_mouse(){
	mousex = screen_wid/2;
	mousey = screen_hei/2;
	wait_KBC();
	write_port(0x64,0x60);
	wait_KBC();
	write_port(0x60,0x47);
	wait_KBC();
	write_port(0x64,0xD4);
	wait_KBC();
	write_port(0x60,0xF4);
	write_idt_entry(0x3C,&mouse_interrupt,INTERRUPT_GATE|INTERRUPT_NOT_FOR_PAGING);
}