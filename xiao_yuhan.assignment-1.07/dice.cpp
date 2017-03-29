#include <cstdlib>
#include <stdio.h>
#include "dice.h"

ostream &operator<<(ostream& os, dice &dice)
{
	return os << dice.base << "+" << dice.dices << "d" << dice.sides;
}

int dice::get_final_num()
{
  int result = base;
  int i;
  for(i = 0; i < dices; i++){
    result += (1 + rand() % sides);
  }
  return result;
}
