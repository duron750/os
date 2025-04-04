
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <utime.h>

int main(int argc, char *argv[])
{
	int fp;
	struct stat st;
	struct utimbuf ubuf;
	if(argc<2) printf("You need to specify file name\n");
	else
	{
		if(stat(argv[1], &st)==0)
		{
			printf("File exists\n");	
			time(&ubuf.actime);
			time(&ubuf.modtime);
			utime(argv[1],&ubuf);
		} else
		{
			fp=creat(argv[1],0644);
			printf("Result: %d\n",fp);
			close(fp);
		}
	}
}
