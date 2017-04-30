#include <iostream>
#include <vector>
#include <string>

#define mmax(x, y) x > y ? x : y

template <class T>
T &max(T& x, T& y) {
  return x > y ? x : y;
}

int main(int argc, char *argv[])
{
  int x, y;

  x = 7;
  y = 11;

  max(x, y) = 5;

  std::cout << "max is " << max(x, y) << std::endl;
  std::cout << "x is " << x << " and y is " << y << std::endl;

  double a, b;
  a = 3.14;
  b = 1.414;

  std::cout << "max is " << max(a, b) << std::endl;

  //  max(v1, v2); // Professor cannot come up with an example that fails :(

  return 0;
}
