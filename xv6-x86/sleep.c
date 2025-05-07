#include "types.h"
#include "user.h"

int
main(int argc, char *argv[])
{
	if(argc > 1 && argc < 3)
	{
		printf(1, "Sleeping for %d seconds\n", atoi(argv[1]));
		sleep(atoi(argv[1])*100);		
	} else
	{
		printf(1,"Usage: sleep <seconds>\n");
	}
	exit();
}
