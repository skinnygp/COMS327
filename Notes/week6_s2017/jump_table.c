#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef void (*func_table_element)(void); /* signature for a pointer to *
                                           * function taking void and   *
                                           * returning void.            */

int i; /* Do not name a global i */

void zero()
{
  printf("zero\n");
}

void one()
{
  printf("one\n");
}

void two()
{
  printf("two\n");
}

func_table_element func_table[] = {
  zero,
  one,
  two,
};

int counter()
{
  static int i = 0;
  return ++i;
}

int main(int argc, char *argv[])
{
  srand(time(NULL));

  i = rand() % 3;

  func_table[i]();

  return 0;
}

