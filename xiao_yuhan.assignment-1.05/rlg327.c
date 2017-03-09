#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <ncurses.h>
#include "dungeon.h"
#include "pc.h"
#include "npc.h"
#include "move.h"
#include "path.h"

void do_look_mode(dungeon_t *d);
void portion(dungeon_t *d);
void PC_control(dungeon_t *d);

const char *victory =
  "\n                                       o\n"
  "                                      $\"\"$o\n"
  "                                     $\"  $$\n"
  "                                      $$$$\n"
  "                                      o \"$o\n"
  "                                     o\"  \"$\n"
  "                oo\"$$$\"  oo$\"$ooo   o$    \"$    ooo\"$oo  $$$\"o\n"
  "   o o o o    oo\"  o\"      \"o    $$o$\"     o o$\"\"  o$      \"$  "
  "\"oo   o o o o\n"
  "   \"$o   \"\"$$$\"   $$         $      \"   o   \"\"    o\"         $"
  "   \"o$$\"    o$$\n"
  "     \"\"o       o  $          $\"       $$$$$       o          $  ooo"
  "     o\"\"\n"
  "        \"o   $$$$o $o       o$        $$$$$\"       $o        \" $$$$"
  "   o\"\n"
  "         \"\"o $$$$o  oo o  o$\"         $$$$$\"        \"o o o o\"  "
  "\"$$$  $\n"
  "           \"\" \"$\"     \"\"\"\"\"            \"\"$\"            \""
  "\"\"      \"\"\" \"\n"
  "            \"oooooooooooooooooooooooooooooooooooooooooooooooooooooo$\n"
  "             \"$$$$\"$$$$\" $$$$$$$\"$$$$$$ \" \"$$$$$\"$$$$$$\"  $$$\""
  "\"$$$$\n"
  "              $$$oo$$$$   $$$$$$o$$$$$$o\" $$$$$$$$$$$$$$ o$$$$o$$$\"\n"
  "              $\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\""
  "\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"$\n"
  "              $\"                                                 \"$\n"
  "              $\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\""
  "$\"$\"$\"$\"$\"$\"$\"$\n"
  "                                   You win!\n\n";

const char *tombstone =
  "\n\n\n\n                /\"\"\"\"\"/\"\"\"\"\"\"\".\n"
  "               /     /         \\             __\n"
  "              /     /           \\            ||\n"
  "             /____ /   Rest in   \\           ||\n"
  "            |     |    Pieces     |          ||\n"
  "            |     |               |          ||\n"
  "            |     |   A. Luser    |          ||\n"
  "            |     |               |          ||\n"
  "            |     |     * *   * * |         _||_\n"
  "            |     |     *\\/* *\\/* |        | TT |\n"
  "            |     |     *_\\_  /   ...\"\"\"\"\"\"| |"
  "| |.\"\"....\"\"\"\"\"\"\"\".\"\"\n"
  "            |     |         \\/..\"\"\"\"\"...\"\"\""
  "\\ || /.\"\"\".......\"\"\"\"...\n"
  "            |     |....\"\"\"\"\"\"\"........\"\"\"\"\""
  "\"^^^^\".......\"\"\"\"\"\"\"\"..\"\n"
  "            |......\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"......"
  "..\"\"\"\"\"....\"\"\"\"\"..\"\"...\"\"\".\n\n"
  "            You're dead.  Better luck in the next life.\n\n\n";

void usage(char *name)
{
  fprintf(stderr,
          "Usage: %s [-r|--rand <seed>] [-l|--load [<file>]]\n"
          "          [-s|--save [<file>]] [-i|--image <pgm file>]\n"
          "          [-p|--pc <y> <x>] [-n|--nummon <count>]\n",
          name);

  exit(-1);
}

