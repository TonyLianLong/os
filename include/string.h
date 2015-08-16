#ifndef string_h
#define string_h
unsigned int strlen(char *s);
int memcmp(void *buf1,void *buf2, unsigned int count);
int memicmp(void *buf1,void *buf2, unsigned int count);
void *memcpy (void *dst,const void *src,unsigned int len);
char toupper(char ch);
char tolower(char ch);
void upper(char *s);
void lower(char *s);
void itoa(int i,char *s);
void uitoa(unsigned int i,char *s);
void itox(unsigned int i,char *s);
#endif
