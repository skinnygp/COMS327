#include <vector>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
  vector<int> v;
  int i;

  for (i = 0; i < 100; i++) {
    v.push_back(i);
  }

  for (i = 0; i < 100; i++) {
    cout << v[i] << endl;
  }

  cout << v.size() << endl;

  vector<int>::iterator it;

  for (it = v.begin(); it != v.end(); it += 2) {
    if (*it > 50) {
      *it -= 50;
    }
    cout << *it << endl;
  }

  return 0;
}
