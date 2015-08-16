#include <floppy.h>
#include <stdint.h>
#include <interrupt.h>
#include <timer.h>
#include <string.h>

#define show_char_now(c) print_char(c);write_char(graphics_default_text_color)
volatile uint8_t floppy_motor;
volatile uint8_t floppy_interrupt_finished;
uint8_t dor_status;
uint8_t floppy_C;
uint8_t st0;
uint8_t st1;
uint8_t st2;
void floppy_disable_other_interrupt(){
	asm("PUSH AX\n\t\
	mov	al,0b10111111\n\t\
	out 0x21,al\n\t\
	mov	al,0b11111111\n\t\
	out	0xA1,al\n\t\
	POP AX");
}
void floppy_enable_other_interrupt(){
	asm("PUSH AX\n\t\
	mov	al,0b00000000\n\t\
	out 0x21,al\n\t\
	mov	al,0b00000000\n\t\
	out	0xA1,al\n\t\
	POP AX");
}
void init_floppy(){
	floppy_motor = 0;
	floppy_C = 0;
	cli();
	init_floppy_DMA();
	write_idt_entry(0x36,&floppy_interrupt,INTERRUPT_GATE|INTERRUPT_NOT_FOR_PAGING);
	sti();
	floppy_reset();
	add_timer(9,floppy_motor_enabled,0);
	//Delay 500ms
}
void init_floppy_DMA(){
	write_port(0xA,0b110);
	write_port(0xC,0xFF);
	write_port(0x81,0x0);//Page
	write_port(0x4,0x0);//Address
	write_port(0x4,0xA);//Address
	write_port(0xC,0xFF);
	write_port(0x5,0xFF);//Count
	write_port(0x5,0x1);//Count
	FLOPPY_SET_DMA_MODE_READ();
	write_port(0xA,0b010);
}
void floppy_motor_enabled(){
	floppy_motor = 1;
}
void turn_on_motor(){
	if(floppy_motor == 0){
		wait_for_FDC();
		dor_status |= FLOPPY_MOTOR_A;
		write_port(FLOPPY_DOR_REG,dor_status);
		add_timer(9,floppy_motor_enabled,0);
	}
}
void turn_off_motor(){
	if(floppy_motor){
		wait_for_FDC();
		dor_status &= ~FLOPPY_MOTOR_A;
		write_port(FLOPPY_DOR_REG,dor_status);
		floppy_motor = 0;
	}
}
void send_floppy_command(uint8_t data){
	while(!((read_port(FLOPPY_STATUS_REG)&0xC0) == 0x80));
	write_port(FLOPPY_DATA_REG,data);
}
void send_floppy_data(uint8_t data){
	while(!(read_port(FLOPPY_STATUS_REG)&0x80));
	write_port(FLOPPY_DATA_REG,data);
}
uint8_t read_floppy_data(){
	uint8_t ret;
	while(!(read_port(FLOPPY_STATUS_REG)&0x80));
	ret = read_port(FLOPPY_DATA_REG);
	return ret;
}
void floppy_reset(){
	uint16_t sensei_data;
	write_port(FLOPPY_DOR_REG,0x00);
	write_port(FLOPPY_DOR_REG,FLOPPY_MOTOR_A|FLOPPY_DMA|FLOPPY_WORK|FLOPPY_SELECT_A);
	dor_status = FLOPPY_MOTOR_A|FLOPPY_DMA|FLOPPY_WORK|FLOPPY_SELECT_A;
	floppy_interrupt_finished = 0;
	backup_eflags();
	sti();
	floppy_disable_other_interrupt();
	while(!floppy_interrupt_finished);
	floppy_enable_other_interrupt();
	restore_eflags();
    for (int i = 0;i<4;i++){
		sensei_data = floppy_check();
    }
	floppy_C = sensei_data&0xFF;
	st0 = sensei_data>>8;
	write_port(FLOPPY_DCR_REG,FLOPPY_500KBPS);
	floppy_set();
}
void floppy_seek(uint8_t C){
	uint16_t sensei_data;
	wait_for_FDC();
	while(!floppy_motor);
	send_floppy_command(FLOPPY_SEEK_CMD);
	send_floppy_data(FLOPPY_SELECT_A);
	send_floppy_data(C);
	floppy_C = C;
	floppy_interrupt_finished = 0;
	backup_eflags();
	sti();
	floppy_disable_other_interrupt();
	while(!floppy_interrupt_finished);
	floppy_enable_other_interrupt();
	restore_eflags();
	sensei_data = floppy_check();
	if((sensei_data&0xFF) != C){
		show_char_now('E');
	}
	if(sensei_data>>14){
		show_char_now('e');
	}
}
void floppy_move(){
	wait_for_FDC();
	while(!floppy_motor);
	send_floppy_command(FLOPPY_RESET_CMD);
	send_floppy_data(FLOPPY_SELECT_A);
	floppy_interrupt_finished = 0;
	backup_eflags();
	sti();
	floppy_disable_other_interrupt();
	while(!floppy_interrupt_finished);
	floppy_enable_other_interrupt();
	restore_eflags();
	floppy_check();
}
void floppy_set(){
	wait_for_FDC();
	send_floppy_command(FLOPPY_SPECIFY_CMD);
	send_floppy_data(0xCF);
	send_floppy_data(0x6);
}
uint8_t read_sector(uint8_t C,uint8_t H,uint8_t S){
	wait_for_FDC();
	while(!floppy_motor);
	if(floppy_C != C){
		floppy_seek(C);
	}
	init_floppy_DMA();
	send_floppy_command(FLOPPY_READ_CMD);
	send_floppy_data((H<<2)|FLOPPY_SELECT_A);
	send_floppy_data(C);
	send_floppy_data(H);
	send_floppy_data(S);
	send_floppy_data(2);
	send_floppy_data(1);//Sector Number
	send_floppy_data(0x1B);
	send_floppy_data(0xFF);
	floppy_interrupt_finished = 0;
	backup_eflags();
	sti();
	floppy_disable_other_interrupt();
	while(!floppy_interrupt_finished);
	floppy_enable_other_interrupt();
	restore_eflags();
	st0 = read_floppy_data();
	st1 = read_floppy_data();
	st2 = read_floppy_data();
	C = read_floppy_data();
	H = read_floppy_data();
	S = read_floppy_data();
	read_floppy_data();//Sector Number
	wait_for_FDC();
	if((st0>>6)||st1||st2){
		return 0;
	}
}
uint8_t floppy_read(uint8_t C,uint8_t H,uint8_t S){
	if(S != 18){
		read_sector(C,H,S+1);
	}else{
		if(H != 1){
			read_sector(C,H+1,0);
		}else if(C != 80){
			read_sector(C+1,0,0);
		}
	}
	memcpy((uint8_t *)0xA00,(uint8_t *)0xC00,0x200);
	read_sector(C,H,S);
	return 0;
}
uint16_t floppy_check(){
	uint16_t ret;
	uint16_t C;
	send_floppy_command(FLOPPY_CHECK_CMD);
	st0 = read_floppy_data();
	C = read_floppy_data();
	ret = (st0<<8)|C;
	return ret;
}
uint8_t floppy_read_address(uint32_t address){
	address /= 0x200;
	uint8_t cylinder = address/36;
	uint8_t head = (address/18)%2;
	uint8_t sector = address%18+1;
	return floppy_read(cylinder,head,sector);
}
void floppy_exec(){
	floppy_interrupt_finished = 1;
	return;
}
void wait_for_FDC(){
	while(1){
		uint8_t floppy_status = read_port(FLOPPY_STATUS_REG);
		if(!(floppy_status&(1<<4))){
			break;
		}
	}
}