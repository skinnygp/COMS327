#include <stdlib.h>

#include "utils.h"
#include "npc.h"
#include "dungeon.h"
#include "character.h"
#include "move.h"
#include "path.h"
#include "event.h"
#include "pc.h"

void npc_delete(npc *n)
{
  if (n) {
    free(n);
  }
}

static uint32_t max_monster_cells(dungeon_t *d)
{
  uint32_t i;
  uint32_t sum;

  for (i = sum = 0; i < d->num_rooms; i++) {
    if (!pc_in_room(d, i)) {
      sum += d->rooms[i].size[dim_y] * d->rooms[i].size[dim_x];
    }
  }

  return sum;
}

void gen_monsters(dungeon_t *d)
{
  uint32_t i;
  uint32_t c;

  if (d->max_monsters < (c = max_monster_cells(d))) {
    d->num_monsters = d->max_monsters;
  } else {
    d->num_monsters = c;
  }

  for (i = 0; i < d->num_monsters; i++) {
    monster_description::generate_monster(d);
  }
}

void npc_next_pos_rand_tunnel(dungeon_t *d, character *c, pair_t next)
{
  pair_t n;
  union {
    uint32_t i;
    uint8_t a[4];
  } r;

  do {
    n[dim_y] = next[dim_y];
    n[dim_x] = next[dim_x];
    r.i = rand();
    if (r.a[0] > 85 /* 255 / 3 */) {
      if (r.a[0] & 1) {
        n[dim_y]--;
      } else {
        n[dim_y]++;
      }
    }
    if (r.a[1] > 85 /* 255 / 3 */) {
      if (r.a[1] & 1) {
        n[dim_x]--;
      } else {
        n[dim_x]++;
      }
    }
  } while (mappair(n) == ter_wall_immutable);

  if (hardnesspair(n) <= 60) {
    if (hardnesspair(n)) {
      hardnesspair(n) = 0;
      mappair(n) = ter_floor_hall;

      /* Update distance maps because map has changed. */
      /* Comment out for performance
      dijkstra(d);
      dijkstra_tunnel(d);
      */
    }

    next[dim_x] = n[dim_x];
    next[dim_y] = n[dim_y];
  } else {
    hardnesspair(n) -= 60;
  }
}

void npc_next_pos_rand(dungeon_t *d, character *c, pair_t next)
{
  pair_t n;
  union {
    uint32_t i;
    uint8_t a[4];
  } r;

  do {
    n[dim_y] = next[dim_y];
    n[dim_x] = next[dim_x];
    r.i = rand();
    if (r.a[0] > 85 /* 255 / 3 */) {
      if (r.a[0] & 1) {
        n[dim_y]--;
      } else {
        n[dim_y]++;
      }
    }
    if (r.a[1] > 85 /* 255 / 3 */) {
      if (r.a[1] & 1) {
        n[dim_x]--;
      } else {
        n[dim_x]++;
      }
    }
  } while (mappair(n) < ter_floor);

  next[dim_y] = n[dim_y];
  next[dim_x] = n[dim_x];
}

void npc_next_pos_rand_pass(dungeon_t *d, character *c, pair_t next)
{
  pair_t n;
  union {
    uint32_t i;
    uint8_t a[4];
  } r;

  do {
    n[dim_y] = next[dim_y];
    n[dim_x] = next[dim_x];
    r.i = rand();
    if (r.a[0] > 85 /* 255 / 3 */) {
      if (r.a[0] & 1) {
        n[dim_y]--;
      } else {
        n[dim_y]++;
      }
    }
    if (r.a[1] > 85 /* 255 / 3 */) {
      if (r.a[1] & 1) {
        n[dim_x]--;
      } else {
        n[dim_x]++;
      }
    }
  } while (mappair(n) == ter_wall_immutable);

  next[dim_y] = n[dim_y];
  next[dim_x] = n[dim_x];
}

