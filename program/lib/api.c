void putchar(char l){
	asm("MOV EAX,1\n\t\
		MOV BL,%0\n\t\
		INT 0x20"
		:
		:"r"(l)
		:"%eax","%ebx");
}
void init_screen(){
	asm("MOV EAX,5\n\t\
		INT 0x20"
		:
		:
		:"%eax");
}
void write(){
	asm("MOV EAX,6\n\t\
		MOV EBX,0\n\t\
		MOV ECX,0x00FFFFFF\n\t\
		INT 0x20"
		:
		:
		:"%eax","%ebx","%ecx");
}
void flush(){
	asm("MOV EAX,7\n\t\
		INT 0x20"
		:
		:
		:"%eax");
}
int run(char *str){
	int pid;
	asm("MOV EAX,4\n\t\
		INT 0x20"
		:"=a"(pid)
		:"b"(str)
		:);
	return pid;
}