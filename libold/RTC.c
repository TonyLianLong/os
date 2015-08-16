#include <RTC.h>
#include <timer.h>
uint8_t CMOS_read(uint16_t address){
	write_port(0x70,(1<<7)|address);
	return read_port(0x71);
}
void init_RTC(){
	time_t time;
	time = read_RTC();
	sys_timestamp = kernel_mktime(&time);
	add_timer(100,add_time,1);
}
time_t read_RTC(){
	time_t time;
	do {
		time.date_sec = CMOS_read(0);
		time.date_min = CMOS_read(2);
		time.date_hour = CMOS_read(4);
		time.date_mday = CMOS_read(7);
		time.date_mon = CMOS_read(8);
		time.date_year = CMOS_read(9);
	} while (time.date_sec != CMOS_read(0));
	BCD_TO_BIN(time.date_sec);
	BCD_TO_BIN(time.date_min);
	BCD_TO_BIN(time.date_hour);
	BCD_TO_BIN(time.date_mday);
	BCD_TO_BIN(time.date_mon);
	BCD_TO_BIN(time.date_year);
	time.date_mon--;
	time.date_year+=100;
	return time;
}
void add_time(){
	sys_timestamp++;
}
unsigned long kernel_mktime(time_t *time){
	unsigned long res;
	int year;

	year = time->date_year - 70;
/* magic offsets (y+1) needed to get leapyears right.*/
	res = YEAR*year + DAY*((year+1)/4);
	res += month[time->date_mon];
/* and (y+2) here. If it wasn't a leap-year, we have to adjust */
	if (time->date_mon>1 && ((year+2)%4))
		res -= DAY;
	res += DAY*(time->date_mday-1);
	res += HOUR*time->date_hour;
	res += MINUTE*time->date_min;
	res += time->date_sec;
	res -= TZ_OFFSET_HOUR*HOUR;
	res -= TZ_OFFSET_MIN*MINUTE;
	return res;
}
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
date_t get_date(time_t time){
	date_t date;
	date.sec = time.date_sec;
	date.min = time.date_min;
	date.hour = time.date_hour;
	date.day = time.date_mday+1;
	date.mon = time.date_mon+1;
	date.year = time.date_year+1900;
	return date;
}
void print_time(){
	time_t now_time = ctime(sys_timestamp);
	date_t date = get_date(now_time);
	print_string("Date:");
	print_int(sys_timestamp);
	print_char(' ');
	print_int(date.year);
	print_char(' ');
	print_int(date.mon);
	print_char(' ');
	print_int(date.day);
	print_char(' ');
	print_int(date.hour);
	print_char(':');
	print_int(date.min);
	print_char(':');
	print_int(date.sec);
	print_char('\n');
}