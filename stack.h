#include <stdbool.h>
#include <stdint.h>

struct stack_elem
{
    struct stack_elem *prev;
    struct stack_elem *next;
    int value;
};

struct stack
{
    struct stack_elem head;
    struct stack_elem tail;
};


void stack_init (struct stack *);

struct stack_elem *stack_begin (struct stack *);
struct stack_elem *stack_next (struct stack_elem *);
struct stack_elem *stack_end (struct stack *);


void push (struct stack *, struct stack_elem *);
struct stack_elem *stack_remove (struct stack_elem *);
struct stack_elem *pop (struct stack *);
struct stack_elem *top (struct stack *);

bool isEmpty (struct stack *);

void clear(struct stack *);