int main(int argc, char *argv[])
{
  dungeon_t d;
  time_t seed;
  struct timeval tv;
  uint32_t i;
  uint32_t do_load, do_save, do_seed, do_image, do_place_pc;
  uint32_t long_arg;
  char *save_file;
  char *load_file;
  char *pgm_file;

  memset(&d, 0, sizeof (d));

  /* Default behavior: Seed with the time, generate a new dungeon, *
   * and don't write to disk.                                      */
  do_load = do_save = do_image = do_place_pc = 0;
  do_seed = 1;
  save_file = load_file = NULL;
  d.max_monsters = MAX_MONSTERS;

  /* The project spec requires '--load' and '--save'.  It's common  *
   * to have short and long forms of most switches (assuming you    *
   * don't run out of letters).  For now, we've got plenty.  Long   *
   * forms use whole words and take two dashes.  Short forms use an *
    * abbreviation after a single dash.  We'll add '--rand' (to     *
   * specify a random seed), which will take an argument of it's    *
   * own, and we'll add short forms for all three commands, '-l',   *
   * '-s', and '-r', respectively.  We're also going to allow an    *
   * optional argument to load to allow us to load non-default save *
   * files.  No means to save to non-default locations, however.    *
   * And the final switch, '--image', allows me to create a dungeon *
   * from a PGM image, so that I was able to create those more      *
   * interesting test dungeons for you.                             */

 if (argc > 1) {
    for (i = 1, long_arg = 0; i < argc; i++, long_arg = 0) {
      if (argv[i][0] == '-') { /* All switches start with a dash */
        if (argv[i][1] == '-') {
          argv[i]++;    /* Make the argument have a single dash so we can */
          long_arg = 1; /* handle long and short args at the same place.  */
        }
        switch (argv[i][1]) {
        case 'r':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-rand")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%lu", &seed) /* Argument is not an integer */) {
            usage(argv[0]);
          }
          do_seed = 0;
          break;
        case 'l':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-load"))) {
            usage(argv[0]);
          }
          do_load = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            load_file = argv[++i];
          }
          break;
        case 's':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-save"))) {
            usage(argv[0]);
          }
          do_save = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            save_file = argv[++i];
          }
          break;
        case 'i':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-image"))) {
            usage(argv[0]);
          }
          do_image = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            pgm_file = argv[++i];
          }
          break;
        case 'n':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-nummon")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%hu", &d.max_monsters)) {
            usage(argv[0]);
          }
          break;
        case 'p':
          if ((!long_arg && argv[i][2])            ||
              (long_arg && strcmp(argv[i], "-pc")) ||
              argc <= i + 2                        ||
              argv[i + 1][0] == '-'                ||
              argv[i + 2][0] == '-') {
            usage(argv[0]);
          }
          do_place_pc = 1;
          if ((d.pc.position[dim_y] = atoi(argv[++i])) < 1 ||
              d.pc.position[dim_y] > DUNGEON_Y - 2         ||
              (d.pc.position[dim_x] = atoi(argv[++i])) < 1 ||
              d.pc.position[dim_x] > DUNGEON_X - 2) {
            fprintf(stderr, "Invalid PC position.\n");
            usage(argv[0]);
          }
          break;
        default:
          usage(argv[0]);
        }
      } else { /* No dash */
        usage(argv[0]);
      }
    }
  }

  if (do_seed) {
    /* Allows me to generate more than one dungeon *
     * per second, as opposed to time().           */
    gettimeofday(&tv, NULL);
    seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  }

  printf("Seed is %ld.\n", seed);
  srand(seed);

  initscr();
  raw();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  init_dungeon(&d);

  if (do_load) {
    read_dungeon(&d, load_file);
  } else if (do_image) {
    read_pgm(&d, pgm_file);
  } else {
    gen_dungeon(&d);
  }

  config_pc(&d);
  gen_monsters(&d);
  d.portion[dim_x] = (d.pc.position[dim_x] - 40);
  d.portion[dim_y] = (d.pc.position[dim_y] - 11);
  if (d.portion[dim_x] < 0) d.portion[dim_x] = 0;
  if (d.portion[dim_x] + 80 >= 159) d.portion[dim_x] = 79;
  if (d.portion[dim_y] < 0) d.portion[dim_y] = 0;
  if (d.portion[dim_y] + 21 >= 104) d.portion[dim_y] = 82;
  d.is_look_mode = 0;
  d.quit = 0;
  while (pc_is_alive(&d) && dungeon_has_npcs(&d) && d.quit == 0) {
    portion(&d);
    do_moves(&d);
    if(d.is_look_mode == 0){
      if(d.portion[dim_x] + 79 - d.pc.position[dim_x] < 5){
        d.portion[dim_x] = (d.pc.position[dim_x] - 40);
      }
      if(d.pc.position[dim_x] - d.portion[dim_x] < 5){
        d.portion[dim_x] = (d.pc.position[dim_x] - 40);
      }
      if(d.portion[dim_y] + 21 - d.pc.position[dim_y] < 5){
        d.portion[dim_y] = (d.pc.position[dim_y] - 11);
      }
      if(d.pc.position[dim_y] - d.portion[dim_y] < 5){
        d.portion[dim_y] = (d.pc.position[dim_y] - 11);
      }
      if (d.portion[dim_x] < 0) d.portion[dim_x] = 0;
      if (d.portion[dim_x] + 80 >= 159) d.portion[dim_x] = 79;
      if (d.portion[dim_y] < 0) d.portion[dim_y] = 0;
      if (d.portion[dim_y] + 21 >= 104) d.portion[dim_y] = 82;
    }
    PC_control(&d);
  }
  portion(&d);
  endwin();
  if (do_save) {
    write_dungeon(&d, save_file);
  }
  if(d.quit == 1){
    printf("You quit!");
    printf("\nYou defended your life in the face of %u deadly beasts.\n"
           "You avenged the cruel and untimely murders of %u peaceful dungeon residents.\n",
           d.pc.kills[kill_direct], d.pc.kills[kill_avenged]);
  }
  else{
    printf(pc_is_alive(&d) ? victory : tombstone);
    printf("\nYou defended your life in the face of %u deadly beasts.\n"
           "You avenged the cruel and untimely murders of %u peaceful dungeon residents.\n",
           d.pc.kills[kill_direct], d.pc.kills[kill_avenged]);
  }


  pc_delete(d.pc.pc);

  delete_dungeon(&d);

  return 0;
}

