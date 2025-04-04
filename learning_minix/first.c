#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	printf("Argumente: %d\n",argc);
	printf("Pid of this program: %d\n",getpid());
	printf("Time in seconds since 01.01.1970: %d\n",time());
	printf("File info: %s\n",stat("test"));
}
