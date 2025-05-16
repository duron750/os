#include "types.h"
#include "user.h"

int
main(void)
{
    int p[2];  // file descriptors for pipe
    char recv_buf[5];

    pipe(p);

    if (fork() == 0) {  // child
        read(p[0], recv_buf, 5);
        printf(1, "%d: receive %s\n", getpid(), recv_buf);
        close(p[0]);
	    
        write(p[1], "pong", 5);
        printf(1, "Executing pid %d\n", getpid());
        sleep(100);
        close(p[1]);
        exit();
		 
    } else {  // parent
        write(p[1], "ping", 5);
        wait(); 
        close(p[1]);

        read(p[0], recv_buf, 5);
        printf(1, "%d: received %s\n", getpid(), recv_buf);
        close(p[0]);
        exit();
    }
}
