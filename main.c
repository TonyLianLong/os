#include <stdint.h>
#define NULL ((void*)0)
#define RAM_size 32*1024*1024
#define SCREEN_MAX_CHAR 80*25 //80*25 是屏幕上显示字符的最大个数
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define move_cursor 1//在输出完字符后移动光标 这个可以改成变量
#pragma pack(1)
#define MEM_TABLE_MAX_NUMBER 64
#define PML4T (uint64_t *)0x100000
#define PAGE_BASE 0x101000 //在PML4T之后
uint64_t RAMbase;
uint64_t RAMlength;
uint64_t MaxRAMAddr;
uint64_t MemoryTableAddr;
uint16_t cursor_offset;
#define MEM_TABLE_FLAG_INITED (1<<0)
//INITED - FREE OR USED
#define MEM_TABLE_FLAG_USED (1<<1)
//USED - malloced
#define DO_NOT_CLEAR_UNINITED_MEM_TABLE_ENTRY 0
//清除未初始化的内存分配表的项目
uint64_t interrupt_flag;//如果是0那么保存时的中断位是0，如果是1<<9那么保存时中断位是1
struct mem_table {
	uint64_t start;
	uint64_t length;
	uint64_t flags;
	struct mem_table *next;
};
typedef struct mem_table mem_table_t;
mem_table_t *mem_table;
struct ramsize_struct {
	uint64_t base;
	uint64_t length;
	uint32_t type;
	uint32_t acpi_ext;
};
void panic(char *str){
	*(uint8_t *)0xB8000 = 'E';
	*(uint8_t *)0xB8001 = 0x07;
	*(uint8_t *)0xB8002 = str[0];
	*(uint8_t *)0xB8003 = 0x07;
	asm("MOV DX,[0x463]\n\
MOV AL,0x0F\n\
OUT DX,AL\n\
PUSH DX\n\
MOV DX,0x3D5\n\
MOV AL,0x02\n\
OUT DX,AL\n\
POP DX\n\
MOV AL,0x0E\n\
OUT DX,AL\n\
MOV DX,0x3D5\n\
MOV AL,0x00\n\
OUT DX,AL"
:
:
:"%al","%dx");
asm("CLI\n\
HLT");
}
inline void noint(){
	asm("pushfq\n\
	popq %0\n\
	and %0,(1<<9)\n\
	cli":"=r"(interrupt_flag));
}
inline void restoreint(){
	asm("pushfq\n\
	or QWORD[esp],%0\n\
	popf"::"r"(interrupt_flag));
}
void print_char(char c){
	if(c == '\n'){
		cursor_offset += SCREEN_WIDTH-cursor_offset%SCREEN_WIDTH;//换行
		//update_cursor();//更新光标
		return;
	}
	if(c == '\r'){
		return;//忽略
	}
	*(uint8_t *)((uint64_t)0xB8000+(uint64_t)cursor_offset*2) = c;
	*(uint8_t *)((uint64_t)0xB8001+(uint64_t)cursor_offset*2) = 0x07;
	cursor_offset++;
	if(cursor_offset == SCREEN_MAX_CHAR){
		cursor_offset = 0;
		panic("2:Too many characters are there on the screen.");
	}
}
void update_cursor(){
	asm("PUSH AX\n\
		MOV DX,[0x463]\n\
		MOV AL,0x0F\n\
		OUT DX,AL\n\
		POP AX\n\
		PUSH DX\n\
		MOV DX,0x3D5\n\
		OUT DX,AL\n\
		POP DX\n\
		MOV AL,0x0E\n\
		OUT DX,AL\n\
		MOV DX,0x3D5\n\
		MOV AL,AH\n\
		OUT DX,AL"
		:
		:"a"(cursor_offset)
		:"%dx");
	
}
void print_str(char *str){
	while(*str != '\0'){
		print_char(*(str++));
	}
	update_cursor();
}
void print_hex(uint64_t hex){
	int n;
	for(int i = 15;i >= 0;i--){//i不可以用无符号整型 i的初始值是hex的长度-1
		n = (hex>>(i*4))&0xF;
		if(n<10){
			print_char('0'+n);
		}else{
			print_char('a'+n-10);
		}
	}
	update_cursor();
}

