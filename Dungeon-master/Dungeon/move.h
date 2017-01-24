#ifndef MOVE_H
#define MOVE_H

#include "dungeon.h"

void dijkstra(dungeon_t *d, pair_t from, pair_t to);

void smart_move(dungeon_t *d, character_t *monster, pair_t to, pair_t next);
void non_smart_move(dungeon_t *d, character_t *monster, pair_t to, pair_t next);

#endif
