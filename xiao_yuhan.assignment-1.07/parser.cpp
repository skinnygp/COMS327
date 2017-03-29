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
    int have_name, have_desc, have_color, have_speed, have_symb, have_dam, have_hp, have_abil;
    Monster_Definitions mon;
    string name, desc, color, speed, symb, dam, hp, abil;
    for(!fs.eof(), count = 0, !check; count < 8; count ++){
      getline(fs, line);
      string op;
			stringstream s(line);
			s >> op;

      if(op == "NAME"){
        if(have_name){
          cerr << "Duplicate field Name\n" << endl;
          check = 1;
        }
        else{
          while (s.peek() == ' ') s.get();
          if(s.peek() == '\n'){
            cerr << "Empty Name\n" << endl;
            check = 1;
          }
          getline(s, name);
          have_name = 1;
        }
      }

      else if(op == "SYMB"){
        if(have_symb){
          cerr << "Duplicate field Symbol\n" << endl;
          check = 1;
        }
        else if(strlen(line.substr(5)) > 1){
          cerr << "Incorrect length of Symbol\n" << endl;
          check = 1;
        }
        else{
          while (s.peek() == ' ') s.get();
          if(s.peek() == '/n'){
            cerr << "Empty Symbol\n" << endl;
            check = 1;
          }
          getline(s, symbol);
          have_symb = 1;
        }
      }

      else if(op == "DESC"){
        if(have_desc){
          cerr << "Duplicate field Description\n" << endl;
          check = 1;
        }
        else if(line.substr(5) != ""){
          cerr << "Format wrong Description\n" << endl;
          check = 1;
        }
        else{
          have_desc = 1;
          while(!fs.eof()){
            getline(fs, line);
            have_desc = 1;
            if(line.c_str()[0]=='.') break;
            else if(strlen(line) > 77){
              cerr << "Length is too long for Description\n" << endl;
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
          cerr << "Duplicate field Color\n" << endl;
          check = 1;
        }
        while (s.peek() == ' ') s.get();
        if(s.peek() == '/n'){
          cerr << "Empty Color\n" << endl;
          check = 1;
        }
        getline(s, color);
        have_color = 1;
        if(!(color == "BLACK" || color == "RED" || color == "GREEN" || color == "YELLOW" || color == "BLUE" ||
           color == "MAGENTA" || color == "CYAN" || color == "WHITE") ){
             cerr << "Wrong Color\n" << endl;
             check = 1;
           }
      }

      else if(op == "SPEED"){
        int base, dices, sides;
        if(have_speed){
          cerr << "Duplicate field Speed\n" << endl;
          check = 1;
        }
        while (s.peek() == ' ') s.get();
        if(s.peek() == '/n'){
          cerr << "Empty Speed\n" << endl;
          check = 1;
        }
        getline(s, speed);
        have_speed = 1;
        if(sscanf(speed.c_str(), "%d+%dd%d", &base, &dices, &sides) != 3){
          check = 1;
        }
      }

      else if(op == "DAM"){
        int base, dices, sides;
        if(have_dam){
          cerr << "Duplicate field Damage\n" << endl;
          check = 1;
        }
        while (s.peek() == ' ') s.get();
        if(s.peek() == '/n'){
          cerr << "Empty Damage\n" << endl;
          check = 1;
        }
        getline(s, dam);
        have_dam = 1;
        if(sscanf(dam.c_str(), "%d+%dd%d", &base, &dices, &sides) != 3){
          check = 1;
        }
      }

      else if(op == "HP"){
        int base, dices, sides;
        if(have_hp){
          cerr << "Duplicate field HP\n" << endl;
          check = 1;
        }
        while (s.peek() == ' ') s.get();
        if(s.peek() == '/n'){
          cerr << "Empty HP";
          check = 1;
        }
        getline(s, hp);
        have_hp = 1;
        if(sscanf(hp.c_str(), "%d+%dd%d", &base, &dices, &sides) != 3){
          check = 1;
        }
      }

      else if(op == "ABIL"){
        if(have_abil){
          cerr << "Duplicate field Ability\n" << endl;
          check = 1;
        }
        while (s.peek() == ' ') s.get();
        if(s.peek() == '/n'){
          cerr << "Empty ABIL\n" << endl;
          check = 1;
        }
        getline(s, abil);
        have_abil = 1;
        if(!(abil == "SMART" || abil == "TELE" || abil == "TUNNEL" || abil == "ERRATIC" || abil == "PASS," ||
           abil == "PICKUP" || abil == "DESTROY")){
             check = 1;
           }
      }
      else if(op == "END"){
        cerr << "Wrong number of descriptions\n" << endl;
        check = 1;
      }
      else {
        cerr << "Wrong name of descriptions\n" << endl;
        check = 1;
      }
    }
    if(!getline(fs, line)){
      cerr << "No END";
      check = 1;
    }
    getline(fs, line);
    if(line != "END"){
      cerr << "Wrong number of descriptions\n" << endl;
      check = 1;
    }
    if(!check){
      mon = Monster_Definitions(name, desc, symb, color, speed, abil, hp, dam);
    }
    m.push_back(mon);

  }
  return 1;
}

int printMD(vector<Monster_Definitions> &m)
{
  vector<Monster_Definitions>::iterator i;
  for (i = m.begin(); i != m.end(); i++) {
    cout << *i << endl;
  }
  return 1;
}

int deleteMD(vector<Monster_Definitions> &m)
{
  m.clear();
  return 1;
}
