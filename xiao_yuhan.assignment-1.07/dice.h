#ifndef DICE_H
#define DICE_H
#include <iostream>

using namespace std;

class dice{
  public:
    int base, dices, sides;
    //constructure for dice
    dice(int base, int dices, int sides) : base(base), dices(dices), sides(sides){}
    int get_final_num();
};
ostream& operator<<(ostream &os, dice &dice);

#endif
