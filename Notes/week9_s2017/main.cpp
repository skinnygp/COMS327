#include <iostream>

#include "string327.h"

using namespace std;

int main(int argc, char *argv[])
{
  string327 s;
  string327 t = "Hello"; // Calls the non-default constructor
  string327 u("Hi");     // Calls the same non-default constructor

  s = "Have a good break";

  cout << s << endl << t << endl << u << endl;

  s += " (even if you aren't going anywhere).";

  cout << s << endl;

  cout << s[19] << endl;

  s[19] = 'E';

  cout << s << endl;  

  return 0;
}
