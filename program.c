int main(){
	*((unsigned char *)0x100) = 0x40;
	char c = 'a';
	asm("MOV EAX,1\n\t\
		MOV BL,%0\n\t\
		INT 0x20"
		:
		:"r"(c)
		:"%eax");
	while(1);
}
