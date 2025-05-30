#include "types.h"
#include "user.h"

int primes(int* a)
{
	printf(1, "Calculating primes up to %d\n", *a);
	int p[2];
	int i=0;
	int j=0;
	int cont=0;
	pipe(p);

	if(fork() == 0) {
		while(read(p[0], &j, sizeof(j)) != 0)
		{
//			printf(1, "Received: %d, pid %d\n", j, getpid());
			for(int k=2; k<=j/2; k++) {
				if( j%k == 0) {	
					cont++;
					break;
				}
			}
			if (cont == 0) printf(1, "Prime: %d\n", j);
			cont = 0;			
		}
		close(p[0]);
		exit();
	} else {
		for (i=1;i<*a;i++) {
//			printf(1, "Sent: %d, pid %d\n", i, getpid());			
			write(p[1], &i, sizeof(i));
		}	
		close(p[1]);
		printf(1, "main process exiting\n");
	}
}

int
main(int argc, char *argv[])
{
	if(argc > 1 && argc < 3)
	{		
		int a=atoi(argv[1]);
		primes(&a);
	} else {
		printf(1,"Usage: primes <max_number>\n");
	}

	exit();
}
