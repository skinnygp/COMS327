#ifdef __cplusplus
extern "C"
{
  #endif
  #include <stdlib.h>

  #include "string.h"

  #include "dungeon.h"
  #include "pc.h"
  #include "utils.h"
  #include "move.h"
  #include "path.h"
  #ifdef __cplusplus
}
  #endif


void pc_delete(pc_t *pc)
{
  delete pc;
}

uint32_t pc_is_alive(dungeon_t *d)
{
  return d->pc->alive;
}

void place_pc(dungeon_t *d)
{
  d->pc->position[dim_y] = rand_range(d->rooms->position[dim_y],
                                     (d->rooms->position[dim_y] +
                                      d->rooms->size[dim_y] - 1));
  d->pc->position[dim_x] = rand_range(d->rooms->position[dim_x],
                                     (d->rooms->position[dim_x] +
                                      d->rooms->size[dim_x] - 1));
}

void config_pc(dungeon_t *d)
{
  /*memset(&d->pc, 0, sizeof (d->pc));*/
  d->pc = new pc_t;
  d->pc->symbol = '@';

  place_pc(d);

  d->pc->speed = PC_SPEED;
  d->pc->alive = 1;
  d->pc->sequence_number = 0;
  //d->pc.pc = calloc(1, sizeof (*d->pc.pc));
  //d->pc.npc = NULL;

  d->character[d->pc->position[dim_y]][d->pc->position[dim_x]] = d->pc;

  dijkstra(d);
  dijkstra_tunnel(d);
}
uint32_t pc_in_room(dungeon_t *d, uint32_t room)
{
  if ((room < d->num_rooms)                                     &&
      (d->pc->position[dim_x] >= d->rooms[room].position[dim_x]) &&
      (d->pc->position[dim_x] < (d->rooms[room].position[dim_x] +
                                d->rooms[room].size[dim_x]))    &&
      (d->pc->position[dim_y] >= d->rooms[room].position[dim_y]) &&
      (d->pc->position[dim_y] < (d->rooms[room].position[dim_y] +
                                d->rooms[room].size[dim_y]))) {
    return 1;
  }

  return 0;
}
