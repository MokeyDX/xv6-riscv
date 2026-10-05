#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

// Report a failed check and exit with a nonzero status.
static void
fail(char *message)
{
  printf("prioritytest: FAIL: %s\n", message);
  exit(1);
}

int
main(void)
{
  int initial = getpriority();
  int child;
  int status;

  printf("initial priority: %d\n", initial);
  if (initial < MINPRIORITY || initial > MAXPRIORITY)
    fail("initial priority is outside the allowed range");

  // Invalid requests must fail without changing the priority.
  if (setpriority(MINPRIORITY - 1) != -1 ||
      setpriority(MAXPRIORITY + 1) != -1 ||
      getpriority() != initial)
    fail("invalid priority handling");

  if (setpriority(MAXPRIORITY) != 0 ||
      getpriority() != MAXPRIORITY)
    fail("setting the maximum priority");

  child = fork();
  if (child < 0)
    fail("fork");

  if (child == 0) {
    printf("child inherited priority: %d\n", getpriority());
    if (getpriority() != MAXPRIORITY)
      fail("child inheritance");
    if (setpriority(7) != 0 || getpriority() != 7)
      fail("child priority change");
    printf("child changed priority: %d\n", getpriority());
    exit(0);
  }

  if (wait(&status) != child || status != 0)
    fail("child result");
  if (getpriority() != MAXPRIORITY)
    fail("child changed parent priority");

  printf("parent priority after child exits: %d\n", getpriority());
  printf("prioritytest: PASS\n");
  exit(0);
}
