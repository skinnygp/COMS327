#include "c-cpp-header.h"

int main(int argc, char *argv[])
{
  ostream *o;

  cpp_function("Hello World!\n");

  o = return_cout();

  use_cout(o);

  return 0;
}
