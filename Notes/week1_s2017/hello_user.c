#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  int i;

  for (i = 1; i < argc; i++) {
    printf("Hello %d!\n", 5 * atoi(argv[i]));
  }

  return 0;
}
