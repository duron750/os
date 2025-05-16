#include "types.h"
#include "user.h"

int
main(void)
{
	int p[2];
	char buf[12];
	pipe(p);
	int pid = fork();

	if(pid == 0) {
		printf(2, "child %d executing\n", getpid());
		read(p[0], buf, 12);
		close(p[0]);
		printf(2, "Received: %s\n", buf);
		printf(2, "child %d exiting\n", getpid());
		exit();
	} else {
		printf(2, "parent %d executing %d\n", getpid(), pid);
		write(p[1], "hello world", 12);
		printf(2, "parent %d sending message 'hello world'\n", getpid());
		wait();
		close(p[1]);
		printf(1, "parent %d exiting\n", getpid());
		exit();
	}
}
