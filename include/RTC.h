#ifndef RTC_h
#define RTC_h
#include <stdint.h>
#include <sys.h>
#define TZ_OFFSET_HOUR +8
#define TZ_OFFSET_MIN 0
#define CMOS_WRITE(addr,data) ({ \
write_port(0x70,addr); \
write_port(0x71,data); \
})
#define BCD_TO_BIN(val) ((val)=((val)&0xF) + (((val)>>4)&0xFF)*10)
struct time_struct{
  uint8_t date_sec;			/* Seconds.	[0-60] (1 leap second) */
  uint8_t date_min;			/* Minutes.	[0-59] */
  uint8_t date_hour;			/* Hours.	[0-23] */
  uint8_t date_mday;			/* Day.		[1-31] */
  uint8_t date_mon;			/* Month.	[0-11] */
  uint16_t date_year;			/* Year	- 1900.  */
};
struct date_struct{
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	uint8_t mon;
	uint16_t year;
};
#define MINUTE 60
#define HOUR (60*MINUTE)
#define DAY (24*HOUR)
#define YEAR (365*DAY)
static int month[12] = {
	0,
	DAY*(31),
	DAY*(31+29),
	DAY*(31+29+31),
	DAY*(31+29+31+30),
	DAY*(31+29+31+30+31),
	DAY*(31+29+31+30+31+30),
	DAY*(31+29+31+30+31+30+31),
	DAY*(31+29+31+30+31+30+31+31),
	DAY*(31+29+31+30+31+30+31+31+30),
	DAY*(31+29+31+30+31+30+31+31+30+31),
	DAY*(31+29+31+30+31+30+31+31+30+31+30)
};
typedef struct time_struct time_t;
typedef struct date_struct date_t;
unsigned long sys_timestamp;
void init_RTC();
void add_time();
time_t read_RTC();
unsigned long kernel_mktime(time_t *time);
void print_time();
date_t get_date(time_t time);
#endif