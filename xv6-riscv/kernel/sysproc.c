#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if(t == SBRK_EAGER || n < 0) {
    if(growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if(addr + n < addr)
      return -1;
    if(addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

extern struct proc proc[NPROC]; // Access the global process table

uint64
sys_ps(void)
{
  uint64 addr;
  struct proc *p;
  struct uproc u;
  int i = 0;

  // Get the user-space address of the struct array
  argaddr(0, &addr);

  for(p = proc; p < &proc[NPROC]; p++){
    acquire(&p->lock);
    if(p->state != UNUSED){
      // Fill the "public" structure
      u.pid = p->pid;
      u.ppid = p->parent ? p->parent->pid : 0;
      u.sz = p->sz;
      safestrcpy(u.name, p->name, sizeof(u.name));
      
      static char *states[] = {"unused", "used", "sleep", "runble", "run", "zombie"};
      safestrcpy(u.state, states[p->state], sizeof(u.state));

      // We release the lock before copyout to avoid holding it during memory I/O
      release(&p->lock);

      // Copy one struct at a time into the user's array at the correct offset
      if(copyout(myproc()->pagetable, addr + i*sizeof(struct uproc), (char *)&u, sizeof(u)) < 0)
        return -1;
      
      i++;
    } else {
      release(&p->lock);
    }
  }
  return i; // Return total processes found
}
