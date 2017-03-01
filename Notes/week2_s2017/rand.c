#include <stdio.h>
#include <stdlib.h> /* rand() and srand() */
#include <time.h> /* time() */

int main(int argc, char *argv[])
{
  int seed;

  if (argc == 2) {
    seed = atoi(argv[1]);
  } else {
    seed = time(NULL);
    printf("Seed is %d\n", seed);
  }

  srand(seed);

  printf("%d\n", rand());
  printf("%d\n", rand());
  printf("%d\n", (signed) rand() % 201 - 100);
  printf("%d\n", rand());
  printf("%d\n", rand());
  printf("%d\n", rand());
  printf("%d\n", rand());
  printf("%d\n", rand());
  printf("%d\n", rand());
  printf("%d\n", rand());

  return 0;
}
