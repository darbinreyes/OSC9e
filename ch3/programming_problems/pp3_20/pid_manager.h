/**

**/

#ifndef __PID_MANAGER_H__
#define __PID_MANAGER_H__

#define MIN_PID 300
#define MAX_PID 500 // der-TODO: Change to 5000, but smaller is easier to test with.

// ret. 0 if successful, -1 on error.
int allocate_map(void);

int allocate_pid(void);

void release_pid(int pid);

#endif