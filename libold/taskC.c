#include <task.h>
#include <interrupt.h>
#include <sys.h>
#include <file.h>

void set_LDT(unsigned int LDT_id,unsigned int entry,unsigned int base,unsigned int limit,unsigned int mode){
	*((uint16_t *)(LDT_BASE_ADDRESS+LDT_id*LDT_SIZE+entry*8)) = limit&0xFFFF;
	*((uint16_t *)(LDT_BASE_ADDRESS+LDT_id*LDT_SIZE+entry*8+2)) = base&0xFFFF;
	*((uint8_t *)(LDT_BASE_ADDRESS+LDT_id*LDT_SIZE+entry*8+4)) = (base>>16)&0xFF;
	*((uint16_t *)(LDT_BASE_ADDRESS+LDT_id*LDT_SIZE+entry*8+5)) = (mode&0xF0FF)|((limit>>8)&0xF00);
	*((uint8_t *)(LDT_BASE_ADDRESS+LDT_id*LDT_SIZE+entry*8+7)) = (base>>24)&0xFF;
}
void init_LDT(unsigned int LDT_id,uint8_t sys,uint32_t base,uint32_t limit){
	if(sys){
		set_LDT(LDT_id,0,base,limit,SEG_32|SEG_G|SEG_C);
		set_LDT(LDT_id,1,base,limit,SEG_32|SEG_G|SEG_DRW);
	}else{
		set_LDT(LDT_id,0,base,limit,SEG_32|SEG_G|SEG_C|SEG_RING3);
		set_LDT(LDT_id,1,base,limit,SEG_32|SEG_G|SEG_DRW|SEG_RING3);
	}
}
void init_TSS(unsigned int task_id,TSS_t *TSS,entry_t entry,unsigned int stack,uint8_t sys,uint32_t interrupt_sp){
	TSS->iomap = 0x40000000;
	TSS->eflags = get_eflags()|1<<9;//IF
	TSS->ldtr = (GDT_USEABLE_NUM+task_id*2+1)*8;
	TSS->eip = (uint32_t)entry;
	TSS->eax = 0;
	TSS->ecx = 0;
	TSS->edx = 0;
	TSS->ebx = 0;
	TSS->esp = stack;
	TSS->ebp = 0;
	TSS->esi = 0;
	TSS->edi = 0;
	TSS->ss0 = kernel_data_selector;
	TSS->esp0 = interrupt_sp;
	if(!sys){
		TSS->cs = (0*8)|0b111;
		TSS->ss = (1*8)|0b111;
		TSS->ds = (1*8)|0b111;
		TSS->es = (1*8)|0b111;
		TSS->fs = (1*8)|0b111;
		TSS->gs = (1*8)|0b111;
	}else{
		TSS->cs = (0*8)|0b100;
		TSS->ss = (1*8)|0b100;
		TSS->ds = (1*8)|0b100;
		TSS->es = (1*8)|0b100;
		TSS->fs = (1*8)|0b100;
		TSS->gs = (1*8)|0b100;
	}
}
void init_task(){
	task_is_ready = 0;
	task = malloc(MAX_TASK_NUMBER*sizeof(struct task_struct));
	for(int i = 0;i<MAX_TASK_NUMBER;i++){
		(task+i)->status = TASK_EMPTY;
		(task+i)->next = NULL;
	}
	end_task = task;
	current_task = task;
}
void create_task(unsigned int task_id,TSS_t *TSS,unsigned int LDT_id){
	set_gdt(GDT_USEABLE_NUM+task_id*2,(uint32_t)TSS,103,SEG_TSS_GATE|SEG_RING3);
	set_gdt(GDT_USEABLE_NUM+task_id*2+1,(uint32_t)(LDT_BASE_ADDRESS+LDT_id*LDT_SIZE),LDT_SIZE-1,SEG_LDT|SEG_RING3);
}
int run_program(char *filename,uint32_t limit4k){
	uint8_t *file_buffer;
	uint8_t file;
	uint32_t task_id;
	int loadelf_return;
	file_t fileinfo;
	entry_t entry;
	fileinfo = file_info(filename);
	uint8_t *task_base_address;
	if(fileinfo.status != 0){
		print_string("Read file failed.");
		return -1;
	}
	file_buffer = malloc(fileinfo.item.size);
	if(file_buffer == NULL){
		return -3;
	}
	file = read_file(filename,(uint8_t *)file_buffer,fileinfo.item.size);
	if(file != 0){
		print_string("Read file failed.");
		return -2;
	}
	entry = get_elf_entry((uint8_t *)file_buffer);
	free(file_buffer);
	if(limit4k > 0){
		task_base_address = (uint8_t *)malloc(limit4k*4096);
		if(task_base_address == NULL){
			return -3;
		}
		if(loadelf_return = loadelf((uint8_t *)file_buffer,(uint8_t *)task_base_address,fileinfo.item.size)){
			return -3-loadelf_return;
		}
		task_id = new_task((uint32_t)task_base_address,limit4k,entry,limit4k*4096);
	}else{
		task_base_address = (uint8_t *)malloc(DEFAULT_TASK_LIMIT_4K*4096);
		if(task_base_address == NULL){
			return -3;
		}
		print_hex((uint32_t)task_base_address);
		if(loadelf_return = loadelf((uint8_t *)file_buffer,(uint8_t *)task_base_address,fileinfo.item.size)){
			return -3-loadelf_return;
		}
		task_id = new_task((uint32_t)task_base_address,DEFAULT_TASK_LIMIT_4K,entry,DEFAULT_TASK_LIMIT_4K*4096);
	}
	return task_id;
}
void task_switch(unsigned int task_id){
	current_task = task+task_id;
	asm("PUSH AX\n\t\
		MOV AL,0x20\n\t\
		outb 0x20\n\t\
		outb 0x0A0\n\t\
		POP AX\n\t");
	_far_jmp(0,(task_id*2+GDT_USEABLE_NUM)*8);
}
void task_switch_no_update(unsigned int task_id){
	asm("PUSH AX\n\t\
		MOV AL,0x20\n\t\
		outb 0x20\n\t\
		outb 0x0A0\n\t\
		POP AX\n\t");
	_far_jmp(0,(task_id*2+GDT_USEABLE_NUM)*8);
}
uint8_t task_status(unsigned int task_id){
	if(task_id >= MAX_TASK_NUMBER){
		return -1;
	}
	return (task+task_id)->status;
}
int get_task_number(){
	int i;
	for(i = 0;i<MAX_TASK_NUMBER;i++){
		if((task+i)->status == TASK_EMPTY){
			return i;
		}
	}
	return -1;
}
int new_task(uint32_t base,uint32_t limit,entry_t entry,uint32_t stack){
	int task_number;
	if((task_number = get_task_number()) == -1){
		return -1;
	}
	(task+task_number)->kernel_stack = (uint8_t *)malloc(TASK_STACK_SIZE)+TASK_STACK_SIZE;
	init_LDT(task_number,0,base,limit-1);//4K
	init_TSS(task_number,&((task+task_number)->TSS),entry,stack,0,(uint32_t)((task+task_number)->kernel_stack));
	create_task(task_number,&((task+task_number)->TSS),task_number);
	end_task->next = task+task_number;
	(task+task_number)->status = TASK_RUN;
	(task+task_number)->next = NULL;
	(task+task_number)->base = (uint8_t *)base;
	(task+task_number)->limit_4k = limit;
	end_task = task+task_number;
	return task_number;
}
int remove_task(uint32_t task_id){
	uint8_t need_switch = 0;
	task_t *ltask = task;
	uint8_t find = 0;
	if(task_id == 0){ /*Cannot remove kernel task*/
		return 1;
	}
	/*Remove system is also OK.
	if(task_id == 1){
		while(1);
	}*/
	if(task_id >= MAX_TASK_NUMBER){
		return 2;
	}
	if((task+task_id)->status == TASK_EMPTY){
		return 3;
	}
	do{
		if(ltask->next == (task+task_id)){
			find = 1;
			break;
		}
		if(ltask->next == NULL){
			break;
		}
		ltask = ltask->next;
	}while((((uint32_t)ltask-(uint32_t)task)/sizeof(task_t))<MAX_TASK_NUMBER);
	if(find){
		ltask->next = (task+task_id)->next;
	}
	if(current_task == task+task_id){
		current_task = (task+task_id)->next;
		if(current_task == NULL){
			current_task = task;
		}
		need_switch = 1;
	}
	if((task+task_id)->base){
		free((task+task_id)->base);
		(task+task_id)->base = NULL;
	}
	(task+task_id)->next = NULL;
	(task+task_id)->status = TASK_EMPTY;
	if(need_switch){
		task_switch_no_update(((uint32_t)current_task-(uint32_t)task)/sizeof(task_t));
	}
	return 0;
}
void change_task(){
	if(current_task->next == NULL){
		if(current_task == task){
			return;
		}
		current_task = task;
	}else{
		current_task = current_task->next;
	}
	task_switch_no_update(((uint32_t)current_task-(uint32_t)task)/sizeof(task_t));
}