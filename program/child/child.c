#include <stdint.h>
#include "api.h"
int main(){
	int o;
	char c;
	asm("MOV EAX,10\n\t\
		MOV EBX,0\n\t\
		INT 0x20"
		:"=a"(o)
		:
		:"%ebx");
	while(1){
		asm("MOV EAX,8\n\t\
		MOV EBX,0\n\t\
		INT 0x20"
		:"=a"(c)
		:
		:"%ebx");
		if(c > 0){
			putchar(c);
		}
	}
}
