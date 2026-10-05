#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

// Report a test failure and stop the current process.
static void
fail(char *message)
{
  printf("schedtest: FAIL: %s\n", message);
  exit(1);
}

// Wait for the parent's release, then record the first scheduling order.
static void
worker(int priority, char label, int readyfd, int gofd, int orderfd)
{
  char token = 'R';

  if (setpriority(priority) != 0)
    exit(1);
  if (write(readyfd, &token, 1) != 1)
    exit(1);
  if (read(gofd, &token, 1) != 1)
    exit(1);
  if (write(orderfd, &label, 1) != 1)
    exit(1);
  exit(0);
}

int
main(void)
{
  int ready[2], go[2], order[2];
  int low, high, status;
  char token = 'G';
  char first, second;
  char expected_first = SCHED_POLICY == SCHED_RR ? 'L' : 'H';
  char expected_second = SCHED_POLICY == SCHED_RR ? 'H' : 'L';

  if (pipe(ready) < 0 || pipe(go) < 0 || pipe(order) < 0)
    fail("pipe setup");

  // Create the low-priority child first to distinguish the policies.
  low = fork();
  if (low < 0)
    fail("low-priority fork");
  if (low == 0)
    worker(5, 'L', ready[1], go[0], order[1]);

  high = fork();
  if (high < 0)
    fail("high-priority fork");
  if (high == 0)
    worker(30, 'H', ready[1], go[0], order[1]);

  // Wait until both children are blocked before releasing them.
  if (read(ready[0], &token, 1) != 1 ||
      read(ready[0], &token, 1) != 1)
    fail("workers did not become ready");
  token = 'G';
  if (write(go[1], &token, 1) != 1 ||
      write(go[1], &token, 1) != 1)
    fail("worker release");

  if (read(order[0], &first, 1) != 1 ||
      read(order[0], &second, 1) != 1)
    fail("result collection");
  if (wait(&status) < 0 || status != 0)
    fail("first child");
  if (wait(&status) < 0 || status != 0)
    fail("second child");

  printf("run order: %c then %c\n", first, second);
  if (first != expected_first || second != expected_second)
    fail("unexpected scheduling order");
  printf("schedtest: PASS\n");
  exit(0);
}
