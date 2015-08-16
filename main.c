#include <stdint.h>
#define RAM_size 32*1024*1024
#define SCREEN_MAX_CHAR 80*25 //80*25 是屏幕上显示字符的最大个数
#define move_cursor 1//在输出完字符后移动光标 这个可以改成变量
#pragma pack(1)
volatile uint64_t *PML4T;
uint64_t *PDPT;
uint64_t *PDT;
uint64_t *PT;
uint64_t RAMbase;
uint64_t RAMlength;
uint64_t MaxRAMAddr;
uint16_t cursor_offset;
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
	asm("MOV DX,[0x463];\
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
void print_char(char c){
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
void print_hex(uint64_t hex){
	hex = 0xFFFFFFFF12345670;
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
int init_paging(){
	//63到47位的内存需要一样（都是0或都是1），所以不用47位和以上的位来进行寻址
	int pt_num;
	PML4T = (uint64_t *)0x10000;
	PDPT = (uint64_t *)0x11000;
	PDT = (uint64_t *)0x12000;
	PT = (uint64_t *)0x13000;
	if(RAM_size  < 1*1024*1024*1024){
		*PML4T = 0x0000000000011003;
		*PDPT = 0x0000000000012003;
		*PDT = 0x0000000000013003;
		pt_num = 1;//(MaxRAMAddr+1)/(4*1024);
	}else{
		//还不用
		panic("1:Memory is too large(>1GB).");
		return -1;
	}
	for(;pt_num>0;pt_num--){
		for(int i = 0;i<512;i++){
			*(PT+i) = (i<<12)|0x003;
			//指针是按个数，而不是按字节来运算
		}
	}
	asm("MOV CR3,RAX"::"a"(PML4T):);//这是按INTEL格式的内联汇编来写的
	return 0;
}
int main(){
	cursor_offset = 0;
	get_RAM_size();
	init_paging();
	print_char('A');
	print_char((uint8_t)('0'+RAMlength/1024/1024/4));
	update_cursor();
print_hex(1);
return 0;
}
