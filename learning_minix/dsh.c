#include <stdio.h>
#include <sys/types.h>

void main()
{
	char command[255];
	int count,status;
	while (1)
	{
		printf("dsh>");
		count=read(0, command, 255);
		command[count-1]=0;
		if (fork()!=0)
		{
			wait(&status);
		} else {
			execve(command, 0, 0);
			break;
		}
	}
	exit(0);
}
