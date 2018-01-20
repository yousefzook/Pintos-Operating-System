#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define EXECUTING -2
struct child_process{
	struct list_elem elem;
	int tid;
	int status;
	bool wait;
};

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);
struct child_process * get_child(struct list * children,tid_t tid);

#endif /* userprog/process.h */
