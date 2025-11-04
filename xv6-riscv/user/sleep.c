#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
	if(argc > 1 && argc < 3)
	{
		fprintf(2, "Sleeping for %d seconds\n", atoi(argv[1]));
		pause(atoi(argv[1])*10);		
	} else
	{
		fprintf(1,"Usage: sleep <seconds>\n");
	}
	exit(1);
}
