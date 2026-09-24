#include "kernel/types.h"
#include "kernel/pstat.h"
#include "user/user.h"

int main(int argc, char **argv) {
  int pid;
  int status;
  int start_time;
  int end_time;
  int elapsed_time;
  int cpu_percent;
  struct rusage usage;

  if(argc < 2){
    fprintf(2, "usage: time command [args ...]\n");
    exit(1);
  }

  start_time = uptime();
  pid = fork();

  if(pid < 0){
    fprintf(2, "time: fork failed\n");
    exit(1);
  }

  if(pid == 0){
    exec(argv[1], &argv[1]);
    fprintf(2, "time: exec %s failed\n", argv[1]);
    exit(1);
  }

  if(wait2(&status, &usage) < 0){
    fprintf(2, "time: wait2 failed\n");
    exit(1);
  }

  end_time = uptime();
  elapsed_time = end_time - start_time;

  if(elapsed_time > 0)
    cpu_percent = usage.cputime * 100 / elapsed_time;
  else
    cpu_percent = 0;

  printf("elapsed time: %d ticks, cpu time: %d ticks, %d%% CPU\n",
         elapsed_time, usage.cputime, cpu_percent);

  exit(status);
}
