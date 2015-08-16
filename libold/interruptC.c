#include <interrupt.h>
#include <stdint.h>
#include <sys.h>
#include <keyboard.h>
#include <task.h>
#include <graphics.h>
#include <console.h>
#include <string.h>

void _init_interrupt();
void write_idt_entry(unsigned int num,void(*entry)(),uint8_t type){
	idt_t idt;
	idt.zero = 0;
	idt.offset_1 = ((uint32_t)entry)&0xFFFF;
	idt.offset_2 = (((uint32_t)entry)>>16)&0xFFFF;
	idt.selector = kernel_code_selector;
	idt.type_attr = type;
	IDT_base_address[num] = idt;
	return;
}
void write_idt_entry_s(unsigned int num,unsigned int selector,void(*entry)(),uint8_t type){
	idt_t idt;
	idt.zero = 0;
	idt.offset_1 = ((uint32_t)entry)&0xFFFF;
	idt.offset_2 = (((uint32_t)entry)>>16)&0xFFFF;
	idt.selector = selector;
	idt.type_attr = type;
	IDT_base_address[num] = idt;
	return;
}
void init_interrupt(){
	_init_interrupt();
	for(int i = 0x0;i<0x20;i++){
		write_idt_entry(i,trap2,TRAP_GATE|INTERRUPT_NOT_FOR_PAGING);
	}
	for(int i = 10;i<=15;i++){
		write_idt_entry(i,trap,TRAP_GATE|INTERRUPT_NOT_FOR_PAGING);
	}
	write_idt_entry(8,trap,TRAP_GATE|INTERRUPT_NOT_FOR_PAGING);
	write_idt_entry(17,trap,TRAP_GATE|INTERRUPT_NOT_FOR_PAGING);
	for(int i = 0x20;i<256;i++){
		write_idt_entry(i,empty_interrupt,INTERRUPT_GATE|INTERRUPT_NOT_FOR_PAGING);
	}
	write_idt_entry(0x20,soft_interrupt,INTERRUPT_GATE|INTERRUPT_NOT_FOR_PAGING|INTERRUPT_NOT_PROTECTED);
	sti();
}
uint8_t *get_task_address(uint32_t address,uint32_t check){
	uint8_t *ret = (uint8_t *)(current_task->base)+address;
	if((address >= ((uint32_t)(current_task->limit_4k)+1)*4096)&&check){//Large than limit
		remove_task(((uint32_t)current_task-(uint32_t)task)/sizeof(task_t));
		return 0;
	}
	if(check == 2){
		if(address+strlen(ret) >= ((uint32_t)(current_task->limit_4k)+1)*4096){
			remove_task(((uint32_t)current_task-(uint32_t)task)/sizeof(task_t));
			return 0;
		}
	}
	return ret;
}
uint32_t soft_interrupt_exec(){
	uint32_t eax,ebx,ecx;
	uint32_t ret;
	asm("MOV %0,EAX":"=r"(eax)::"%eax");
	asm("MOV %0,EBX":"=r"(ebx)::"%ebx");
	asm("MOV %0,ECX":"=r"(ecx)::"%ecx");
	ret = 0;
	if(eax == 1){//Print
		print_char((char)ebx);
		ret = 0;
	}else if(eax == 2){//Print string
		print_string(get_task_address(ebx,2));
		ret = 0;
	}else if(eax == 0xFF){
		ret = remove_task(((uint32_t)current_task-(uint32_t)task)/sizeof(task_t));
	}else if(eax == 3){//Get task ID
		ret = ((uint32_t)current_task-(uint32_t)task)/sizeof(task_t);
	}else if(eax == 4){//Run program from pointer
		//print_hex((uint32_t)get_task_address(ebx,2));
		//print_string(get_task_address(ebx,2));
		ret = run_program(get_task_address(ebx,2),0);
		//print_hex(-ret);
	}else if(eax == 5){//Add VRAM segment to LDT and get console control
		if(task_status((console+SCREEN_DESKTOP_ID)->control_task) == TASK_EMPTY){
			(console+SCREEN_DESKTOP_ID)->control_task = 0;
		}
		if((console+SCREEN_DESKTOP_ID)->control_task == 0){
			(console+SCREEN_DESKTOP_ID)->control_task = ((uint32_t)current_task-(uint32_t)task)/sizeof(task_t);
			set_LDT(((uint32_t)current_task-(uint32_t)task)/sizeof(task_t),2,(uint32_t)graphics_desktop_buffer,screen_wid*screen_hei*3-1,SEG_32|SEG_DRW|SEG_RING3);
			ret = 2;//LDT
		}else{
			ret = -1;//Another task alreasy has this LDT.
		}
	}else if(eax == 6){//Change a pixel if you have VRAM segment
		if((console+SCREEN_DESKTOP_ID)->control_task == (((uint32_t)current_task-(uint32_t)task)/sizeof(task_t))){//This task is able to edit VRAM.
			graphics_pixel_t pixel;
			ret = 0;
			if((uint8_t)(ecx>>24) == 0){
				pixel.R = (uint8_t)(ecx>>16);
				pixel.G = (uint8_t)(ecx>>8);
				pixel.B = (uint8_t)ecx;
			}else if((uint8_t)(ecx>>24) == 1){
				pixel = graphics_default_text_color;
			}else if((uint8_t)(ecx>>24) == 2){
				pixel = graphics_cursor_color;
			}else if((uint8_t)(ecx>>24) == 3){
				pixel = graphics_background_color;
			}else{
				pixel.R = 0xFF;
				pixel.G = 0xFF;
				pixel.B = 0xFF;
				ret = 2;
			}
			desktop_buffer_pixel_write(((uint16_t)(ebx>>16)),(uint16_t)ebx,pixel);
		}else{
			ret = 1;
		}
	}else if(eax == 7){//Flush VRAM from buffer
		if(((console+SCREEN_DESKTOP_ID)->control_task == (((uint32_t)current_task-(uint32_t)task)/sizeof(task_t)))&&((console+current_screen)->type == SCREEN_DESKTOP)){
			desktop_screen_flush();
			ret = 0;
		}else{
			ret = 1;
		}
	}else if(eax == 8){//Get console keyboard input
		if(ebx >= CONSOLE_BUFFER_NUMBER){
			ret = -2;
		}else{
			if(((console+ebx)->control_task == (((uint32_t)current_task-(uint32_t)task)/sizeof(task_t)))&&((console+current_screen)->type == SCREEN_CONSOLE)){
				ret = get_char(ebx);
			}else{
				ret = -1;
			}
		}
	}else if(eax == 9){//Get desktop keyboard input
		if(((console+SCREEN_DESKTOP_ID)->control_task == (((uint32_t)current_task-(uint32_t)task)/sizeof(task_t)))&&((console+current_screen)->type == SCREEN_DESKTOP)){
			ret = get_char(SCREEN_DESKTOP_ID);
		}else{
			ret = -1;
		}
	}else if(eax == 10){//Get console control
		if(ebx >= CONSOLE_BUFFER_NUMBER){
			ret = -2;
		}else{
			if((console+ebx)->type != SCREEN_CONSOLE){
				ret = -3;
			}else{
				if(task_status((console+ebx)->control_task) == TASK_EMPTY){//If the control task is stopped.
					(console+ebx)->control_task = 0;
				}
				if((console+ebx)->control_task == 0){
					(console+ebx)->control_task = ((uint32_t)current_task-(uint32_t)task)/sizeof(task_t);
					ret = ebx;
				}else{
					ret = -1;//Another task alreasy controls this console.
				}
			}
		}
	}else if(eax == 11){//Set keyboard LED
		set_keyboard_LED((uint8_t)ebx);
		ret = 1;
	}else if(eax == 12){//Enable delete
		ret = console_enable_delete(ebx);
	}else if(eax == 13){//Disable delete
		ret = console_disable_delete(ebx);
	}else if(eax == 14){//Get console flags
		if(ebx >= CONSOLE_BUFFER_NUMBER){
			ret = -2;
		}else{
			if(((console+ebx)->control_task == (((uint32_t)current_task-(uint32_t)task)/sizeof(task_t)))&&((console+current_screen)->type == SCREEN_CONSOLE)){
				ret = (console+ebx)->flags;
			}else{
				ret = -1;
			}
		}
	}else if(eax == 15){//Set console flags
		if(ebx >= CONSOLE_BUFFER_NUMBER){
			ret = -2;
		}else{
			if(((console+ebx)->control_task == (((uint32_t)current_task-(uint32_t)task)/sizeof(task_t)))&&((console+current_screen)->type == SCREEN_CONSOLE)){
				(console+ebx)->flags = ecx;
				ret = (console+ebx)->flags;
			}else{
				ret = -1;
			}
		}
	}else if(eax == 16){//Remove console control
		if(ebx >= CONSOLE_BUFFER_NUMBER){
			ret = -2;
		}else{
			if((console+ebx)->type != SCREEN_CONSOLE){
				ret = -3;
			}else{
				if((console+ebx)->control_task != 0){
					(console+ebx)->control_task = 0;
				}else{
					ret = -1;//It's already empty.
				}
			}
		}
	}else if(eax == 17){//Get task status
		if(ebx >= MAX_TASK_NUMBER){
			ret = -1;
		}else{
			ret = task_status(ebx);//So it will not return 0xFF
		}
	}
	return ret;
}
void trap_exec(int *esp){
	print_string("Error ");
	print_hex(esp[10]);
	print_string(" Address:");
	print_hex(esp[12]);
	print_char(':');
	print_hex(esp[11]);
	print_char('\n');
	remove_task(((uint32_t)current_task-(uint32_t)task)/sizeof(task_t));
}
void trap2_exec(int *esp){
	print_string("Error address:");
	print_hex(esp[11]);
	print_char(':');
	print_hex(esp[10]);
	print_char('\n');
	remove_task(((uint32_t)current_task-(uint32_t)task)/sizeof(task_t));
}