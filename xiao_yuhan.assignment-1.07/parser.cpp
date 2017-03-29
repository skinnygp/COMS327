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
    int check = 0;
    for(!fs.eof(), count = 0, !check; count < 8; count ++){
      int have_name, have_desc, have_color, have_speed, have_symb, have_dam, have_hp, have_abil;
      Monster_Definitions m;
      string name, desc, color, speed, symb, dam, hp, abil;
      getline(fs, line);
      string op;
			stringstream s(line);
			s >> op;

      if(op == "NAME"){
        if(have_name){
          cerr << "Duplicate field Name";
          check = 1;
        }
        else{
          while (s.peek() == ' ') s.get();
          if(s.peek() == '\n'){
            cerr << "Empty Name";
            check = 1;
          }
          getline(s, name);
          have_name = 1;
        }
      }

      else if(op == "SYMB"){
        if(have_symb){
          cerr << "Duplicate field Symbol";
          check = 1;
        }
        else if(strlen(line.substr(5)) > 1){
          cerr << "Incorrect length of Symbol";
          check = 1;
        }
        else{
          while (s.peek() == ' ') s.get();
          if(s.peek() == '/n'){
            cerr << "Empty Symbol";
            check = 1;
          }
          getline(s, symbol);
          have_name = 1;
        }
      }

      else if(op == "DESC"){
        if(have_desc){
          cerr << "Duplicate field Description";
          check = 1;
        }
        else if(line.substr(5) != ""){
          cerr << "Format wrong Description";
          check = 1;
        }
        else{
          have_desc = 1;
          while(!fs.eof()){
            getline(fs, line);
            if(line.c_str()[0]=='.') break;
            else if(strlen(line) > 77){
              cerr << "Length is too long for Description";
              check = 1;
              break;
            }
            else {
              desc += line;
              desc += "\n";
            }
          }
        }
      }

      else if(op == "COLOR"){
        if(have_color){
          cerr << "Duplicate field Color";
          check = 1;
        }
        while (s.peek() == ' ') s.get();
        if(s.peek() == '/n'){
          cerr << "Empty Color";
          check = 1;
        }
        getline(s, color);
        if(color != "BLACK" || color != "RED" || color != "GREEN" || color != "YELLOW" || color != "BLUE" ||
           color != "MAGENTA" || color != "CYAN" || color != "WHITE" || ){
             cerr << "Wrong Color";
             check = 1;
           }
      }

      else if(op == "SPEED"){
        if(have_speed){
          cerr << "Duplicate field Speed";
          check = 1;
        }
        while (s.peek() == ' ') s.get();
        if(s.peek() == '/n'){
          cerr << "Empty Speed";
          check = 1;
        }
        getline(s, speed);


      }

      else if(op == "DAM"){

      }

      else if(op == "HP"){

      }

      else if(op == "ABIL"){

      }

      else if(op == "END"){
        break;
      }
      else check = 1;
    }
  }



}
int printMD(vector<Monster_Definitions> &m)
{

}
int deleteMD(vector<Monster_Definitions> &m)
{

}
