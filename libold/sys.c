#include <stdint.h>
#include <stdbool.h>
#include <graphics.h>
#include <sys.h>
uint8_t *mem_table;
mem_list_t *mem_list;
volatile gdt_t gdt;
void mem_init(){
	mem_size = get_memory_size();
	mem_table_size = (mem_size-MEM_USEABLE_ADDRESS)/(4*1024)/8;
	mem_table = (uint8_t *)MEM_TABLE_ADDRESS;
	mem_list = (mem_list_t *)MEM_LIST_ADDRESS;
	for(int i = 0;i<mem_table_size;i++){
		mem_table[i] = 0;
	}
	for(int i = 0;i<MEM_LIST_LENGTH;i++){
		(mem_list + i)->address = 0;
	}
}
uint8_t read_port(uint16_t port){
	uint8_t ret;
	__asm volatile("IN AL,DX\n\t"
:"=a"(ret)
:"d"(port)
);
	return ret;
}
void write_port(uint16_t port,uint8_t data){
	__asm volatile("OUT DX,AL\n\t"
	:
	:"a"(data),
	"d"(port)
	);
}
int get_eflags(){
	int eflags;
	__asm volatile("PUSHFD \n\t\
	POP EAX\n\t"
	:"=a"(eflags));
	return eflags;
}
void set_eflags(int eflags){
	__asm volatile("PUSH EAX\n\t\
POPFD\n\t"
:
:"a"(eflags));
}
bool is386(){
	int eflags = get_eflags();
	eflags |= 1<<18;
	set_eflags(eflags);
	eflags = get_eflags();
	if(eflags&(1<<18)){
		eflags&=~(1<<18);
		set_eflags(eflags);
		return 0;
	}else{
		return 1;
	}
}
void disable_cache(){
	if(!is386()){
		__asm volatile("MOV EAX,CR0\n\t\
		OR EAX,1<<30\n\t\
		AND EAX,~(1<<29)\n\t\
		MOV CR0,EAX\n\t"
		:
		:
		:"%eax");
	}
}
void enable_cache(){
	if(!is386()){
		__asm volatile("MOV EAX,CR0\n\t\
		AND EAX,~(1<<30)\n\t\
		AND EAX,~(1<<29)\n\t\
		MOV CR0,EAX\n\t"
		:
		:
		:"%eax");
	}
}
unsigned int get_memory_size(){
	disable_cache();
	volatile uint32_t *i = (uint32_t *)(1024*1024);
	volatile uint32_t data;
	while((uint32_t)i<(uint32_t)graphics_vram){
		data = *i;
		*i = 0x55AA55AA;
		if(*i != 0x55AA55AA){
			break;
		}
		*i ^= 0xFFFFFFFF;
		if(*i != 0xAA55AA55){
			break;
		}
		*i = data;
		i+=4*1024;
	}
	enable_cache();
	return (unsigned int)i;
}
void *malloc(uint32_t size){
	int size_in_4k = size/(4*1024);
	if(size%(4*1024)){
		size_in_4k++;
	}
	int z = 0;
	int start_address;
	for(int i = 0;i<mem_table_size;i++){
		if(mem_table[i] != 0xFF){
			for(int j = 0;j<8;j++){
				if(!(mem_table[i]>>j)&1){
					if(z == 0){
						start_address = i*8+j;
					}
					z++;
					if(z == size_in_4k){
						goto fin;
					}
				}else{
					z = 0;
				}
			}
		}
	}
	fin:
	if(z == size_in_4k){
		for(int i = 0;i<size_in_4k;i++){
			mem_table[(start_address+i)/8] |= 1<<((start_address+i)%8);
		}
		for(int i = 0;i<MEM_LIST_LENGTH;i++){
			if(((mem_list+i)->address) == 0){
				(mem_list+i)->address = start_address;
				(mem_list+i)->size_in_4k = size_in_4k;
				break;
			}
		}
		return (void *)(start_address*(4*1024)+(uint32_t)MEM_USEABLE_ADDRESS);
	}else{
		return NULL;
	}
}
void free(void *address){
	int start_address = ((uint32_t)address-(uint32_t)MEM_USEABLE_ADDRESS)/(4*1024);
	for(int i = 0;i<MEM_LIST_LENGTH;i++){
		if((mem_list+i)->address == (uint32_t)start_address){
			(mem_list+i)->address = 0;
			for(int j = 0;j<((mem_list+i)->size_in_4k);j++){
				mem_table[(start_address+j)/8] &= ~(1<<((start_address+j)%8));
			}
			break;
		}
	}
}
void set_gdt(unsigned int entry,unsigned int base,unsigned int limit,unsigned int mode){
	*((uint16_t *)(GDT_ADDRESS+entry*8)) = limit&0xFFFF;
	*((uint16_t *)(GDT_ADDRESS+entry*8+2)) = base&0xFFFF;
	*((uint8_t *)(GDT_ADDRESS+entry*8+4)) = (base>>16)&0xFF;
	*((uint16_t *)(GDT_ADDRESS+entry*8+5)) = (mode&0xF0FF)|((limit>>8)&0xF00);
	*((uint8_t *)(GDT_ADDRESS+entry*8+7)) = (base>>24)&0xFF;
}
void init_gdt(){
	set_gdt(0,0,0,0);
	set_gdt(1,0,0xFFFFF,SEG_C|SEG_32|SEG_G);
	set_gdt(2,0,0xFFFFF,SEG_DRW|SEG_32|SEG_G);
	for(int i = 3;i<8192;i++){
		set_gdt(i,0,0,0);
	}
	gdt.size = 8192*8-1;
	gdt.offset = GDT_ADDRESS;
	kernel_code_selector = 1*8;
	kernel_data_selector = 2*8;
	asm("LGDT [ebx]"
	:
	:"b"(&gdt));
}