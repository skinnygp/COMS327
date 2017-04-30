#include <iostream>

using namespace std;

class asteroid;

class space_ship {
public:
  virtual void collide_with(asteroid &in_asteroid);
};

class apollo_spacecraft : public space_ship {
public:
  virtual void collide_with(asteroid &in_asteroid);
};

class asteroid {
public:
  virtual void collide_with(space_ship&) {
    cout << "asteroid hit a space ship" << endl;
  }
  virtual void collide_with(apollo_spacecraft&) {
    cout << "asteroid hit the apollo spacecraft" << endl;
  }
};

class exploding_asteroid : public asteroid {
public:
  virtual void collide_with(space_ship&) {
    cout << "exploding asteroid hit a space ship" << endl;
  }
  virtual void collide_with(apollo_spacecraft&) {
    cout << "exploding asteroid hit the apollo spacecraft" << endl;
  }
};

int main(int argc, char *argv[])
{
  asteroid the_asteroid;
  exploding_asteroid the_exploding_asteroid;
  space_ship the_space_ship;
  apollo_spacecraft the_apollo_spacecraft;

  //static dispatch
  the_asteroid.collide_with(the_space_ship);
  the_asteroid.collide_with(the_apollo_spacecraft);
  the_exploding_asteroid.collide_with(the_space_ship);
  the_exploding_asteroid.collide_with(the_apollo_spacecraft);

  cout << endl;

  //(single) dynamic dispatch
  asteroid &the_asteroid_reference = the_exploding_asteroid;
  the_asteroid_reference.collide_with(the_space_ship);
  the_asteroid_reference.collide_with(the_apollo_spacecraft);

  //static dispatch
  space_ship &the_space_ship_reference = the_apollo_spacecraft;
  the_asteroid.collide_with(the_space_ship_reference);
  the_exploding_asteroid.collide_with(the_space_ship_reference);
  
  //(simulated) double dispatch
  the_space_ship_reference.collide_with(the_asteroid_reference);

  return 0;
}

void space_ship::collide_with(asteroid &in_asteroid)
{
  in_asteroid.collide_with(*this);
}

void apollo_spacecraft::collide_with(asteroid &in_asteroid)
{
  in_asteroid.collide_with(*this);
}
