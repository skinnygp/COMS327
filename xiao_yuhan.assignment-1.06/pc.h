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
  terrain_type_t PCMap[DUNGEON_Y][DUNGEON_X];
  character_t *PCCharacter[DUNGEON_Y][DUNGEON_X];
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
void set_PCMap(pc_t *pc, int x, int y, terrain_type_t ch);
terrain_type_t get_PCMap(pc_t *pc, int x, int y);
void set_PCCharacter(pc_t *pc, int x, int y, character_t *ch);
character_t* get_PCCharacter(pc_t *pc, int x, int y);
# ifdef __cplusplus
}
# endif
#endif
