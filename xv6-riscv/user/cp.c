#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"


int
main(int argc, char *argv[])
{
  int fd_src, fd_dst, n;
  char buf[512];

  if(argc <= 2){
    fprintf(2, "usage cp [src_file] [dst_file]\n");
    exit(0);
  }

  if((fd_src = open(argv[1], O_RDONLY)) < 0){
    fprintf(2, "cat: cannot open source file %s\n", argv[1]);
    exit(1);
  }
  
  if((fd_dst = open(argv[2], O_CREATE | O_WRONLY)) < 0){
    fprintf(2, "cat: cannot create destination file %s\n", argv[2]);
    exit(1);
  }
  while((n = read(fd_src, buf, sizeof(buf))) > 0) {
	if (write(fd_dst, buf, n) != n)
	{
		fprintf(2, "Error writing data\n");
		exit(1);
	}
  }
  close(fd_src);
  close(fd_dst);  
  exit(0);
}
