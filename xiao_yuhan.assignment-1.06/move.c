#include "move.h"

#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "dungeon.h"
#include "heap.h"
#include "move.h"
#include "npc.h"
#include "pc.h"
#include "character.h"
#include "utils.h"
#include "path.h"
#include "event.h"

void do_combat(dungeon_t *d, character_t *atk, character_t *def)
{
  if (isAlive(def)) {
    setAlive(def, 0);
    if (def != d->pc) {
      d->num_monsters--;
    }
  }
}

void move_character(dungeon_t *d, character_t *c, pair_t next)
{
  if (charpair(next) &&
      ((next[dim_y] != getY(c) ||
       (next[dim_x] != getX(c))))) {
    do_combat(d, c, charpair(next));
  } else {
    /* No character in new position. */

    d->character[getY(c)][getX(c)] = NULL;
    setY(c, next[dim_y]);
    setX(c, next[dim_x]);
    d->character[getY(c)][getX(c)] = c;
  }
}

void do_moves(dungeon_t *d)
{
  pair_t next;
  character_t *c;
  event_t *e;

  /* Remove the PC when it is PC turn.  Replace on next call.  This allows *
   * use to completely uninit the heap when generating a new level without *
   * worrying about deleting the PC.                                       */

  if (pc_is_alive(d)) {
    /* The PC always goes first one a tie, so we don't use new_event().  *
     * We generate one manually so that we can set the PC sequence       *
     * number to zero.                                                   */
    e = malloc(sizeof (*e));
    e->type = event_character_turn;
    /* The next line is buggy.  Monsters get first turn before PC.  *
     * Monster gen code always leaves PC in a monster-free room, so *
     * not a big issue, but it needs a better solution.             */
    e->time = d->time + (1000 / getSpeed(d->pc))
;
    e->sequence = 0;
    e->c = d->pc;
    heap_insert(&d->events, e);
  }

  while (pc_is_alive(d) &&
         (e = heap_remove_min(&d->events)) &&
         ((e->type != event_character_turn) || (e->c != d->pc))) {
    d->time = e->time;
    if (e->type == event_character_turn) {
      c = e->c;
    }
    if (!isAlive(c)) {
      if (d->character[getY(c)][getX(c)] == c) {
        d->character[getY(c)][getX(c)] = NULL;
      }
      if (c != d->pc) {
        event_delete(e);
      }
      continue;
    }

    npc_next_pos(d, c, next);
    move_character(d, c, next);

    heap_insert(&d->events, update_event(d, e, 1000 / getSpeed(c)));
  }

  if (pc_is_alive(d) && e->c == d->pc) {
    c = e->c;
    d->time = e->time;
    /* Kind of kludgey, but because the PC is never in the queue when   *
     * we are outside of this function, the PC event has to get deleted *
     * and recreated every time we leave and re-enter this function.    */
    e->c = NULL;
    event_delete(e);
  }
}

void dir_nearest_wall(dungeon_t *d, character_t *c, pair_t dir)
{
  dir[dim_x] = dir[dim_y] = 0;

  if (getX(c) != 1 && getX(c) != DUNGEON_X - 2) {
    dir[dim_x] = (getX(c) > DUNGEON_X - getX(c) ? 1 : -1);
  }
  if (getY(c) != 1 && getY(c) != DUNGEON_Y - 2) {
    dir[dim_y] = (getY(c) > DUNGEON_Y - getY(c) ? 1 : -1);
  }
}

uint32_t against_wall(dungeon_t *d, character_t *c)
{
  return ((mapxy(getX(c) - 1, getY(c)) == ter_wall_immutable) ||
          (mapxy(getX(c) + 1, getY(c)) == ter_wall_immutable) ||
          (mapxy(getX(c), getY(c) - 1) == ter_wall_immutable) ||
          (mapxy(getX(c), getY(c) + 1) == ter_wall_immutable));
}

uint32_t in_corner(dungeon_t *d, character_t *c)
{
  uint32_t num_immutable;

  num_immutable = 0;

  num_immutable += (mapxy(getX(c) - 1,
                          getY(c)   ) == ter_wall_immutable);
  num_immutable += (mapxy(getX(c) + 1,
                          getY(c)    ) == ter_wall_immutable);
  num_immutable += (mapxy(getX(c)    ,
                          getY(c) - 1) == ter_wall_immutable);
  num_immutable += (mapxy(getX(c)    ,
                          getY(c) + 1) == ter_wall_immutable);

  return num_immutable > 1;
}
