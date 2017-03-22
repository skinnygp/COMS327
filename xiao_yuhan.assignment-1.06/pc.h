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

# ifdef __cplusplus
}
# endif
#endif
