#include "kernel/types.h"
#include "user/user.h"

int main() {
  struct uproc procs[64]; // NPROC is 64 in xv6
  int n = ps(procs);

  if(n < 0){
    printf("ps: error getting process table\n");
    exit(1);
  }

  printf("PID\tPPID\tSIZE\tSTATE\tNAME\n");
  for(int i = 0; i < n; i++){
	printf("%d\t%d\t%d\t%s\t%s\n", 
       procs[i].pid, procs[i].ppid, (int)procs[i].sz, procs[i].state, procs[i].name);
  }
  exit(0);
}