void npc_next_pos_line_of_sight(dungeon_t *d, character *c, pair_t next)
{
  pair_t dir;

  dir[dim_y] = character_get_y(d->PC) - c->position[dim_y];
  dir[dim_x] = character_get_x(d->PC) - c->position[dim_x];
  if (dir[dim_y]) {
    dir[dim_y] /= abs(dir[dim_y]);
  }
  if (dir[dim_x]) {
    dir[dim_x] /= abs(dir[dim_x]);
  }

  if (((npc *) c)->characteristics & NPC_PASS_WALL) {
    next[dim_x] += dir[dim_x];
    next[dim_y] += dir[dim_y];
  } else {
    if (mapxy(next[dim_x] + dir[dim_x],
              next[dim_y] + dir[dim_y]) >= ter_floor) {
      next[dim_x] += dir[dim_x];
      next[dim_y] += dir[dim_y];
    } else if (mapxy(next[dim_x] + dir[dim_x], next[dim_y]) >= ter_floor) {
      next[dim_x] += dir[dim_x];
    } else if (mapxy(next[dim_x], next[dim_y] + dir[dim_y]) >= ter_floor) {
      next[dim_y] += dir[dim_y];
    }
  }
}

void npc_next_pos_line_of_sight_tunnel(dungeon_t *d,
                                       character *c,
                                       pair_t next)
{
  pair_t dir;

  dir[dim_y] = character_get_y(d->PC) - c->position[dim_y];
  dir[dim_x] = character_get_x(d->PC) - c->position[dim_x];
  if (dir[dim_y]) {
    dir[dim_y] /= abs(dir[dim_y]);
  }
  if (dir[dim_x]) {
    dir[dim_x] /= abs(dir[dim_x]);
  }

  dir[dim_x] += next[dim_x];
  dir[dim_y] += next[dim_y];

  if (hardnesspair(dir) <= 60) {
    if (hardnesspair(dir)) {
      hardnesspair(dir) = 0;
      mappair(dir) = ter_floor_hall;

      /* Update distance maps because map has changed. */
      /* Comment out for performance
      dijkstra(d);
      dijkstra_tunnel(d);
      */
    }

    next[dim_x] = dir[dim_x];
    next[dim_y] = dir[dim_y];
  } else {
    hardnesspair(dir) -= 60;
  }
}