int init_mem_table(){
	mem_table = (mem_table_t *)MemoryTableAddr;
	mem_table->start = RAMbase;//起始地址，第一部分是给kernel的，系统会用malloc来占用，防止被其他程序使用
	mem_table->length = RAMlength;
	mem_table->flags = MEM_TABLE_FLAG_INITED;
	mem_table->next = NULL;
	for(unsigned int i = 1;i<MEM_TABLE_MAX_NUMBER;i++){
		(mem_table+i)->flags= 0;
		#if !DO_NOT_CLEAR_UNINITED_MEM_TABLE_ENTRY
			(mem_table+i)->start = 0;
			(mem_table+i)->length = 0;
			(mem_table+i)->next = NULL;
		#endif
	}
	return 0;
}
inline mem_table_t *search_for_not_inited_mem_table(){
	//inline可以当作内置的函数，因为不需要call，所以可以加速
	for(unsigned int i = 1;i<MEM_TABLE_MAX_NUMBER;i++){
		if((((mem_table+i)->flags)&MEM_TABLE_FLAG_INITED) == 0){
			return mem_table+i;
		}
	}
	panic("5:Memory table is full.");
	return NULL;
}
void *malloc(unsigned int size){
	noint();
	unsigned int i;
	mem_table_t *mem_table_now = mem_table;
	mem_table_t *nexttable;
	for(i = 0;i<MEM_TABLE_MAX_NUMBER;i++){
		if(!((mem_table_now->flags)&MEM_TABLE_FLAG_INITED)){
			panic("0:Uninited entry in linktable.");
			restoreint();
			return NULL;
		}
		if((!((mem_table_now->flags)&MEM_TABLE_FLAG_USED))){
			if((mem_table_now->length)>size){
				//size 10
				//... 30 ...
				//    FREE
				//... 10 20 ...
				//  USED FREE
				nexttable = search_for_not_inited_mem_table();
				if(nexttable == NULL){
					restoreint();
					return NULL;
				}
				mem_table_now -> flags |= MEM_TABLE_FLAG_USED;
				nexttable -> flags |= MEM_TABLE_FLAG_INITED;
				nexttable -> start = mem_table_now -> start + size;
				nexttable -> length = mem_table_now -> length - size;
				mem_table_now -> length = size;
				nexttable -> next = mem_table_now -> next;
				mem_table_now -> next = nexttable;
				restoreint();
				return (void *)(mem_table_now->start);
			}else if((mem_table_now->length) == size){
				mem_table_now -> flags |= MEM_TABLE_FLAG_USED;
				restoreint();
				return (void *)(mem_table_now->start);
			}
		}
		if(mem_table_now -> next != NULL){
			mem_table_now = mem_table_now -> next;
		}else{
			break;
		}
	}
	if(i == MEM_TABLE_MAX_NUMBER){//Kernel Error 内核出错，项目数量超过最大值了
		panic("4:The number of the entries is greater than the max number.");
		restoreint();
		return NULL;
	}
	restoreint();
	return NULL;
}
void free(void *addr){
	noint();
	if(addr == NULL){
		restoreint();
		return;//释放NULL，不产生效果
	}
	unsigned int i;
	mem_table_t *mem_table_last = NULL;
	mem_table_t *mem_table_now = mem_table;
	for(i = 0;i<MEM_TABLE_MAX_NUMBER;i++){
		if(!((mem_table_now->flags)&MEM_TABLE_FLAG_INITED)){//项目链表中有未初始化的项目
			panic("6:There are entries which are uninited in the linktable.");
		}
		if((mem_table_now -> start) == (uint64_t)addr){
			break;
		}
		if((mem_table_now -> next) == NULL){
			panic("9:No entry which starts at this address.");//已经搜索到末尾了，没有在这个地址开始的项目
			restoreint();
			return;
		}else{
			mem_table_last = mem_table_now;
			mem_table_now = mem_table_now -> next;
		}
	}	
	if(mem_table_last == NULL){//这是第一个项目
		//一般内核项目的起始地址是0，所以不会出现下面的错误，因为如果释放0地址就当作释放NULL，没有效果，但是如果起始地址不是0而又要释放内核项目，就会出现下面的错误
		panic("8:You can't free the kernel entry.");
		restoreint();
		return;//You can't free the kernel entry(Paging,memory alloc table...)
		//你不能释放内核项目，这里有分页，内存分配表等
	}
	if(i == MEM_TABLE_MAX_NUMBER){//Kernel Error 内核出错，项目数量超过最大值了
		panic("4:Entries' number is greater than the max number.");
		restoreint();
		return;
	}
	if(!((mem_table_now->flags)&MEM_TABLE_FLAG_USED)){//要释放的项目没有被使用
		panic("7:Try to free unused item.");
		restoreint();
		return;
	}
	if(((mem_table_now->next->flags)&MEM_TABLE_FLAG_USED) || (mem_table_now->next == NULL)){//下一个项目被使用了或者不可用，不合并这个和下一个项目
		// ... 10 20 ...
		//   USED USED
		//     ->
		// ... 10 20 ...
		//   FREE USED
		//mem_table_now->flags &= ~MEM_TABLE_FLAG_USED;
		mem_table_now->flags = MEM_TABLE_FLAG_INITED;//去掉当前项目的使用标志，只保留初始化标志
	}else if((mem_table_now -> next -> start) == ((mem_table_now -> start) + (mem_table_now -> length))){
		//要在相同的地址区域才能合并，下一个项目的地址要是这个项目的地址加上这个项目的大小
		//下一个项目没有被使用，可以合并这个和下一个项目
		// ... 10 20 ...
		//   USED FREE
		//     ->
		// ... 30 ...
		//    FREE
		mem_table_now -> length += mem_table_now->next->length;
		//mem_table_now -> flags &= ~MEM_TABLE_FLAG_USED;
		mem_table_now -> flags = MEM_TABLE_FLAG_INITED;//去掉当前项目的使用标志，只保留初始化标志
		mem_table_now -> next -> flags = 0;//去除下一项的初始化标志
		mem_table_t *mem_table_next = mem_table_now -> next -> next;//记录下一项的下一项的地址
		#if !DO_NOT_CLEAR_UNINITED_MEM_TABLE_ENTRY
			mem_table_now -> next -> start = 0;
			mem_table_now -> next -> length = 0;
			mem_table_now -> next -> next = NULL;
		#endif
		mem_table_now -> next = mem_table_next;
	}
	if((!((mem_table_last->flags)&MEM_TABLE_FLAG_USED)) && ((mem_table_now->start) == ((mem_table_last -> start)+(mem_table_last -> length)))){//上一个项目是可用的并且上一个项目和这个项目在同一个地址区域，那么可以合并项目
		//... 10 20    10...
		//  FREE USED USED
		//       ->
		//... 30 10 ...
		//  FREE USED
		mem_table_last -> length += mem_table_now -> length;
		//mem_table_now -> flags &= ~MEM_TABLE_FLAG_INITED;
		mem_table_now -> flags = 0;//去掉当前项目的初始化标志，删除所有标志
		mem_table_last -> next = mem_table_now -> next;//让上一个项目指向下一个项目
		#if !DO_NOT_CLEAR_UNINITED_MEM_TABLE_ENTRY
			mem_table_now -> start = 0;
			mem_table_now -> length = 0;
			mem_table_now -> next = NULL;
			//把当前项目清空（如果代码没有问题也可以不清空，因为以后会初始化，为了避免以后发生问题并且方便调试，也可以清空）
		#endif
	}
	restoreint();
}
void print_mem_table(unsigned int number){//按内存分配表中的位置输出
	noint();
	if(number > MEM_TABLE_MAX_NUMBER){
		print_str("Memory table entry read number is too big,change to the max number.");
	}
	if((number == 0)||(number > MEM_TABLE_MAX_NUMBER)){
		number = MEM_TABLE_MAX_NUMBER;
	}
	for(unsigned int i = 0;i<number;i++){
		print_str("Memory Table Number:");
		print_hex(i);
		print_str(" Start:");
		print_hex((mem_table+i)->start);
		print_str(" Length:");
		print_hex((mem_table+i)->length);
		print_str(" Flag:");
		print_hex((mem_table+i)->flags);
		print_char('\n');
	}
	restoreint();
}
void print_mem_table_link(unsigned int number){//按链表顺序输出
	noint();
	if(number > MEM_TABLE_MAX_NUMBER){
		print_str("Memory table entry read number is too big,change to the max number.");
	}
	if((number == 0)||(number > MEM_TABLE_MAX_NUMBER)){
		number = MEM_TABLE_MAX_NUMBER;
	}
	mem_table_t *mem_table_now = mem_table;
	unsigned int i;
	for(i = 0;i<number;i++){
		print_str("Memory Table Number:");
		print_hex(i);
		print_str(" Start:");
		print_hex(mem_table_now->start);
		print_str(" Length:");
		print_hex(mem_table_now->length);
		print_str(" Flag:");
		print_hex(mem_table_now->flags);
		print_char('\n');
		if(mem_table_now->next == NULL){
			restoreint();
			return;
		}
		mem_table_now = mem_table_now->next;
	}
	if(i == MEM_TABLE_MAX_NUMBER){
		panic("4:Entries' number is greater than the max number.");
	}
	restoreint();
}
int get_RAM_size(){
	//这个函数是从原来在内存中储存的BIOS数据（由INT 0x15 EAX=0xE820获得）里读取出内存大小，数据获取代码在启动文件内
	//0x5000 开始是内存分配数据
	//0x5FFD 16bit 最后一个指针
	//0x5FFF 8bit 每个项目长度
	int maxaddr = *((uint16_t *)0x5FFD);
	int datalength = *((uint8_t *)0x5FFF);
	RAMlength = 0;
	for(struct ramsize_struct* ramsize_addr = (struct ramsize_struct*)0x5000;(uint64_t)ramsize_addr < (uint64_t)maxaddr;ramsize_addr++){
		if(ramsize_addr->type == 1){
			if(datalength == 24 && ((ramsize_addr->acpi_ext)&1)){
				//如果所有项目是24字节的就查看ACPI 3.0的第0位，如果是0就说明这个项目要被忽略
				continue;
			}
			if(ramsize_addr->length > RAMlength){//选择最大的可用内存
				RAMbase = ramsize_addr->base;
				RAMlength = ramsize_addr->length;
			}
		}
		if((RAMbase+RAMlength) > MaxRAMAddr){
			MaxRAMAddr = RAMbase+RAMlength;//最大能访问的物理内存地址，这有可能是特殊数据区，不能给普通数据存储使用
		}
	}
	if(RAMlength == 0){
		//不能得到内存大小
		panic("0:Can't get RAM size.");
		return -1;
	}
	return MaxRAMAddr;
	//返回物理内存大小
}
//inline 
int init_paging(){
	//63到47位的虚拟地址需要一样（都是0或都是1），所以不用47位和以上的位来进行寻址
	unsigned int PDPT_number;
	if(MaxRAMAddr  < 1*1024*1024*1024){
		PDPT_number = 1;
	}else{
		if(MaxRAMAddr%(1*1024*1024*1024) == 0){
			PDPT_number = (MaxRAMAddr+1)/(1*1024*1024*1024);
		}else{
			PDPT_number = (MaxRAMAddr+1)/(1*1024*1024*1024)+1;
		}
		//panic("1:Memory is too large(>1GB).");
	}
	*PML4T = (PAGE_BASE+(PDPT_number+1)*512*8)|0x003;
	for(unsigned int PDPT_entry = 0;PDPT_entry < PDPT_number;PDPT_entry++){
		for(unsigned int PDT_entry = 0;PDT_entry < 512;PDT_entry++){
			*(uint64_t *)((uint64_t)(PAGE_BASE+PDPT_entry*512*8+PDT_entry*8)) = (uint64_t)(((PDPT_entry*512+PDT_entry)<<21)|0x0A3);
			//*(uint64_t *)((uint64_t)(0x120000+(a<<12<<9)+((a+b)<<12))) = (0x120000+(a<<12<<9)+((a+b)<<12)+0x1000)|0x003;
		}
		*(uint64_t *)((uint64_t)(PAGE_BASE+(PDPT_number+1)*512*8+PDPT_entry*8)) = (uint64_t)((PAGE_BASE+PDPT_entry*512*8)|0x003);
	}
	MemoryTableAddr=PAGE_BASE+(PDPT_number+1)*512*8+PDPT_number*8;
	asm("MOV CR3,%0"::"r"(PML4T):);//这是按INTEL格式的内联汇编来写的
	return 0;
}
inline void init_interrupt(){
	asm("mov al,0b00000000/*启动8259的所有中断*/\n\
out 0x21,al/*Master 8259A,OCW1*/\n\
out	0xA1,al/*Slave 8259A,OCW1*/\n\
mov al,0x11\n\
out 0x20,al/*开始*/\n\
mov al,0x20/*偏移*/\n\
out 0x21,al/*ICW2*/\n\
mov al,1<<2\n\
out 0x21,al/*ICW3*/\n\
mov al,1\n\
out 0x21,al/*ICW4*/\n\
mov al,0x11\n\
out 0xA0,al\n\
mov al,0x28\n\
out 0xA1,al\n\
mov al,1<<2\n\
out 0xA1,al\n\
mov al,1\n\
out 0xA1,al":::"%al");
	
}
char *a;
int *b;
int main(){
	cursor_offset = 0;
	get_RAM_size();
	init_paging();
	init_mem_table();
	if((MemoryTableAddr+sizeof(mem_table)*MEM_TABLE_MAX_NUMBER)<=RAMbase){
		panic("3:Kernel's table end address is smaller than RAM's useable base.");
		//It can be disabled if you are sure it is right and you don't want to use that part of RAM
		malloc(0);//占用第一部分内存，防止被其他程序占用，大小为0
	}else{
		/*print_str("Kernel memory alloc usable address starts at:0x");
		print_hex(MemoryTableAddr+sizeof(mem_table)*MEM_TABLE_MAX_NUMBER);
		print_char('\n');*/
		malloc(MemoryTableAddr+sizeof(mem_table)*MEM_TABLE_MAX_NUMBER-RAMbase);//获取系统部分的内存，在这之前不可以用malloc获取内存
	}
	a = (char *)malloc(1);
	b = (int *)malloc(sizeof(int));
	print_mem_table_link(4);
	free(a);
	print_mem_table_link(4);
	free(b);
	print_mem_table_link(4);
	init_interrupt();
	/*print_char('A');
	print_char((uint8_t)('0'+RAMlength/1024/1024/4));*/
	update_cursor();
	return 0;
}
