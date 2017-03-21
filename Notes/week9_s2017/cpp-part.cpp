#include <iostream>

#include "c-cpp-header.h"

void cpp_function(const char *s)
{
  cout << s;
}

ostream *return_cout(void)
{
  return &cout;
}

void use_cout(ostream *o)
{
  *o << "Using the pointer that I got from C code!" << endl;
}