void npc_next_pos_gradient(dungeon_t *d, npc *c, pair_t next)
{
  /* Handles both tunneling and non-tunneling versions */
  pair_t min_next;
  uint16_t min_cost;
  if (c->characteristics & NPC_TUNNEL) {
    min_cost = (d->pc_tunnel[next[dim_y] - 1][next[dim_x]] +
                (d->hardness[next[dim_y] - 1][next[dim_x]] / 60));
    min_next[dim_x] = next[dim_x];
    min_next[dim_y] = next[dim_y] - 1;
    if ((d->pc_tunnel[next[dim_y] + 1][next[dim_x]    ] +
         (d->hardness[next[dim_y] + 1][next[dim_x]] / 60)) < min_cost) {
      min_cost = (d->pc_tunnel[next[dim_y] + 1][next[dim_x]] +
                  (d->hardness[next[dim_y] + 1][next[dim_x]] / 60));
      min_next[dim_x] = next[dim_x];
      min_next[dim_y] = next[dim_y] + 1;
    }
    if ((d->pc_tunnel[next[dim_y]    ][next[dim_x] + 1] +
         (d->hardness[next[dim_y]    ][next[dim_x] + 1] / 60)) < min_cost) {
      min_cost = (d->pc_tunnel[next[dim_y]][next[dim_x] + 1] +
                  (d->hardness[next[dim_y]][next[dim_x] + 1] / 60));
      min_next[dim_x] = next[dim_x] + 1;
      min_next[dim_y] = next[dim_y];
    }
    if ((d->pc_tunnel[next[dim_y]    ][next[dim_x] - 1] +
         (d->hardness[next[dim_y]    ][next[dim_x] - 1] / 60)) < min_cost) {
      min_cost = (d->pc_tunnel[next[dim_y]][next[dim_x] - 1] +
                  (d->hardness[next[dim_y]][next[dim_x] - 1] / 60));
      min_next[dim_x] = next[dim_x] - 1;
      min_next[dim_y] = next[dim_y];
    }
    if ((d->pc_tunnel[next[dim_y] - 1][next[dim_x] + 1] +
         (d->hardness[next[dim_y] - 1][next[dim_x] + 1] / 60)) < min_cost) {
      min_cost = (d->pc_tunnel[next[dim_y] - 1][next[dim_x] + 1] +
                  (d->hardness[next[dim_y] - 1][next[dim_x] + 1] / 60));
      min_next[dim_x] = next[dim_x] + 1;
      min_next[dim_y] = next[dim_y] - 1;
    }
    if ((d->pc_tunnel[next[dim_y] + 1][next[dim_x] + 1] +
         (d->hardness[next[dim_y] + 1][next[dim_x] + 1] / 60)) < min_cost) {
      min_cost = (d->pc_tunnel[next[dim_y] + 1][next[dim_x] + 1] +
                  (d->hardness[next[dim_y] + 1][next[dim_x] + 1] / 60));
      min_next[dim_x] = next[dim_x] + 1;
      min_next[dim_y] = next[dim_y] + 1;
    }
    if ((d->pc_tunnel[next[dim_y] - 1][next[dim_x] - 1] +
         (d->hardness[next[dim_y] - 1][next[dim_x] - 1] / 60)) < min_cost) {
      min_cost = (d->pc_tunnel[next[dim_y] - 1][next[dim_x] - 1] +
                  (d->hardness[next[dim_y] - 1][next[dim_x] - 1] / 60));
      min_next[dim_x] = next[dim_x] - 1;
      min_next[dim_y] = next[dim_y] - 1;
    }
    if ((d->pc_tunnel[next[dim_y] + 1][next[dim_x] - 1] +
         (d->hardness[next[dim_y] + 1][next[dim_x] - 1] / 60)) < min_cost) {
      min_cost = (d->pc_tunnel[next[dim_y] + 1][next[dim_x] - 1] +
                  (d->hardness[next[dim_y] + 1][next[dim_x] - 1] / 60));
      min_next[dim_x] = next[dim_x] - 1;
      min_next[dim_y] = next[dim_y] + 1;
    }
    if (hardnesspair(min_next) <= 60) {
      if (hardnesspair(min_next)) {
        hardnesspair(min_next) = 0;
        mappair(min_next) = ter_floor_hall;

        /* Update distance maps because map has changed. */
        dijkstra(d);
        dijkstra_tunnel(d);
      }

      next[dim_x] = min_next[dim_x];
      next[dim_y] = min_next[dim_y];
    } else {
      hardnesspair(min_next) -= 60;
    }
  } else {
    /* Make monsters prefer cardinal directions */
    if (d->pc_distance[next[dim_y] - 1][next[dim_x]    ] <
        d->pc_distance[next[dim_y]][next[dim_x]]) {
      next[dim_y]--;
      return;
    }
    if (d->pc_distance[next[dim_y] + 1][next[dim_x]    ] <
        d->pc_distance[next[dim_y]][next[dim_x]]) {
      next[dim_y]++;
      return;
    }
    if (d->pc_distance[next[dim_y]    ][next[dim_x] + 1] <
        d->pc_distance[next[dim_y]][next[dim_x]]) {
      next[dim_x]++;
      return;
    }
    if (d->pc_distance[next[dim_y]    ][next[dim_x] - 1] <
        d->pc_distance[next[dim_y]][next[dim_x]]) {
      next[dim_x]--;
      return;
    }
    if (d->pc_distance[next[dim_y] - 1][next[dim_x] + 1] <
        d->pc_distance[next[dim_y]][next[dim_x]]) {
      next[dim_y]--;
      next[dim_x]++;
      return;
    }
    if (d->pc_distance[next[dim_y] + 1][next[dim_x] + 1] <
        d->pc_distance[next[dim_y]][next[dim_x]]) {
      next[dim_y]++;
      next[dim_x]++;
      return;
    }
    if (d->pc_distance[next[dim_y] - 1][next[dim_x] - 1] <
        d->pc_distance[next[dim_y]][next[dim_x]]) {
      next[dim_y]--;
      next[dim_x]--;
      return;
    }
    if (d->pc_distance[next[dim_y] + 1][next[dim_x] - 1] <
        d->pc_distance[next[dim_y]][next[dim_x]]) {
      next[dim_y]++;
      next[dim_x]--;
      return;
    }
  }
}

static void npc_next_pos_00(dungeon_t *d, npc *c, pair_t next)
{
  /* not smart; not telepathic; not tunneling; not erratic */
  if (can_see(d, character_get_pos(c),character_get_pos(d->PC), 0, 0)) {
    c->pc_last_known_position[dim_y] = character_get_y(d->PC);
    c->pc_last_known_position[dim_x] = character_get_x(d->PC);
    npc_next_pos_line_of_sight(d, c, next);
  } else {
    npc_next_pos_rand(d, c, next);
  }
}

