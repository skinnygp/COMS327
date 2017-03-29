#include <iostream>
#include <cstdlib>
#include <ncurses.h>
#include <cstring>
#include <cstdio.h>
#include <sstream>

#include "parser.h"
#include "dungeon.h"
#include "npc.h"
#include "dice.h"
#include "character.h"
#include "utils.h"


int parse_symb(string &Line)
{
  if(line.length() == 1) return line[0];
  return -1;
}


int parse_desc(string &Line)
{
  stringstream input(line);
  while(!input.eof()){
    string name;
    input >> name;
    if(name.empty()) return -1;
    else return name;
  }
  return -1;
}

int parse_color(string &Line)
{
  stringstream input(line);
  while(!input.eof()){
    string name;
    input >> name;
    if(name.empty()) return -1;
    else return name;
  }
  return -1;
}
int parse_dice(string &Line)
{
  stringstream input(line);
  while(!input.eof()){
    string name;
    input >> name;
    if(name.empty()) return -1;
    else return name;
  }
  return -1;
}
int parse_abil(string &Line)
{
  stringstream input(line);
  while(!input.eof()){
    string name;
    input >> name;
    if(name.empty()) return -1;
    else return name;
  }
  return -1;
}
