#include <iostream>

using namespace std;

class greater_than {
private:
  int value;
public:
  greater_than(int i) : value(i) {}
  bool operator()(int i) { return i > value; }
};

int main(int argc, char *argv[])
{
  greater_than g17(17);
  greater_than g93(93);
  int i;

  for (i = 0; i < 100; i++) {
    if (g17(i)) {
      cout << i << " is greater than 17" << endl;
    }
    if (g93(i)) {
      cout << i << " is greater than 93" << endl;
    }
  }

  return 0;
}
