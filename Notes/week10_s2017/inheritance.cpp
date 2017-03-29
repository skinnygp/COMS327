#include <iostream>
#include <cstdlib>

using namespace std;

class shape {
public:
  int color;
  virtual double area() = 0;
  virtual double perimeter() = 0;
  virtual ~shape() {} // Destructor must always be virtual if class will
                      // be inherited from
};

class rectangle : public shape {
private:
  double side1, side2;
public:
  double area();
  double perimeter();
  rectangle() {}
  rectangle(double s1, double s2)
  {
    side1 = s1;
    side2 = s2;
    color = 0;
  }
  ~rectangle() {}
};

class square : public shape {
private:
  double side;
public:
  double area();
  double perimeter();
  square() {}
  square(double s) {
    side = s;
    color = 3;
  }
  ~square() {}
};

double rectangle::area()
{
  return side1 * side2;
}

double rectangle::perimeter()
{
  return 2 * (side1 + side2);
}

double square::area()
{
  return side * side;
}

double square::perimeter()
{
  return 4 * side;
}

int main(int argc, char *argv[])
{
  square s(3);
  rectangle r(5, 7);
  shape *sp;
  shape &sr = r;

  sp = &s;

  cout << s.color << " " << s.area() << " " << s.perimeter() << endl;
  cout << r.color << " " << r.area() << " " << r.perimeter() << endl;
  cout << sp->color << " " << sp->area() << " " << sp->perimeter() << endl;
  cout << sr.color << " " << sr.area() << " " << sr.perimeter() << endl;

  shape *a[20];
  int i;

  for (i = 0; i < 20; i++) {
    if (rand() & 1) {
      a[i] = new square(3 * (i + 1));
    } else {
      a[i] = new rectangle(2 * (i + 1), 5 * (i + 1));
    }
  }

  for (i = 0; i < 20; i++) {
    cout << a[i]->color << " " << a[i]->area() << " " << a[i]->perimeter() << endl;
  }
  
  for (i = 0; i < 20; i++) {
    delete a[i];
  }

  return 0;
}
