#include <iostream>
#include <cstdlib>

using namespace std;

//Two example of static that exist in C and C++
/*
static int func()
{
  static int num_calls = 0;

  return ++num_calls;
}
*/

class singleton {
private:
  static singleton *the_instance;
  singleton() { i = 0; }
  singleton &operator=(const singleton &s) { return *the_instance; }
public:
  int i;
  static singleton *instance();
  // Break the singleton by being an idiot
  singleton *break_singleton() { return new singleton(); }
};

singleton *singleton::the_instance;

singleton *singleton::instance() {
  if (!the_instance) {
    the_instance = new singleton();
  }

  return the_instance;
}

int main(int argc, char *argv[])
{
  singleton *s = singleton::instance();
  singleton *t = singleton::instance();

  cout << "s is " << s->i << endl;

  t->i = 2;

  cout << "s is " << s->i << endl;
  cout << "t is " << t->i << endl;

  /* Break the singleton by going around the C++ protections */
  t = (singleton *) malloc(sizeof (*t));

  t->i = 4;

  cout << "s is " << s->i << endl;
  cout << "t is " << t->i << endl;

  delete s;
  free(t);

  return 0;
}
