#include "threads/thread.h"
#include "threads/synch.h"
#include "tests/threads/tests.h"

void init_priority_scheduler(void);

int get_last_priority(struct thread * t);

/* Compares the value of two list elements A and B, given
   auxiliary data AUX.  Returns true if A is less than B, or
   false if A is greater than or equal to B. */

/* returns max priority ELEM */
bool less_than (const struct list_elem *a,
                             const struct list_elem *b,
                             void *aux UNUSED);

/* this function causes preemption if the high-priority 
   thread containing ELEM should preempt the current
   thread ,then it returns true . it returns false otherwise */

bool check_preemption(void);

/* returns the highest priority thread */
struct thread *priority_scheduler(void);

/* priority inheritance donation function
   for dynamic scheduling
*/
void donate_priority(struct lock *);

void restore_priority(struct lock *);