static void npc_next_pos_01(dungeon_t *d, npc *c, pair_t next)
{
  /*     smart; not telepathic; not tunneling; not erratic */
  if (can_see(d, character_get_pos(c), character_get_pos(d->PC), 0, 0)) {
    c->pc_last_known_position[dim_y] = character_get_y(d->PC);
    c->pc_last_known_position[dim_x] = character_get_x(d->PC);
    c->have_seen_pc = 1;
    npc_next_pos_line_of_sight(d, c, next);
  } else if (c->have_seen_pc) {
    npc_next_pos_line_of_sight(d, c, next);
  }

  if ((next[dim_x] == c->pc_last_known_position[dim_x]) &&
      (next[dim_y] == c->pc_last_known_position[dim_y])) {
    c->have_seen_pc = 0;
  }
}

static void npc_next_pos_02(dungeon_t *d, npc *c, pair_t next)
{
  /* not smart;     telepathic; not tunneling; not erratic */
  c->pc_last_known_position[dim_y] = character_get_y(d->PC);
  c->pc_last_known_position[dim_x] = character_get_x(d->PC);
  npc_next_pos_line_of_sight(d, c, next);
}

static void npc_next_pos_03(dungeon_t *d, npc *c, pair_t next)
{
  /*     smart;     telepathic; not tunneling; not erratic */
  npc_next_pos_gradient(d, c, next);
}

static void npc_next_pos_04(dungeon_t *d, npc *c, pair_t next)
{
  /* not smart; not telepathic;     tunneling; not erratic */
  if (can_see(d, character_get_pos(c), character_get_pos(d->PC), 0, 0)) {
    c->pc_last_known_position[dim_y] = character_get_y(d->PC);
    c->pc_last_known_position[dim_x] = character_get_x(d->PC);
    npc_next_pos_line_of_sight(d, c, next);
  } else {
    npc_next_pos_rand_tunnel(d, c, next);
  }
}

static void npc_next_pos_05(dungeon_t *d, npc *c, pair_t next)
{
  /*     smart; not telepathic;     tunneling; not erratic */
  if (can_see(d, character_get_pos(c), character_get_pos(d->PC), 0, 0)) {
    c->pc_last_known_position[dim_y] = character_get_y(d->PC);
    c->pc_last_known_position[dim_x] = character_get_x(d->PC);
    c->have_seen_pc = 1;
    npc_next_pos_line_of_sight(d, c, next);
  } else if (c->have_seen_pc) {
    npc_next_pos_line_of_sight_tunnel(d, c, next);
  }

  if ((next[dim_x] == c->pc_last_known_position[dim_x]) &&
      (next[dim_y] == c->pc_last_known_position[dim_y])) {
    c->have_seen_pc = 0;
  }
}

static void npc_next_pos_06(dungeon_t *d, npc *c, pair_t next)
{
  /* not smart;     telepathic;     tunneling; not erratic */
  c->pc_last_known_position[dim_y] = character_get_y(d->PC);
  c->pc_last_known_position[dim_x] = character_get_x(d->PC);
  npc_next_pos_line_of_sight_tunnel(d, c, next);
}

static void npc_next_pos_07(dungeon_t *d, npc *c, pair_t next)
{
  /*     smart;     telepathic;     tunneling; not erratic */
  npc_next_pos_gradient(d, c, next);
}

