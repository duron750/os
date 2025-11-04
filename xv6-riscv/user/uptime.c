#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

void print_padded_number(int num) {  
  // In minimalist printf implementations (like in XV6) that do not support 
  // the %02d format specifier, manual conditional logic must be used.
  
  // Check if the number is a single digit (0-9)
  if (num >= 0 && num < 10) {
    // If single digit, manually print the leading zero character.
    printf("0");
  }
  
  // Print the number itself using the basic %d specifier.
  printf("%d", num);
}

int
main()
{
	int ticks, sec, min=0, hour=0, days=0;
	ticks = uptime()/10;
	sec = ticks;
	min = ticks/60;
	hour = min/60;
	days = hour/24;	
	if(sec>60) sec=sec%60;
	if(min>60) min=min%60;
	if(hour>24) hour=hour%24;

	printf("Up %d days ", days);
	print_padded_number(hour);
	printf(":");
	print_padded_number(min);
	printf(":");
	print_padded_number(sec);
	printf("\n");
	exit(0);
}
