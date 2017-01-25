#ifndef STACK_H
#define STACK_H


typedef struct stack_node {
  int value;
  struct stack_node *next;
} stack_node_t;

typedef struct stack_node stack_node_t;

struct stack {
  int size;
  stack_node_t *top;
} stack_t;

int stack_init(stack_t *s);
int stack_delete(stack_t *s);
int stack_push(stack_t *s, int v);
int stack_pop(stack_t *s, int *v);
int stack_size(stack_t *s); 
int stack_is_Empty(stack_t *s);


#endif