static void npc_next_pos_08(dungeon_t *d, npc *c, pair_t next)
{
  /* not smart; not telepathic; not tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand(d, c, next);
  } else {
    npc_next_pos_00(d, c, next);
  }
}

static void npc_next_pos_09(dungeon_t *d, npc *c, pair_t next)
{
  /*     smart; not telepathic; not tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand(d, c, next);
  } else {
    npc_next_pos_01(d, c, next);
  }
}

static void npc_next_pos_0a(dungeon_t *d, npc *c, pair_t next)
{
  /* not smart;     telepathic; not tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand(d, c, next);
  } else {
    npc_next_pos_02(d, c, next);
  }
}

static void npc_next_pos_0b(dungeon_t *d, npc *c, pair_t next)
{
  /*     smart;     telepathic; not tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand(d, c, next);
  } else {
    npc_next_pos_03(d, c, next);
  }
}

static void npc_next_pos_0c(dungeon_t *d, npc *c, pair_t next)
{
  /* not smart; not telepathic;     tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand_tunnel(d, c, next);
  } else {
    npc_next_pos_04(d, c, next);
  }
}

static void npc_next_pos_0d(dungeon_t *d, npc *c, pair_t next)
{
  /*     smart; not telepathic;     tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand_tunnel(d, c, next);
  } else {
    npc_next_pos_05(d, c, next);
  }
}

static void npc_next_pos_0e(dungeon_t *d, npc *c, pair_t next)
{
  /* not smart;     telepathic;     tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand_tunnel(d, c, next);
  } else {
    npc_next_pos_06(d, c, next);
  }
}

static void npc_next_pos_0f(dungeon_t *d, npc *c, pair_t next)
{
  /*     smart;     telepathic;     tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand_tunnel(d, c, next);
  } else {
    npc_next_pos_07(d, c, next);
  }
}

static void npc_next_pos_10(dungeon_t *d, npc *c, pair_t next)
{
  npc_next_pos_00(d, c, next);
}

static void npc_next_pos_11(dungeon_t *d, npc *c, pair_t next)
{
  /* pass wall;     smart; not telepathic; not tunneling; not erratic */
  if (can_see(d, character_get_pos(c), character_get_pos(d->PC), 0, 0)) {
    c->pc_last_known_position[dim_y] = character_get_y(d->PC);
    c->pc_last_known_position[dim_x] = character_get_x(d->PC);
    c->have_seen_pc = 1;
    npc_next_pos_line_of_sight(d, c, next);
  } else if (c->have_seen_pc) {
    npc_next_pos_line_of_sight(d, c, next);
  }

  if ((next[dim_x] == c->pc_last_known_position[dim_x]) &&
      (next[dim_y] == c->pc_last_known_position[dim_y])) {
    c->have_seen_pc = 0;
  }
}

static void npc_next_pos_12(dungeon_t *d, npc *c, pair_t next)
{
  /* pass wall; not smart;     telepathic; not tunneling; not erratic */
  c->pc_last_known_position[dim_y] = character_get_y(d->PC);
  c->pc_last_known_position[dim_x] = character_get_x(d->PC);
  npc_next_pos_line_of_sight(d, c, next);
}

static void npc_next_pos_13(dungeon_t *d, npc *c, pair_t next)
{
  /* pass wall;     smart;     telepathic; not tunneling; not erratic */
  c->pc_last_known_position[dim_y] = character_get_y(d->PC);
  c->pc_last_known_position[dim_x] = character_get_x(d->PC);
  npc_next_pos_line_of_sight(d, c, next);
}

static void npc_next_pos_14(dungeon_t *d, npc *c, pair_t next)
{
  /* pass wall; not smart; not telepathic;     tunneling; not erratic */
  if (can_see(d, character_get_pos(c), character_get_pos(d->PC), 0, 0)) {
    c->pc_last_known_position[dim_y] = character_get_y(d->PC);
    c->pc_last_known_position[dim_x] = character_get_x(d->PC);
    npc_next_pos_line_of_sight(d, c, next);
  } else {
    npc_next_pos_rand_pass(d, c, next);
  }
}

static void npc_next_pos_15(dungeon_t *d, npc *c, pair_t next)
{
  npc_next_pos_11(d, c, next);
}

static void npc_next_pos_16(dungeon_t *d, npc *c, pair_t next)
{
  /* pass wall; not smart;     telepathic;     tunneling; not erratic */
  c->pc_last_known_position[dim_y] = character_get_y(d->PC);
  c->pc_last_known_position[dim_x] = character_get_x(d->PC);
  npc_next_pos_line_of_sight(d, c, next);
}

static void npc_next_pos_17(dungeon_t *d, npc *c, pair_t next)
{
  /* pass wall;     smart;     telepathic;     tunneling; not erratic */
  c->pc_last_known_position[dim_y] = character_get_y(d->PC);
  c->pc_last_known_position[dim_x] = character_get_x(d->PC);
  npc_next_pos_line_of_sight(d, c, next);
}

