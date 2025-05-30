#include "types.h"
#include "user.h"

int
main()
{
	int ticks, sec, min=0, hour=0, days=0;
	ticks = uptime()/100;
	sec = ticks;
	min = ticks/60;
	hour = min/60;
	days = hour/24;	
	if(sec>60) sec=sec%60;
	if(min>60) min=min%60;
	if(hour>24) hour=hour%24;

	printf(1,"Up %d days %d:%d:%d\n", days, hour, min, sec);
	exit();
}
