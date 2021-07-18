#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/processinfo.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  struct processinfo proinf;
  if(get_process_info(&proinf)<0){
    printf("Error");
  }
  printf("Process ID −> %d\n", proinf.pid);
  printf("Process Name −> %s\n", proinf.name);
  printf("Memory Size −> %d Bytes\n", proinf.sz);
  exit(0);
}
