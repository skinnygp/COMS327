#include <stdio.h>
#include <unistd.h>

void times_table(void);

int main(int argc, char *argv[])
{
  times_table();

  return 0;
}

void times_table(void)
{
  int table[10][10];
  int i, j, k;

  for (j = 0; j < 10; j++) {
    for (i = 0; i < 10; i++) { 
      table[j][i] = (i + 1) * (j + 1);
    }
  }

  k = 0;
  while (1) {
    printf("%d\n", k++);
    for (j = 0; j < 10; j++) {
      for (i = 0; i < 10; i++) { 
        printf("%3d ", table[j][i]);
      }
      printf("\n");
    }
    usleep(83333);
  }
}

