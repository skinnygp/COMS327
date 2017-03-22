#ifndef PC_H
# define PC_H
#ifdef __cplusplus
extern "C"
{
  #endif
  # include <stdint.h>
  # include "dims.h"
  # include "character.h"
  # include "dungeon.h"
  #ifdef __cplusplus
}
  #endif
# ifdef __cplusplus
class pc_t : public character_t {
 public:
  unsigned char PCMap[DUNGEON_Y][DUNGEON_X];
};
/*typedef struct pc {
} pc_t;*/
extern "C" {
# else
typedef void pc_t;
# endif

typedef struct dungeon dungeon_t;
#include "dungeon.h"

void pc_delete(pc_t *pc);
uint32_t pc_is_alive(dungeon_t *d);
void config_pc(dungeon_t *d);
void place_pc(dungeon_t *d);
uint32_t pc_in_room(dungeon_t *d, uint32_t room);

void pc_learn_terrain(pc_t *p, pair_t pos, terrain_type_t ter);
terrain_type_t pc_learned_terrain(pc_t *p, int8_t y, int8_t x);
void pc_init_known_terrain(pc_t *p);
void pc_observe_terrain(pc_t *p, dungeon_t *d);
int32_t is_illuminated(pc_t *p, int8_t y, int8_t x);
void pc_reset_visibility(pc_t *p);

# ifdef __cplusplus
}
# endif
#endif
