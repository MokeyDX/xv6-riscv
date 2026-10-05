#include "kernel/types.h"
#include "user/user.h"

#define MAXARGS 16 // Maximum command arguments, including the null terminator.

int
main(int argc, char *argv[])
{
  char *command_argv[MAXARGS];
  int priority;
  int child;
  int status;

  if (argc < 3 || argc > MAXARGS + 1) {
    printf("Usage: pexec <priority> <command> [args...]\n");
    exit(1);
  }

  // Set this process's priority before forking so the child inherits it.
  priority = atoi(argv[1]);
  if (setpriority(priority) < 0) {
    printf("pexec: priority must be between 0 and 49\n");
    exit(1);
  }

  for (int i = 2; i < argc; i++)
    command_argv[i - 2] = argv[i];
  command_argv[argc - 2] = 0;

  child = fork();
  if (child < 0) {
    printf("pexec: fork failed\n");
    exit(1);
  }
  if (child == 0) {
    exec(command_argv[0], command_argv);
    printf("pexec: exec failed\n");
    exit(1);
  }

  // Return the command's exit status after it finishes.
  if (wait(&status) != child)
    exit(1);
  exit(status);
}
