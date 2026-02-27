#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"


int
main(int argc, char *argv[])
{

  if(argc <= 2){
    fprintf(2, "usage mv [src_file] [dst_file]\n");
    exit(0);
  }

  if(link(argv[1], argv[2]) < 0) {
	fprintf(2, "cannot create destination file\n");
	exit(1);
  } 
  
  if(unlink(argv[1]) < 0 ) {
	fprintf(2, "cannot remove source file \n");
	unlink(argv[2]); //cleanup the destination link 
	exit(1);
  } 
 
  exit(0);
}
