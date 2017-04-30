#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/* First overflow at 159487 */
int main(int argc, char *argv[])
{
  unsigned val;
  int count;

  val = atoi(argv[1]);

  count = 0;
  while (val != 1) {
    if (val & 1) {
      if (val >= UINT_MAX / 3) {
        fprintf(stderr, "OVERFLOW!\n");
        return -1;
      }
      val = val * 3 + 1;
    } else {
      val /= 2;
    }
    count++;
  }

  printf("%d\n", count);

  return 0;
}
