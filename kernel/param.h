#define NPROC       64                // maximum number of processes
#define MINPRIORITY 0 // Minimum base priority.
#define MAXPRIORITY 49 // Maximum base priority.
#define MAXEFFPRIORITY 99 // Cap effective priority after aging.
#define SCHED_RR 0 // Run runnable processes in table order.
#define SCHED_PRIORITY 1 // Select the highest base priority.
#define SCHED_AGING 2 // Select aged effective priority.
#define SCHED_POLICY SCHED_AGING // Compile-time scheduler selection.
#define NCPU        8                 // maximum number of CPUs
#define NOFILE      16                // open files per process
#define NFILE       100               // open files per system
#define NINODE      50                // maximum number of active i-nodes
#define NDEV        10                // maximum major device number
#define ROOTDEV     1                 // device number of file system root disk
#define MAXARG      32                // max exec arguments
#define MAXOPBLOCKS 10                // max # of blocks any FS op writes
#define LOGBLOCKS   (MAXOPBLOCKS * 3) // max data blocks in on-disk log
#define NBUF        (MAXOPBLOCKS * 3) // size of disk block cache
#define FSSIZE      2000              // size of file system in blocks
#define MAXPATH     128               // maximum file path name
#define USERSTACK   1                 // user stack pages
