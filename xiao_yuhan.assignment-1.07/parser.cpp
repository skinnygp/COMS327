#include <iostream>
#include <cstdlib>
#include <ncurses.h>
#include <cstring>
#include <cstdio.h>
#include <sstream>
#include <fstream>
#include <vector>
using namespace std;
#include "parser.h"
// #include "dungeon.h"
// #include "npc.h"
// #include "dice.h"
// #include "character.h"
// #include "utils.h"

int parse(vector<Monster_Definitions> &m)
{
  string file;
  ifstream fs;
  file = getnev("HOME");
  file += "/.rlg327/monster_desc.txt";
  fs.open(file.c_str());
  if(!fs.is_open()){
    cerr << "File cannot open";
    return -1;
  }
  string line;
  // stringstream metadata;
  // metadata << "RLG327 MONSTER DESCRIPTION 1";
  getline(fs, line);
  if(line != "RLG327 MONSTER DESCRIPTION 1"){
    cerr << "metadata is not correct";
    return -1;
  }


  while(f.peek() != EOF){
    while(line != "BEGIN MONSTER" && fs.peek() != EOF){
      getline(fs, line);
    }
    int count;
    for(getline(file,line), count = 0; count < 8; count ++){
      int have_name, have_desc, have_color, have_speed, have_symb, have_dam, have_hp, have_abil;
      Monster_Definitions m;
      string name, desc, color, speed, symb, dam, hp, abil;
      getline(file, line);

      if(line.substr(0, 4) == "NAME"){
        if(have_name){
          cerr << "Duplicate field Name";
        }
        else if(line.substr(5) == ""){
          cerr << "Empty Name";
        }
        else{
          name = line.substr(5);
          have_name = 1;
        }
      }

      else if(line.substr(0, 4) == "SYMB"){
        if(have_symb){
          cerr << "Duplicate field Symbol";
        }
        else if(line.substr(5) == ""){
          cerr << "Empty Symbol";
        }
        else if(strlen(line.substr(5)) > 1){
          cerr << "Incorrect length of Symbol";
        }
        else{
          symbol = line.substr(5);
          have_name = 1;
        }
      }

      else if(line.substr(0, 4) == "DESC"){
        if(have_desc){
          cerr << "Duplicate field Description";
        }
        else if()
      }

    }
  }



}
int printMD(vector<Monster_Definitions> &m)
{

}
int deleteMD(vector<Monster_Definitions> &m)
{

}
