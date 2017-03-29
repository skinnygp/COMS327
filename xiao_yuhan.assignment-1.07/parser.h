#ifndef PARSER_H
#define PARSER_H
#include <iostream>
#include <vector>
using namespace std;

class Monster_Definitions
{
    string name;
		string desc;
		string symb;
		string color;
		string speed;
		string abil;
		string hp;
		string dam;
    Monster_Definitions() : name(), desc(), symb(), color(), speed(), abil(), hp(), dam(){}
    Monster_Definitions(string name, string desc, string symb, string color,
      string speed, string abil, string hp, string dam) : name(name), desc(desc),
      symb(symb), color(color),
      speed(speed), abil(abil), hp(hp), dam(dam)
      {
      }
    int checkFailure();
    ostream &print(ostream &o);
};
ostream &operator<<(ostream &o, Monster_Definitions &m);
int parse(vector<Monster_Definitions> &m);
int printMD(vector<Monster_Definitions> &m);
int deleteMD(vector<Monster_Definitions> &m);
#endif