static void npc_next_pos_18(dungeon_t *d, npc *c, pair_t next)
{
  /* pass wall; not smart; not telepathic; not tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand_pass(d, c, next);
  } else {
    npc_next_pos_10(d, c, next);
  }
}

static void npc_next_pos_19(dungeon_t *d, npc *c, pair_t next)
{
  /* pass wall;     smart; not telepathic; not tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand_pass(d, c, next);
  } else {
    npc_next_pos_11(d, c, next);
  }
}

static void npc_next_pos_1a(dungeon_t *d, npc *c, pair_t next)
{
  /* pass wall; not smart;     telepathic; not tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand_pass(d, c, next);
  } else {
    npc_next_pos_12(d, c, next);
  }
}

static void npc_next_pos_1b(dungeon_t *d, npc *c, pair_t next)
{
  /* pass wall;     smart;     telepathic; not tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand_pass(d, c, next);
  } else {
    npc_next_pos_13(d, c, next);
  }
}

static void npc_next_pos_1c(dungeon_t *d, npc *c, pair_t next)
{
  /* pass wall; not smart; not telepathic;     tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand_pass(d, c, next);
  } else {
    npc_next_pos_14(d, c, next);
  }
}

static void npc_next_pos_1d(dungeon_t *d, npc *c, pair_t next)
{
  /* pass wall;     smart; not telepathic;     tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand_pass(d, c, next);
  } else {
    npc_next_pos_15(d, c, next);
  }
}

static void npc_next_pos_1e(dungeon_t *d, npc *c, pair_t next)
{
  /* pass wall; not smart;     telepathic;     tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand_pass(d, c, next);
  } else {
    npc_next_pos_16(d, c, next);
  }
}

static void npc_next_pos_1f(dungeon_t *d, npc *c, pair_t next)
{
  /* pass wall;     smart;     telepathic;     tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand_pass(d, c, next);
  } else {
    npc_next_pos_17(d, c, next);
  }
}

void (*npc_move_func[])(dungeon_t *d, npc *c, pair_t next) = {
  /* We'll have one function for each combination of bits, so the *
   * order is based on binary counting through the NPC_* bits.    *
   * It could be very easy to mess this up, so be careful.  We'll *
   * name them according to their hex value.                      */
  npc_next_pos_00,
  npc_next_pos_01,
  npc_next_pos_02,
  npc_next_pos_03,
  npc_next_pos_04,
  npc_next_pos_05,
  npc_next_pos_06,
  npc_next_pos_07,
  npc_next_pos_08,
  npc_next_pos_09,
  npc_next_pos_0a,
  npc_next_pos_0b,
  npc_next_pos_0c,
  npc_next_pos_0d,
  npc_next_pos_0e,
  npc_next_pos_0f,
  npc_next_pos_10,
  npc_next_pos_11,
  npc_next_pos_12,
  npc_next_pos_13,
  npc_next_pos_14,
  npc_next_pos_15,
  npc_next_pos_16,
  npc_next_pos_17,
  npc_next_pos_18,
  npc_next_pos_19,
  npc_next_pos_1a,
  npc_next_pos_1b,
  npc_next_pos_1c,
  npc_next_pos_1d,
  npc_next_pos_1e,
  npc_next_pos_1f,
};

void npc_next_pos(dungeon_t *d, npc *c, pair_t next)
{
  next[dim_y] = c->position[dim_y];
  next[dim_x] = c->position[dim_x];

  npc_move_func[c->characteristics & 0x0000001f](d, c, next);
}

uint32_t dungeon_has_npcs(dungeon_t *d)
{
  return d->num_monsters;
}

npc::npc(dungeon_t *d, const monster_description &m)
{
  pair_t p;
  uint32_t room;
  uint32_t i;

  symbol = m.symbol;
  color = m.color;
  room = rand_range(1, d->num_rooms - 1);
  do {
    p[dim_y] = rand_range(d->rooms[room].position[dim_y],
                          (d->rooms[room].position[dim_y] +
                           d->rooms[room].size[dim_y] - 1));
    p[dim_x] = rand_range(d->rooms[room].position[dim_x],
                          (d->rooms[room].position[dim_x] +
                           d->rooms[room].size[dim_x] - 1));
  } while (d->character_map[p[dim_y]][p[dim_x]]);
  pc_last_known_position[dim_y] = p[dim_y];
  pc_last_known_position[dim_x] = p[dim_x];
  position[dim_y] = p[dim_y];
  position[dim_x] = p[dim_x];
  d->character_map[p[dim_y]][p[dim_x]] = this;
  speed = m.speed.roll();
  hp = m.hitpoints.roll();
  damage = &m.damage;
  alive = 1;
  sequence_number = ++d->character_sequence_number;
  characteristics = m.abilities;
  have_seen_pc = 0;
  name = m.name.c_str();
  description = (const char *) m.description.c_str();
  for (i = 0; i < num_kill_types; i++) {
    kills[i] = 0;
  }
  EXP = 0;
  rank = rand_range(1, 100);
}
