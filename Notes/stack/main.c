#include <stdio.h>

#include "stack.h"

int main(int atgc, char *argv[])
{
  stack_t s;
  int i;

  stack_init(&s);

  stack_push(&s, 1);
  stack_push(&s, 2);
  stack_push(&s, 3);
  stack_push(&s, 4);
  stack_push(&s, 5);

  while (stack_size(&s)) {
    stack_pop(&s, &i);
    printf("%d\n", i);
  }
