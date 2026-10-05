#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

struct timing {
  char label;
  int priority;
  uint started;
  uint finished;
};

// Stop the test when setup or result collection fails.
static void
fail(char *message)
{
  printf("agingtest: FAIL: %s\n", message);
  exit(1);
}

// Wait for release, perform the assigned work, and report its timing.
static void
worker(int priority, char label, int readyfd, int gofd, int resultfd)
{
  char token = 'R';
  struct timing result;

  if (priority >= 0 && setpriority(priority) != 0)
    fail("worker priority");
  if (write(readyfd, &token, 1) != 1 ||
      read(gofd, &token, 1) != 1)
    fail("worker setup");

  result.label = label;
  result.priority = getpriority();
  result.started = uptime();
  if (label == 'H') {
    // Keep the high-priority worker runnable for 120 clock ticks.
    while ((uint)(uptime() - result.started) < 120)
      ;
  }
  result.finished = uptime();

  if (write(resultfd, &result, sizeof(result)) != sizeof(result))
    fail("worker result");
  exit(0);
}

int
main(int argc, char *argv[])
{
  int ready[2], go[2], results[2];
  int lowpid, highpid, status;
  char token;
  char release[2] = {'G', 'G'};
  struct timing first, second, low, high;
  uint released, lowresponse, highresponse;
  uint lowturnaround, highturnaround;
  int lowpriority = 0;

  // Preserve the shell's inherited priority when requested.
  if (argc == 2 && strcmp(argv[1], "inherit") == 0)
    lowpriority = -1;
  else if (argc != 1)
    fail("usage: agingtest [inherit]");

  if (pipe(ready) < 0 || pipe(go) < 0 || pipe(results) < 0)
    fail("pipe setup");

  // Create the low-priority worker first for a consistent process order.
  lowpid = fork();
  if (lowpid < 0)
    fail("low-priority fork");
  if (lowpid == 0)
    worker(lowpriority, 'L', ready[1], go[0], results[1]);

  highpid = fork();
  if (highpid < 0)
    fail("high-priority fork");
  if (highpid == 0)
    worker(49, 'H', ready[1], go[0], results[1]);

  if (read(ready[0], &token, 1) != 1 ||
      read(ready[0], &token, 1) != 1)
    fail("worker readiness");

  // One write makes both release tokens available before either worker runs.
  released = uptime();
  if (write(go[1], release, sizeof(release)) != sizeof(release))
    fail("worker release");
  if (read(results[0], &first, sizeof(first)) != sizeof(first) ||
      read(results[0], &second, sizeof(second)) != sizeof(second))
    fail("result collection");
  if (wait(&status) < 0 || status != 0 ||
      wait(&status) < 0 || status != 0)
    fail("worker exit");

  if (first.label == 'L' && second.label == 'H') {
    low = first;
    high = second;
  } else if (first.label == 'H' && second.label == 'L') {
    high = first;
    low = second;
  } else {
    fail("worker labels");
  }

  lowresponse = low.started - released;
  highresponse = high.started - released;
  lowturnaround = low.finished - released;
  highturnaround = high.finished - released;

  printf("policy: %s\n",
         SCHED_POLICY == SCHED_AGING ? "aging" : "priority");
  printf("low base priority: %d\n", low.priority);
  printf("low: response %u, turnaround %u ticks\n",
         lowresponse, lowturnaround);
  printf("high: response %u, turnaround %u ticks\n",
         highresponse, highturnaround);
  printf("average response: %u.%u ticks\n",
         (lowresponse + highresponse) / 2,
         ((lowresponse + highresponse) % 2) * 5);
  printf("average turnaround: %u.%u ticks\n",
         (lowturnaround + highturnaround) / 2,
         ((lowturnaround + highturnaround) % 2) * 5);

  // The short worker must run before the long worker finishes with aging.
  if (SCHED_POLICY == SCHED_AGING && lowresponse >= 100)
    fail("low-priority worker waited too long");
  if (SCHED_POLICY == SCHED_PRIORITY && lowresponse < 100)
    fail("plain priority ordering");
  printf("agingtest: PASS\n");
  exit(0);
}