void portion(dungeon_t *d)
{
  pair_t p;
  clear();
  for (p[dim_y] = 1; p[dim_y] < 22; p[dim_y]++) {
    for (p[dim_x] = 0; p[dim_x] < 80; p[dim_x]++) {
      if (charxy(d->portion[dim_x] + p[dim_x],d->portion[dim_y] + p[dim_y])){
        mvaddch(p[dim_y], p[dim_x], charxy(d->portion[dim_x] + p[dim_x], d->portion[dim_y] + p[dim_y])->symbol);
      } else {
        switch (mapxy(d->portion[dim_x] + p[dim_x],
                      d->portion[dim_y] + p[dim_y])) {
        case ter_debug:
        case ter_wall:
        case ter_wall_immutable:
          mvaddch(p[dim_y], p[dim_x], ' ');
          break;
        case ter_stair_up:
          mvaddch(p[dim_y], p[dim_x], '<');
          break;
        case ter_stair_down:
          mvaddch(p[dim_y], p[dim_x], '>');
          break;
        case ter_floor:
        case ter_floor_room:
          mvaddch(p[dim_y], p[dim_x], '.');
          break;
        case ter_floor_hall:
          mvaddch(p[dim_y], p[dim_x], '#');
          break;
        }
      }
    }
  }
  mvprintw(0, 0, "PC position: %d %d.",d->pc.position[dim_x], d->pc.position[dim_y]);
  if(d->is_look_mode){
    mvprintw(22, 0, "Look Mode: Press esc to enter Control Mode.");
  }
  else mvprintw(22, 0, "Control Mode: Press L to enter Look Mode");
  refresh();
}

