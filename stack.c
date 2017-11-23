#include "stack.h"
#include "../debug.h"

void  stack_init (struct stack *stack)
{
    ASSERT (stack != NULL);
    stack->head.prev = NULL;
    stack->head.next = &stack->tail;
    stack->tail.prev = &stack->head;
    stack->tail.next = NULL;
}

struct stack_elem *stack_begin (struct stack *stack)
{
    ASSERT (stack != NULL);
    return stack->head.next;
}


struct stack_elem *stack_end (struct stack *stack)
{
    ASSERT (stack != NULL);
    return &stack->tail;
}

void stack_insert (struct stack_elem *before, struct stack_elem *elem)
{
    ASSERT (is_interior (before) || is_tail (before));
    ASSERT (elem != NULL);

    elem->prev = before->prev;
    elem->next = before;
    before->prev->next = elem;
    before->prev = elem;
}

void push(struct stack *stack, struct stack_elem *elem)
{
    stack_insert (stack_begin (stack), elem);
}

struct stack_elem *stack_remove (struct stack_elem *elem)
{
    ASSERT (is_interior (elem));
    elem->prev->next = elem->next;
    elem->next->prev = elem->prev;
    return elem->next;
}

static inline bool is_tail (struct stack_elem *elem)
{
    return elem != NULL && elem->prev != NULL && elem->next == NULL;
}

struct stack_elem *pop (struct stack *stack)
{
    struct stack_elem *top =  top(stack);
    stack_remove (top);
    return top;
}

struct stack_elem *top (struct stack *stack)
{
    ASSERT (!stack_empty (stack));
    return stack->head.next;
}

bool isEmpty (struct stack *stack)
{
    return stack_begin (stack) == stack_end (stack);
}

static inline bool is_interior (struct stack_elem *elem)
{
    return elem != NULL && elem->prev != NULL && elem->next != NULL;
}

void clear(struct stack *)
{
    stack_init(stack);
}