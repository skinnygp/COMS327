#ifndef DICE_H
#define DICE_H
#include <iostream>

class dice{
    int base, dices, sides;
  public:
    //constructure for dice
    dice(int base, int dices, int sides) : base(base), dices(dices), sides(sides){}
    int get_random_num();
    int get_base(){return this->base;};
    int get_dices(){return this->dices;};
    int get_sides(){return this->sides;};
};

#endif
