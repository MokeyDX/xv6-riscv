#ifndef XV6_PSTAT_H
#define XV6_PSTAT_H

// Process states shared by the kernel and user programs.
enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// A snapshot of one process returned by getprocs.
struct pstat {
  int pid;               // Process ID.
  enum procstate state;  // Process state.
  uint64 size;           // Process memory size in bytes.
  int ppid;              // Parent process ID.
  char name[16];         // Process command name.
};

// CPU usage returned by the PA1 wait2 system call.
struct rusage {
  uint cputime;
};

#endif
