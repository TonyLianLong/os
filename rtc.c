#include <stdint.h>
#include <stdio.h>
#define TZ_OFFSET_HOUR +8
#define TZ_OFFSET_MIN 0
struct time_struct{
  uint8_t date_sec;			/* Seconds.	[0-60] (1 leap second) */
  uint8_t date_min;			/* Minutes.	[0-59] */
  uint8_t date_hour;			/* Hours.	[0-23] */
  uint8_t date_mday;			/* Day.		[1-31] */
  uint8_t date_mon;			/* monthnl.	[0-11] */
  uint16_t date_year;			/* Year	- 1900.  */
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
uint16_t lyd(uint16_t year){
	year += 1900;
	if(year%4 == 0){
		if(year%100 != 0){
			return 366;
		}else if(year%400 == 0){
			return 366;
		}
	}
	return 365;
}
time_t ctime(unsigned long timestamp){
	time_t time;
	uint16_t y = 0;
	timestamp += TZ_OFFSET_HOUR*HOUR;
	timestamp += TZ_OFFSET_MIN*MINUTE;
	time.date_sec = timestamp%MINUTE;
	time.date_min = (timestamp%HOUR)/MINUTE;
	time.date_hour = (timestamp%DAY)/HOUR;
	if(timestamp != 0){//If timestamp = 0,it will be still 0
		timestamp-=timestamp%DAY;
	}
	while(1){
		if(timestamp<(lyd(y+70)*DAY)){
			break;
		}
		timestamp-=lyd(y+70)*DAY;
		y++;
	}
	time.date_year = y+70;
	for(int i = 0;;i++){
		if(i+1 == 12){
			time.date_mon = 11;
			break;
		}
		if((i < 2)||(lyd(y+70) == 366)){
			if((timestamp >= month[i]) && (timestamp < month[i+1])){
				time.date_mon = i;
				break;
			}
		}else{
			if((timestamp >= (month[i]-DAY)) && (timestamp < (month[i+1]-DAY))){
				time.date_mon = i;
				break;
			}
		}
	}
	if((time.date_mon < 2)||(lyd(y+70) == 366)){
		time.date_mday = (timestamp-month[time.date_mon])/DAY;
	}else{
		time.date_mday = (timestamp-(month[time.date_mon]-DAY))/DAY;
	}
	return time;
}
int main(){
	time_t t = ctime(1409331329L);
	printf("%d %d %d %d %d %d\n",t.date_year+1900,t.date_mon+1,t.date_mday+1,t.date_hour,t.date_min,t.date_sec);
}
