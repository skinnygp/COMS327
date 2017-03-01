#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int *f1(int j)
{
  static int i;

  i = j;

  printf("In f1: %d\n", i);

  return &i;
}

void f2(void)
{
  int i;

  printf("In f2: %d\n", i);
}

int main(int argc, char *argv[])
{
  int *i;

  srand(time(NULL));

  f2();

  i = f1(rand());
  printf("In main: %d\n", *i);

  f2();

  return 0;
}
