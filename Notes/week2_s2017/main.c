#include <stdio.h>

#include "fc.h"

int main(int argc, char *argv[])
{
  printf("100C is %dF\n", c2f(100));
  printf("32F is %dC\n", f2c(32));

  return 0;
}
