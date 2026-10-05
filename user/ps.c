#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/pstat.h"
#include "user/user.h"

// Convert a process state to a printable name.
static char *
state_name(enum procstate state)
{
  switch (state) {
  case SLEEPING: return "sleeping";
  case RUNNABLE: return "runnable";
  case RUNNING:  return "running";
  case ZOMBIE:   return "zombie";
  default:       return "unknown";
  }
}

int
main(void)
{
  struct pstat processes[NPROC];
  int count;

  // Request a snapshot of active processes from the kernel.
  count = getprocs(processes);
  if (count < 0) {
    printf("ps: getprocs failed\n");
    exit(1);
  }

  // Take one clock sample so all listed ages use the same reference.
  uint now = uptime();

  printf("pid\tstate\t\tsize\tage\tpriority\tppid\tname\n");
  for (int i = 0; i < count; i++) {
    printf("%d\t%s\t%lu\t",
           processes[i].pid, state_name(processes[i].state),
           processes[i].size);
    if (processes[i].state == RUNNABLE)
      printf("%u", now - processes[i].readytime);
    else
      printf("-");
    printf("\t%d\t%d\t%s\n",
           processes[i].priority, processes[i].ppid, processes[i].name);
  }

  exit(0);
}