void PC_control(dungeon_t *d)
{
  pair_t next;
  next[dim_x] = d->pc.position[dim_x];
  next[dim_y] = d->pc.position[dim_y];

  int unbound = 1;
  int input;
  while(unbound){
    input = getch();
    switch (input) {
      case 81:
        d->quit = 1;
        printf("Quit Game!");
        return;
      case 55:
      case 121:
        next[dim_x]--;
        next[dim_y]--;
        if(mappair(next) == ter_wall || mappair(next) == ter_wall_immutable){
          mvprintw(23, 0, "Wrong Way!");
          unbound = 1;
          next[dim_x]++;
          next[dim_y]++;
        }
        else {
          move_character(d, &d->pc, next);
          unbound = 0;
          dijkstra(d);
          dijkstra_tunnel(d);
        }
        break;
      case 56:
      case 107:
        next[dim_y]--;
        if(mappair(next) == ter_wall || mappair(next) == ter_wall_immutable){
          mvprintw(23, 0, "Wrong Way!");
          unbound = 1;
          next[dim_y]++;
        }
        else {
          move_character(d, &d->pc, next);
          unbound = 0;
          dijkstra(d);
          dijkstra_tunnel(d);
        }
        break;
      case 57:
      case 117:
        next[dim_x]++;
        next[dim_y]--;
        if(mappair(next) == ter_wall || mappair(next) == ter_wall_immutable){
          mvprintw(23, 0, "Wrong Way!");
          unbound = 1;
          next[dim_x]--;
          next[dim_y]++;
        }
        else {
          move_character(d, &d->pc, next);
          unbound = 0;
          dijkstra(d);
          dijkstra_tunnel(d);
        }
        break;
      case 54:
      case 108:
        next[dim_x]++;
        if(mappair(next) == ter_wall || mappair(next) == ter_wall_immutable){
          mvprintw(23, 0, "Wrong Way!");
          unbound = 1;
          next[dim_x]--;
        }
        else {
          move_character(d, &d->pc, next);
          unbound = 0;
          dijkstra(d);
          dijkstra_tunnel(d);
        }
        break;
      case 51:
      case 110:
        next[dim_x]++;
        next[dim_y]++;
        if(mappair(next) == ter_wall || mappair(next) == ter_wall_immutable){
          mvprintw(23, 0, "Wrong Way!");
          unbound = 1;
          next[dim_x]--;
          next[dim_y]--;
        }
        else {
          move_character(d, &d->pc, next);
          unbound = 0;
          dijkstra(d);
          dijkstra_tunnel(d);
        }
        break;
      case 50:
      case 106:
        next[dim_y]++;
        if(mappair(next) == ter_wall || mappair(next) == ter_wall_immutable){
          mvprintw(23, 0, "Wrong Way!");
          unbound = 1;
          next[dim_y]--;
        }
        else {
          move_character(d, &d->pc, next);
          unbound = 0;
          dijkstra(d);
          dijkstra_tunnel(d);
        }
        break;
      case 49:
      case 98:
        next[dim_x]--;
        next[dim_y]++;
        if(mappair(next) == ter_wall || mappair(next) == ter_wall_immutable){
          mvprintw(23, 0, "Wrong Way!");
          unbound = 1;
          next[dim_x]++;
          next[dim_y]--;
        }
        else {
          move_character(d, &d->pc, next);
          unbound = 0;
          dijkstra(d);
          dijkstra_tunnel(d);
        }
        break;
      case 52:
      case 104:
        next[dim_x]--;
        if(mappair(next) == ter_wall || mappair(next) == ter_wall_immutable){
          mvprintw(23, 0, "Wrong Way!");
          unbound = 1;
          next[dim_x]++;
        }
        else {
          move_character(d, &d->pc, next);
          unbound = 0;
          dijkstra(d);
          dijkstra_tunnel(d);
        }
        break;
      case 53:
      case 32:
        unbound = 0;
        break;
      case 62:
        if(mappair(next) == ter_stair_down){
          unbound = 0;
          delete_dungeon(d);
	        free(d->pc.pc);
          init_dungeon(d);
          gen_dungeon(d);
          config_pc(d);
          gen_monsters(d);
          d->portion[dim_x] = (d->pc.position[dim_x] - 40);
          d->portion[dim_y] = (d->pc.position[dim_y] - 11);
          if (d->portion[dim_x] < 0) d->portion[dim_x] = 0;
          if (d->portion[dim_x] + 80 >= 159) d->portion[dim_x] = 79;
          if (d->portion[dim_y] < 0) d->portion[dim_y] = 0;
          if (d->portion[dim_y] + 21 >= 104) d->portion[dim_y] = 83;
          d->is_look_mode = 0;
          d->quit = 0;
        }
        else{
          mvprintw(23, 0, "There is no down stairs!");
          unbound = 1;
        }
        break;
      case 60:
        if(mappair(next) == ter_stair_up){
          unbound = 0;
          delete_dungeon(d);
	        free(d->pc.pc);
          init_dungeon(d);
          gen_dungeon(d);
          config_pc(d);
          gen_monsters(d);
          d->portion[dim_x] = (d->pc.position[dim_x] - 40);
          d->portion[dim_y] = (d->pc.position[dim_y] - 11);
          if (d->portion[dim_x] < 0) d->portion[dim_x] = 0;
          if (d->portion[dim_x] + 80 >= 159) d->portion[dim_x] = 79;
          if (d->portion[dim_y] < 0) d->portion[dim_y] = 0;
          if (d->portion[dim_y] + 21 >= 104) d->portion[dim_y] = 83;
          d->is_look_mode = 0;
          d->quit = 0;
        }
        else{
          mvprintw(23, 0, "There is no up stairs!");
          unbound = 1;
        }
        break;
      case 76:
        unbound = 0;
        d->is_look_mode = 1;
        do_look_mode(d);
        break;
    }
  }
}

void do_look_mode(dungeon_t *d)
{
  portion(d);
  int input;
  while(1){
    input = getch();
    switch (input) {
      case 27:
        d->portion[dim_x] = (d->pc.position[dim_x] - 40);
        d->portion[dim_y] = (d->pc.position[dim_y] - 11);
        if (d->portion[dim_x] < 0) d->portion[dim_x] = 0;
        if (d->portion[dim_x] + 80 >= 159) d->portion[dim_x] = 79;
        if (d->portion[dim_y] < 0) d->portion[dim_y] = 0;
        if (d->portion[dim_y] + 21 >= 104) d->portion[dim_y] = 83;
        d->is_look_mode = 0;
        portion(d);
        return;
      case 56:
      case 107:
        if(d->portion[dim_y] - 3 > 0){
          d->portion[dim_y] -= 3;
        }
        break;
      case 54:
      case 108:
        if(d->portion[dim_x] + 3 < 79){
          d->portion[dim_x] += 3;
        }
        break;
      case 50:
      case 106:
        if(d->portion[dim_y] + 3 < 83){
          d->portion[dim_y] += 3;
        }
        break;
      case 52:
      case 104:
        if(d->portion[dim_x] - 3 > 0){
          d->portion[dim_x] -= 3;
        }
        break;
    }
    portion(d);
  }
}
