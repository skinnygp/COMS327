#ifndef PARSER_H
#define PARSER_H
#include <iostream>
using namespace std;
class Monster_Definitions
{
  public:
    int parse_name(string &Line);
    int parse_symb(string &Line);
    int parse_desc(string &Line);
    int parse_color(string &Line);
    int parse_speed(string &Line);
    int parse_abil(string &Line);
    int parse_hp(string &Line);
    int parse_dam(string &Line);

};

#endif
