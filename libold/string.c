#include <string.h>
#include <stdint.h>
#include <console.h>

unsigned int strlen(char *s){
	volatile int len = 0;
	for(len = 0;*(s+len);len++);
	return len;
}
int memcmp(void *buf1,void *buf2, unsigned int count){
	while(count){
		if((*((uint8_t *)buf1)) != (*((uint8_t *)buf2))){
			return (*((uint8_t *)buf1))-(*((uint8_t *)buf2));
		}
		buf1++;
		buf2++;
		count--;
	}
	return 0;
}
int memicmp(void *buf1,void *buf2, unsigned int count){
	while(count){
		if(tolower((*((uint8_t *)buf1))) != tolower((*((uint8_t *)buf2)))){
			return tolower((*((uint8_t *)buf1))) - tolower((*((uint8_t *)buf2)));
		}
		buf1++;
		buf2++;
		count--;
	}
	return 0;
}
void *memcpy(void *dest,const void *src,unsigned int len){
	for(int i = 0;i<len;i++){
		*((uint8_t *)(dest+i)) = *((uint8_t *)(src+i));
	}
	return dest;
}
char toupper(char ch){
	if(ch >= 'a' && ch <= 'z'){
		return (char)(ch - ('a' - 'A'));
	}else{
		return ch;
	}
}
char tolower(char ch){
	if(ch >= 'A' && ch <= 'Z'){
		return (char)(ch + ('a' - 'A'));
	}else{
		return ch;
	}
}
void upper(char *s){
	while(*s){
		*s=toupper(*s);
		s++;
	}
}
void lower(char *s){
	while(*s){
		*s=tolower(*s);
		s++;
	}
}
void itoa(int i,char *s){
	if(i == 0){
		*s = '0';
		*(s+1) = '\0';
		return;
	}
	char ch[12];
	int sign = 1;
	int n;
	if(i<0){
		sign=-1;
		i=-i;
	}
	for(n = 0;i!=0;i/=10,n++){
		ch[n]=i%10 + '0';
	}
	if(sign==-1){
          *s='-';
	}
	n--;
	for(;n>=0;n--,s++){
		*s=ch[n];
	}
	*s='\0';
}
void uitoa(unsigned int i,char *s){
        if(i == 0){
                *s = '0';
				*(s+1) = '\0';
                return;
        }
		uint32_t b = 10;
		while(i/b){
			b=b*10;
			s++;
		}
		*(s+1) = 0;
        do{
			*s = i%10+'0';
			s--;
		}while(i=i/10);
}
void itox(unsigned int i,char *s){
        if(i == 0){
                *s = '0';
                s++;
        }
		uint8_t have_zero = 0;
		for(int c = 0;c<(sizeof(unsigned int)*2);c++){
                uint8_t c;
                c = (i>>(sizeof(i)*8-4))&0xF;
				i=i<<4;
                if((c == 0) && (have_zero == 0)){
                        continue;
                }
                if(c > 9){
                        *s = 'a'+c-10;
                }else{
                        *s = c+'0';
                }
				have_zero = 1;
                s++;
        }
        *s = 0;
}
