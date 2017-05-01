#include <iostream>

using namespace std;

class A {
public:
  A() { cout << "Constructing A" << endl; }
  A(const A& a) { cout << "Copy constructing A" << endl; }
  virtual ~A() { cout << "Destroying A" << endl; }
  virtual void print() { cout << "A" << endl; }
};

class B : virtual public A {
public:
  B() { cout << "Constructing B" << endl; }
  B(const B& b) { cout << "Copy constructing B" << endl; }
  virtual ~B() { cout << "Destroying B" << endl; }
  virtual void print() { cout << "B" << endl; }
};

class C : virtual public A {
public:
  C() { cout << "Constructing C" << endl; }
  C(const C& c) { cout << "Copy constructing C" << endl; }
  virtual ~C() { cout << "Destroying C" << endl; }
  virtual void print() { cout << "C" << endl; }
};

class D : virtual public B, virtual public C {
public:
  D() { cout << "Constructing D" << endl; }
  D(const D& d) { cout << "Copy constructing D" << endl; }
  virtual ~D() { cout << "Destroying D" << endl; }
  virtual void print() { cout << "D" << endl; }
};

int main(int argc, char *argv[])
{
  
  A a;
  a.print();
  B b;
  b.print();
  C c;
  c.print();
  

  D d;
  d.print();

  d.A::print();

  A *e = new D;
  e->print();

  delete e;

  return 0;
}
