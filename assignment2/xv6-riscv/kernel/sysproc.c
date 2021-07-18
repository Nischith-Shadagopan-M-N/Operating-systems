#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "processinfo.h"

#define INT_MAX	+2147483647

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
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
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
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

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
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

uint64
sys_echo_simple(void)
{
  char input[1000];

  if(argstr(0, input, INT_MAX)<0)
    return -1;
  printf("%s\n", &input[0]);
  return 0;
}

uint64
sys_echo_kernel(void)
{
  int argc;
  if(argint(0, &argc)<0)
    return - 1;
  uint64 argv;
  if(argaddr(1, &argv)<0)
    return -1;
  for(int i = 1; i < argc; i++){
    char input[1000];
    uint64 inp;
    if(fetchaddr(argv + sizeof(char **)*i, &inp)<0)
      return -1;
    if(fetchstr(inp, input, INT_MAX)<0)
      return -1;
    printf("%s", &input[0]);
    if(i + 1 < argc){
      printf(" ");
    } else {
      printf("\n");
    }
  }
  return 0;
}

uint64
sys_get_process_info(void){
  uint64 pro;
  if(argaddr(0, &pro) < 0)
    return -1;
  struct proc *p = myproc();
  struct processinfo pinf;
  pinf.pid= p->pid;
  for(int i=0;i<16;i++){
    pinf.name[i] = p->name[i];
  }
  pinf.sz = p->sz;
  if(copyout(p->pagetable, pro, (char *)&pinf, sizeof(pinf)) < 0)
    return -1;
  return 0;
}

uint64
sys_trace(void){
  int input;
  if(argint(0, &input)<0)
    return - 1;
  struct proc *p = myproc();
  p->trace = input;
  return 0;
}
