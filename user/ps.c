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

  printf("pid\tstate\t\tsize\tpriority\tppid\tname\n");
  for (int i = 0; i < count; i++) {
    printf("%d\t%s\t%lu\t%d\t%d\t%s\n",
           processes[i].pid, state_name(processes[i].state),
           processes[i].size, processes[i].priority, processes[i].ppid, processes[i].name);
  }

  exit(0);
}
