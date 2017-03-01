#include <stdio.h>
#include <stdarg.h>

void foo(char *format, ...)
{
  va_list ap;
  int d;
  char *s;
  char c;
  float f;

  va_start(ap, format);
  while (*format) {
    switch (*format++) {
    case 's':
      s = va_arg(ap, char *);
      printf("string %s\n", s);
      break;
    case 'd':
      d = va_arg(ap, int);
      printf("int %d\n", d);
      break;
    case 'c':
      c = (char) va_arg(ap, int);
      printf("char %c\n", c);
      break;
    case 'f':
      f = (float) va_arg(ap, double);
      printf("float %f\n", f);
      break;
    }
  }

  va_end(ap);
}

#define eprintf(...) fprintf(stderr, __VA_ARGS__)

int main(int argc, char *argv[])
{
  foo("dfsc", 3, 0.14, "Hello", 'h');

  eprintf("I am printing %d error(s).\n", "hello");

  eprintf();

  return 0;
}
