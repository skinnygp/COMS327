#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <cstring>

using namespace std;

class greater_than {
private:
  int value;
public:
  greater_than(int i) : value(i) {}
  bool operator()(int i) { return i > value; }
};

class print_greater_than {
private:
  int value;
public:
  print_greater_than(int i) : value(i) {}
  bool operator()(int i) { if (i > value) cout << i << endl; return i > value; }
};

int print_greater_than_93(int i)
{
  if (i > 93) {
    cout << i << endl;

    return 0;
  }
  return 1;
}

void print_longer_than(char *s)
{
  if (strlen(s) > 10) {
    cout << s << endl;
  }
}

int main(int argc, char *argv[])
{
  greater_than g17(17);
  greater_than g93(93);
  print_greater_than pg93(93);
  int i;
  vector<int> v;
  vector<int>::iterator vi;

  for (i = 0; i < 100; i++) {
    v.push_back(rand() % 100);
  }

  for (i = 0; i < 100; i++) {
    //    cout << v[i] << endl;
  }

  //  cout << "--------------------------------------------------------------------------------" << endl;
  for (vi = v.begin(); (vi = find_if(vi, v.end(), g93)) != v.end(); vi++) {
    //    cout << *vi << endl;
  }

  //  for_each(v.begin(), v.end(), pg93);

  vi = find(v.begin(), v.end(), 93);

  if (vi == v.end()) {
    cout << "Item not found" << endl;
  } else {
    cout << *vi << " was found" << endl;
  }



  return 0;
}
