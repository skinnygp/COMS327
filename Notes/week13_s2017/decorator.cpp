#include <iostream>

using namespace std;

class shape {
public:
  virtual ~shape() {}
  virtual void draw() = 0;
};

class square : public shape {
public:
  void draw() {
    cout << "[]" << endl;
  };
};

class circle : public shape {
public:
  void draw() {
    cout << "()" << endl;
  };
};

class puce_square : public square {
public:
  void draw() {
    cout << "puce-[]" << endl;
  }
};

class shape_decorator : public shape {
private:
  shape *s;
public:
  shape_decorator(shape *s) : s(s) {}
  ~shape_decorator() { delete s; }
  void draw() { s->draw(); }
};

class puce_shape : public shape_decorator {
public:
  puce_shape(shape *s) : shape_decorator(s) {}
  void draw()
  {
    cout << "puce-";
    shape_decorator::draw();
  }
};

class big_shape : public shape_decorator {
public:
  big_shape(shape *s) : shape_decorator(s) {}
  void draw()
  {
    cout << "big-";
    shape_decorator::draw();
  }
};

int main(int argc, char *argv[])
{
  square s;
  circle c;
  puce_square p;
  puce_shape ps(new puce_shape(new big_shape(new circle())));

  s.draw();
  c.draw();
  p.draw();
  ps.draw();

  return 0;
}
