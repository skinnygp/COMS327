#ifndef DUNGEON_H
# define DUNGEON_H

# include "heap.h"
# include "macros.h"
# include "dims.h"

#define DUNGEON_X              160
#define DUNGEON_Y              105
#define MIN_ROOMS              25
#define MAX_ROOMS              40
#define ROOM_MIN_X             7
#define ROOM_MIN_Y             5
#define ROOM_MAX_X             20
#define ROOM_MAX_Y             15
#define SAVE_DIR               ".rlg327"
#define DUNGEON_SAVE_FILE      "dungeon"
#define DUNGEON_SAVE_SEMANTIC  "RLG327-S2017"
#define DUNGEON_SAVE_VERSION   0U

#define mappair(pair) (d->map[pair[dim_y]][pair[dim_x]])
#define mapxy(x, y) (d->map[y][x])
#define hardnesspair(pair) (d->hardness[pair[dim_y]][pair[dim_x]])
#define hardnessxy(x, y) (d->hardness[y][x])

typedef enum __attribute__ ((__packed__)) terrain_type {
  ter_debug,
  ter_wall,
  ter_wall_immutable,
  ter_floor,
  ter_floor_room,
  ter_floor_hall,
  ter_monster,
} terrain_type_t;

typedef struct room {
  pair_t position;
  pair_t size;
} room_t;

static int seqs = 0;
static int target = 0;

typedef struct character{
  uint32_t room;
  pair_t position;
  pair_t last_position;
  uint8_t is_pc;
  uint8_t speed;
  uint32_t seq;
  uint32_t turn;
  uint8_t health;
  uint8_t is_intelligence;
  uint8_t is_telepathy;
  uint8_t is_tunneling;
  uint8_t is_erratic;
  uint32_t target;
  char characteristics;
} character_t;

typedef struct dungeon {
  uint32_t num_rooms;
  room_t *rooms;
  terrain_type_t map[DUNGEON_Y][DUNGEON_X];
  uint32_t nummon;
  /* Since hardness is usually not used, it would be expensive to pull it *
   * into cache every time we need a map cell, so we store it in a        *
   * parallel array, rather than using a structure to represent the       *
   * cells.  We may want a cell structure later, but from a performanace  *
   * perspective, it would be a bad idea to ever have the map be part of  *
   * that structure.  Pathfinding will require efficient use of the map,  *
   * and pulling in unnecessary data with each map cell would add a lot   *
   * of overhead to the memory system.                                    */
  uint8_t hardness[DUNGEON_Y][DUNGEON_X];
  uint8_t pc_distance[DUNGEON_Y][DUNGEON_X];
  uint8_t pc_tunnel[DUNGEON_Y][DUNGEON_X];
  character_t pc;
  character_t *monsters;
} dungeon_t;

void init_dungeon(dungeon_t *d);
void delete_dungeon(dungeon_t *d);
int gen_dungeon(dungeon_t *d);
void render_dungeon(dungeon_t *d);
int write_dungeon(dungeon_t *d, char *file);
int read_dungeon(dungeon_t *d, char *file);
int read_pgm(dungeon_t *d, char *pgm);
void excute(dungeon_t *d, int nummon);
int check_monsters_alive(dungeon_t *d);
void pc_move(dungeon_t *d);
void monster_move(dungeon_t *d, character_t *monster);
int line_of_sight(character_t *pc, character_t *monster);
void stupid_move(dungeon_t *d, character_t *monster, pair_t next, pair_t next_position);
void smart_move(dungeon_t *d, character_t *monster, pair_t next, pair_t next_position);
void stupid_tunneling_move(dungeon_t *d, character_t *monster, pair_t next, pair_t next_position);
void smart_tunneling_move(dungeon_t *d, character_t *monster, pair_t next, pair_t next_position);
#endif
