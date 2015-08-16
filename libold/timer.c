#include <timer.h>
#include <sys.h>
#include <stdint.h>
#include <stdbool.h>
#include <interrupt.h>
timer_t *timer_list;
uint32_t timer_list_id;
void timer_exec(){
	for(;timer_list_id<TIMER_LIST_NUMBER;timer_list_id++){
		if(!((timer_list+timer_list_id)->exist)){
			continue;
		}
		((timer_list+timer_list_id)->time)--;
		if((timer_list+timer_list_id)->time == 0){
			if((timer_list+timer_list_id)->loop){
				(timer_list+timer_list_id)->time = (timer_list+timer_list_id)->default_time;
			}else{
				(timer_list+timer_list_id)->exist = 0;
			}
			((timer_list+timer_list_id)->entry)();
		}
	}
	timer_list_id = 0;
}

void init_timer(){
	write_port(0x43,0b00110100);
	write_port(0x40,(uint8_t)(1193180/CLOCK_FREQUENCY));
	write_port(0x40,(uint8_t)((1193180/CLOCK_FREQUENCY)>>8));
	timer_list_id = 0;
	timer_list = (timer_t *)TIMER_LIST_ADDRESS;
	for(int i = 0;i<TIMER_LIST_NUMBER;i++){
		(timer_list+i)->exist = 0;
	}
	write_idt_entry(0x30,&timer_interrupt,INTERRUPT_GATE|INTERRUPT_NOT_FOR_PAGING);
}
uint16_t get_pit_count(){
	uint16_t count;
	cli();
	asm("mov eax,0\n\t\
	out 0x43, al\n\t\
	in al, 0x40\n\t\
	mov ah, al\n\t\
	in al, 0x40\n\t\
	rol ax, 8"
	:"=a"(count));
	sti();
	return count;
}
void delay10ms(uint32_t time){
	uint32_t pit_count = get_pit_count();
	uint32_t pit_count2;
	while(1){
		pit_count2 = get_pit_count();
		if(pit_count2<pit_count){
			pit_count2+=1193180/CLOCK_FREQUENCY;
		}
		time-=pit_count2 - pit_count;
		if(time<=0){
			return;
		}
		pit_count = pit_count2;
	}
}
void play_sound(uint32_t nFrequence){
 	uint32_t div;
 	uint8_t tmp;
 	div = 1193180/nFrequence;
 	write_port(0x43, 0xb6);
 	write_port(0x42,(uint8_t)(div));
 	write_port(0x42,(uint8_t)(div>>8));
    //And play the sound using the PC speaker
 	tmp = read_port(0x61);
  	if(tmp != (tmp|0b11)){
 		write_port(0x61,tmp|0b11);
 	}
}
void nosound(){
	uint8_t tmp = read_port(0x61) & 0xFC;
	write_port(0x61, tmp);
}
void set_pit_count(uint16_t count){
	asm("out 0x40, al\n\t\
	rol ax, 8\n\t\
	out 0x40, al\n\t\
	rol ax, 8"
	:
	:"a"(count));
}
void set_timer(uint8_t num,unsigned int time,void(*entry)(),bool loop){
	(timer_list+num)->exist = 1;
	(timer_list+num)->time = time;
	(timer_list+num)->default_time = time;
	(timer_list+num)->entry = entry;
	(timer_list+num)->loop = loop;
}
int add_timer(unsigned int time,void(*entry)(),bool loop){
	for(int i = 0;i<TIMER_LIST_NUMBER;i++){
		if(!((timer_list+i)->exist)){
			set_timer(i,time,entry,loop);
			return i;
		}
	}
	return -1;
}
bool istimer(uint8_t num){
	return (timer_list+num)->exist;
}
void clear_timer(uint8_t num){
	(timer_list+num)->exist = 0;
}