#ifndef DUNGEON_H
#define DUNGEON_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "heap.h"
#include "macros.h"

typedef enum dim {
  dim_x,
  dim_y,
  num_dims
} dim_t;

typedef int16_t pair_t[num_dims];

#define DUNGEON_X              160
#define DUNGEON_Y              96
#define MIN_ROOMS              12
#define MAX_ROOMS              24
#define ROOM_MIN_X             10
#define ROOM_MIN_Y             8
#define ROOM_SEPARATION        5
#define MAX_PLACEMENT_ATTEMPTS 1000
#define SAVE_DIR               ".rlg229"
#define DUNGEON_SAVE_FILE      "dungeon"
#define DUNGEON_SAVE_SEMANTIC  "RLG229"
#define DUNGEON_SAVE_VERSION   0U
#define VISUAL_RANGE           30
#define PC_SPEED               10
#define UP_KEY                 65
#define DOWN_KEY               66
#define RIGHT_KEY              67
#define LEFT_KEY               68
#define MIN_MONSTER_SPEED      5
#define MAX_MONSTER_SPEED      20
#define DELAY                  800
#define mappair(pair) (d->map[pair[dim_y]][pair[dim_x]])
#define mapxy(x, y) (d->map[y][x])
#define hardnesspair(pair) (d->hardness[pair[dim_y]][pair[dim_x]])
#define hardnessxy(x, y) (d->hardness[y][x])
#define NONE                  "\033[m"
#define RED                   "\033[0;32;31m"
#define LIGHT_RED             "\033[1;31m"
#define GREEN                 "\033[0;32;32m"
#define LIGHT_GREEN           "\033[1;32m"
#define BLUE                  "\033[0;32;34m"
#define LIGHT_BLUE            "\033[1;34m"
#define DARY_GRAY             "\033[1;30m"
#define CYAN                  "\033[0;36m"
#define LIGHT_CYAN            "\033[1;36m"
#define PURPLE                "\033[0;35m"
#define LIGHT_PURPLE          "\033[1;35m"
#define BROWN                 "\033[0;33m"
#define YELLOW                "\033[1;33m"
#define LIGHT_GRAY            "\033[0;37m"
#define WHITE                 "\033[1;37m"
static int16_t DIR_X[] = {0, 0, 1, -1};
static int16_t DIR_Y[] = {-1, 1, 0, 0};

typedef enum __attribute__ ((__packed__)) terrain_type {
  ter_pc,
  ter_monster_people,
  ter_monster_giant,
  ter_monster_dragon,
  ter_monster_smaug,
  ter_monster_ruth,
  ter_monster_falcor,
  ter_monster_saphira,
  ter_monster_norbert,
  ter_debug,
  ter_wall,
  ter_wall_no_room,
  ter_wall_no_floor,
  ter_wall_immutable,
  ter_floor,
  ter_floor_room,
  ter_floor_hall,
  ter_floor_tentative,
} terrain_type_t;

typedef struct room {
  pair_t position;
  pair_t size;
  uint32_t connected;
  uint8_t exists;
} room_t;

typedef struct dungeon {
  uint32_t num_rooms;
  room_t rooms[MAX_ROOMS];
  terrain_type_t map[DUNGEON_Y][DUNGEON_X];
  /* Since hardness is usually not used, it would be expensive to pull it *
   * into cache every time we need a map cell, so we store it in a        *
   * parallel array, rather than using a structure to represent the       *
   * cells.  We may want a cell structure later, but from a performanace  *
   * perspective, it would be a bad idea to ever have the map be part of  *
   * that structure.  Pathfinding will require efficient use of the map,  *
   * and pulling in unnecessary data with each map cell would add a lot   *
   * of overhead to the memory system.                                    */
  uint8_t hardness[DUNGEON_Y][DUNGEON_X];
} dungeon_t;

static uint8_t character_id = 0;

/* Represents a monster or pc. */
typedef struct character {
  uint8_t id;
  uint8_t is_monster;
  pair_t position;
  pair_t last_position;
  uint8_t is_smart;
  uint8_t is_telepathic;
  uint8_t is_dead;
  uint8_t speed;
  uint32_t turn;
} character_t;

int read_dungeon(dungeon_t *dungeon, char *filename);
int gen_dungeon(dungeon_t *dungeon);
void render_dungeon(dungeon_t *dungeon);
int write_dungeon(dungeon_t *dungeon);
void init_dungeon(dungeon_t *d);
void delete_dungeon(dungeon_t *d);
bool game_over(character_t *pc, character_t *monsters, int nummon);
void player_move(dungeon_t *d, character_t *pc, character_t *monsters, int nummon);
void monsters_move(dungeon_t *d, character_t *pc, character_t *monster);
void play(dungeon_t *d, int nummon);


#endif
