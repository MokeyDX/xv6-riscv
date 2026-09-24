#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char **argv) {
  int pid;
  int status;
  int start_time;
  int end_time;

  if(argc < 2){
    fprintf(2, "usage: time1 command [args ...]\n");
    exit(1);
  }

  start_time = uptime();
  pid = fork();

  if(pid < 0){
    fprintf(2, "time1: fork failed\n");
    exit(1);
  }

  if(pid == 0){
    exec(argv[1], &argv[1]);
    fprintf(2, "time1: exec %s failed\n", argv[1]);
    exit(1);
  }

  if(wait(&status) < 0){
    fprintf(2, "time1: wait failed\n");
    exit(1);
  }

  end_time = uptime();
  printf("elapsed time: %d ticks\n", end_time - start_time);

  exit(status);
}
