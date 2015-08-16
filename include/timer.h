#ifndef timer_h
#define timer_h
#include <interrupt.h>
#include <stdbool.h>
#define TIMER_LIST_NUMBER 256
#define TIMER_LIST_ADDRESS 0x301800
#define CLOCK_FREQUENCY 100
struct timer_struct {
	bool exist;
	unsigned int time;
	unsigned int default_time;
	void(*entry)();
	bool loop;
};
typedef struct timer_struct timer_t;
void init_timer();
uint16_t get_pit_count();
void delay10ms(uint32_t time);
void set_pit_count(uint16_t count);
void set_timer(uint8_t num,unsigned int time,void(*entry)(),bool loop);
int add_timer(unsigned int time,void(*entry)(),bool loop);
bool istimer(uint8_t num);
void clear_timer(uint8_t num);
#endif