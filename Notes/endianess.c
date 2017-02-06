#include <stdio.h>
#include <endian.h>
#include <math.h>

int main(int argc, char *argv[])
{
  union {
    int i;
    float f;
    char c[4];
  } u;

  u.i = 0x01020304;



  u.i = htobe32(u.i);




  return 0;
}
