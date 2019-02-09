# Constant time Scheduler - O(1)

Constant time scheduler schedules processes within a constant amount of time, regardless of number of processes running on the Operating System. It's an improvement over classical O(n) schedulers.

Processes are generated at random and stored in a structure. Timeslices are alloted to processes based on the dynamic priority of process 
which is real time. Based on dynamic priority each process is placed in active or expired linked list queues indexed by priority array(100 
to 139 where 100 implies highest priority). Indexing is done using a bitmap which is key to implementing O(1) scheduling. As each process 
completes its execution, the log is stored into process log file. 
