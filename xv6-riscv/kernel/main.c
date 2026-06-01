#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

volatile static int started = 0;

// start() jumps here in supervisor mode on all CPUs.
void
main()
{
  if(cpuid() == 0){
    consoleinit();
    printfinit();
    printf("\n");
    printf("xv6 kernel is booting\n");
    printf("\n");
    printf("hart 0 starting\n");
    kinit();         // physical page allocator
    printf("initializing virtual memory\n");
    kvminit();       // create kernel page table
    kvminithart();   // turn on paging
    printf("initializing process table\n");
    procinit();      // process table
    printf("initializing trap vectors\n");
    trapinit();      // trap vectors
    trapinithart();  // install kernel trap vector
    printf("setting up interrupt controller\n");
    plicinit();      // set up interrupt controller
    plicinithart();  // ask PLIC for device interrupts
    binit();         // buffer cache
    printf("configuring inode and file table\n");
    iinit();         // inode table
    fileinit();      // file table
    printf("initializing hard drive\n");
    virtio_disk_init(); // emulated hard disk
    printf("starting init...\n");
    userinit();      // first user process
    __sync_synchronize();
    started = 1;
  } else {
    while(started == 0)
      ;
    __sync_synchronize();
    printf("hart %d starting\n", cpuid());
    kvminithart();    // turn on paging
    trapinithart();   // install kernel trap vector
    plicinithart();   // ask PLIC for device interrupts
  }

  scheduler();        
}
