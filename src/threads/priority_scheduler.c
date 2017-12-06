#include "threads/priority_scheduler.h"
#include "threads/malloc.h"


static struct list *ready_list;
static inline struct list_elem* get_max(void);
static inline void free_lock_entry(struct list*,struct lock*);


struct priority_elem{
	struct list_elem e;
	struct lock * lock;
	int64_t pr;
};

void
init_priority_scheduler(){
	ready_list = get_ready_list();
}

int 
get_last_priority(struct thread * t)
{
	if(list_empty(&t->priority_list))
		return t->priority;
	struct priority_elem * pe = list_entry(list_begin(&t->priority_list),
		                      struct priority_elem, e);
	return pe->pr;
}

bool 
less_than (const struct list_elem *a,
                             const struct list_elem *b,
                             void *aux UNUSED)
{
  int p_1 = get_last_priority(list_entry(a,struct thread,elem));
  int p_2 = get_last_priority(list_entry(b,struct thread,elem));
  if(p_1 < p_2)
    return true;
  return false;
}


static inline struct list_elem* 
get_max()
{
  return list_max(ready_list,less_than ,NULL);
}

bool 
check_preemption()
{
  if(list_empty(ready_list))
    return false;
  struct thread* cur = thread_current();
  struct thread* new_thread = list_entry(get_max(),
                              struct thread, elem);
  if(get_last_priority(new_thread) > get_last_priority(cur)){
    thread_yield();
    return true;
  }
  return false;
}


/* returns the highest priority thread */
struct thread *
priority_scheduler(){

  /* return max priority and remove it from the ready list*/
  struct list_elem *elem = get_max();
  list_remove(elem);
  return list_entry(elem, struct thread, elem);

}

void 
donate_priority(struct lock * lock)
{
  if(lock == NULL)
    return;
  struct thread * temp = thread_current();
  temp->obstacle_thread = &lock->holder;
  struct thread * obs = *temp->obstacle_thread;
  while(obs != NULL)
  {
    if(get_last_priority(temp) >= get_last_priority(obs))
    {
      // create new element of donation and add it to waiters stack 
      struct priority_elem *elem = malloc(sizeof(struct priority_elem));
      elem->lock = lock;
      elem->pr = get_last_priority(temp);
      free_lock_entry(&obs->priority_list,lock);
      list_push_front(&obs->priority_list,&elem->e); 

      /* update variables */
      temp = obs;
      if(temp->obstacle_thread == NULL){
         break;
      }
      obs = *temp->obstacle_thread;
        
    }	
    else
      return;
  }
}


void 
restore_priority(struct lock * lock)
{
  struct thread * t = thread_current();
  struct list * list = &t->priority_list;
  if(list_empty(list) || lock == NULL)
    return;
  if(t->number_of_locks == 0){
     while(!list_empty(list)){
     	struct list_elem *old_e = list_begin (list);
     	list_remove(old_e);
     }
  }else
    free_lock_entry(list,lock); 
}


static inline void 
free_lock_entry(struct list* list,struct lock* lock)
{
	if(list_empty(list))
		return;
	struct list_elem *e;  
    for (e = list_begin (list); e != list_end (list); e = list_next (e))
      {
      	struct priority_elem * pe = list_entry(e,
      		                      struct priority_elem, e);
      	if(pe->lock == lock)
      	{
          list_remove(e);
          free(pe);
          return;
      	}
      }
}