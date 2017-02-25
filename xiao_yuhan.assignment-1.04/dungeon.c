#include <stdio.h>
#include <stdint.h>
// #include <endian.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include "dungeon.h"
#include "utils.h"
#include "heap.h"

typedef struct corridor_path {
  heap_node_t *hn;
  uint8_t pos[2];
  uint8_t from[2];
  int32_t cost;
} corridor_path_t;

typedef struct non_tunneling_distance {
  heap_node_t *hn;
  uint8_t pos[2];
  uint8_t from[2];
  int32_t cost;
} non_tunneling_distance_t;

typedef struct tunneling_distance {
  heap_node_t *hn;
  uint8_t pos[2];
  uint8_t from[2];
  int32_t cost;
} tunneling_distance_t;

static uint32_t in_room(dungeon_t *d, int16_t y, int16_t x)
{
  int i;

  for (i = 0; i < d->num_rooms; i++) {
    if ((x >= d->rooms[i].position[dim_x]) &&
        (x < (d->rooms[i].position[dim_x] + d->rooms[i].size[dim_x])) &&
        (y >= d->rooms[i].position[dim_y]) &&
        (y < (d->rooms[i].position[dim_y] + d->rooms[i].size[dim_y]))) {
      return 1;
    }
  }

  return 0;
}

static int32_t corridor_path_cmp(const void *key, const void *with) {
  return ((corridor_path_t *) key)->cost - ((corridor_path_t *) with)->cost;
}

static void dijkstra_corridor(dungeon_t *d, pair_t from, pair_t to)
{
  static corridor_path_t path[DUNGEON_Y][DUNGEON_X], *p;
  static uint32_t initialized = 0;
  heap_t h;
  uint32_t x, y;

  if (!initialized) {
    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        path[y][x].pos[dim_y] = y;
        path[y][x].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      path[y][x].cost = INT_MAX;
    }
  }

  path[from[dim_y]][from[dim_x]].cost = 0;

  heap_init(&h, corridor_path_cmp, NULL);

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (mapxy(x, y) != ter_wall_immutable) {
        path[y][x].hn = heap_insert(&h, &path[y][x]);
      } else {
        path[y][x].hn = NULL;
      }
    }
  }

  while ((p = heap_remove_min(&h))) {
    p->hn = NULL;

    if ((p->pos[dim_y] == to[dim_y]) && p->pos[dim_x] == to[dim_x]) {
      for (x = to[dim_x], y = to[dim_y];
           (x != from[dim_x]) || (y != from[dim_y]);
           p = &path[y][x], x = p->from[dim_x], y = p->from[dim_y]) {
        if (mapxy(x, y) != ter_floor_room) {
          mapxy(x, y) = ter_floor_hall;
          hardnessxy(x, y) = 0;
        }
      }
      heap_delete(&h);
      return;
    }

    if ((path[p->pos[dim_y] - 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost >
         p->cost + hardnesspair(p->pos))) {
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost =
        p->cost + hardnesspair(p->pos);
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
                                           [p->pos[dim_x]    ].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost >
         p->cost + hardnesspair(p->pos))) {
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost =
        p->cost + hardnesspair(p->pos);
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost >
         p->cost + hardnesspair(p->pos))) {
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost =
        p->cost + hardnesspair(p->pos);
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] + 1].hn);
    }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost >
         p->cost + hardnesspair(p->pos))) {
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost =
        p->cost + hardnesspair(p->pos);
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
                                           [p->pos[dim_x]    ].hn);
    }
  }
}

/* This is a cut-and-paste of the above.  The code is modified to  *
 * calculate paths based on inverse hardnesses so that we get a    *
 * high probability of creating at least one cycle in the dungeon. */
static void dijkstra_corridor_inv(dungeon_t *d, pair_t from, pair_t to)
{
  static corridor_path_t path[DUNGEON_Y][DUNGEON_X], *p;
  static uint32_t initialized = 0;
  heap_t h;
  uint32_t x, y;

  if (!initialized) {
    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        path[y][x].pos[dim_y] = y;
        path[y][x].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      path[y][x].cost = INT_MAX;
    }
  }

  path[from[dim_y]][from[dim_x]].cost = 0;

  heap_init(&h, corridor_path_cmp, NULL);

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (mapxy(x, y) != ter_wall_immutable) {
        path[y][x].hn = heap_insert(&h, &path[y][x]);
      } else {
        path[y][x].hn = NULL;
      }
    }
  }

  while ((p = heap_remove_min(&h))) {
    p->hn = NULL;

    if ((p->pos[dim_y] == to[dim_y]) && p->pos[dim_x] == to[dim_x]) {
      for (x = to[dim_x], y = to[dim_y];
           (x != from[dim_x]) || (y != from[dim_y]);
           p = &path[y][x], x = p->from[dim_x], y = p->from[dim_y]) {
        if (mapxy(x, y) != ter_floor_room) {
          mapxy(x, y) = ter_floor_hall;
          hardnessxy(x, y) = 0;
        }
      }
      heap_delete(&h);
      return;
    }

#define hardnesspair_inv(p) (in_room(d, p[dim_y], p[dim_x]) ? \
                             224                            : \
                             (255 - hardnesspair(p)))

    if ((path[p->pos[dim_y] - 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost >
         p->cost + hardnesspair_inv(p->pos))) {
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost =
        p->cost + hardnesspair_inv(p->pos);
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
                                           [p->pos[dim_x]    ].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost >
         p->cost + hardnesspair_inv(p->pos))) {
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost =
        p->cost + hardnesspair_inv(p->pos);
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost >
         p->cost + hardnesspair_inv(p->pos))) {
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost =
        p->cost + hardnesspair_inv(p->pos);
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] + 1].hn);
    }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost >
         p->cost + hardnesspair_inv(p->pos))) {
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost =
        p->cost + hardnesspair_inv(p->pos);
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
                                           [p->pos[dim_x]    ].hn);
    }
  }
}

/* Chooses a random point inside each room and connects them with a *
 * corridor.  Random internal points prevent corridors from exiting *
 * rooms in predictable locations.                                  */
static int connect_two_rooms(dungeon_t *d, room_t *r1, room_t *r2)
{
  pair_t e1, e2;

  e1[dim_y] = rand_range(r1->position[dim_y],
                         r1->position[dim_y] + r1->size[dim_y] - 1);
  e1[dim_x] = rand_range(r1->position[dim_x],
                         r1->position[dim_x] + r1->size[dim_x] - 1);
  e2[dim_y] = rand_range(r2->position[dim_y],
                         r2->position[dim_y] + r2->size[dim_y] - 1);
  e2[dim_x] = rand_range(r2->position[dim_x],
                         r2->position[dim_x] + r2->size[dim_x] - 1);

  /*  return connect_two_points_recursive(d, e1, e2);*/
  dijkstra_corridor(d, e1, e2);

  return 0;
}

static int create_cycle(dungeon_t *d)
{
  /* Find the (approximately) farthest two rooms, then connect *
   * them by the shortest path using inverted hardnesses.      */

  int32_t max, tmp, i, j, p, q;
  pair_t e1, e2;

  for (i = max = 0; i < d->num_rooms - 1; i++) {
    for (j = i + 1; j < d->num_rooms; j++) {
      tmp = (((d->rooms[i].position[dim_x] - d->rooms[j].position[dim_x])  *
              (d->rooms[i].position[dim_x] - d->rooms[j].position[dim_x])) +
             ((d->rooms[i].position[dim_y] - d->rooms[j].position[dim_y])  *
              (d->rooms[i].position[dim_y] - d->rooms[j].position[dim_y])));
      if (tmp > max) {
        max = tmp;
        p = i;
        q = j;
      }
    }
  }

  /* Can't simply call connect_two_rooms() because it doesn't *
   * use inverse hardnesses, so duplicate it here.            */
  e1[dim_y] = rand_range(d->rooms[p].position[dim_y],
                         (d->rooms[p].position[dim_y] +
                          d->rooms[p].size[dim_y] - 1));
  e1[dim_x] = rand_range(d->rooms[p].position[dim_x],
                         (d->rooms[p].position[dim_x] +
                          d->rooms[p].size[dim_x] - 1));
  e2[dim_y] = rand_range(d->rooms[q].position[dim_y],
                         (d->rooms[q].position[dim_y] +
                          d->rooms[q].size[dim_y] - 1));
  e2[dim_x] = rand_range(d->rooms[q].position[dim_x],
                         (d->rooms[q].position[dim_x] +
                          d->rooms[q].size[dim_x] - 1));

  dijkstra_corridor_inv(d, e1, e2);

  return 0;
}

static int connect_rooms(dungeon_t *d)
{
  uint32_t i;

  for (i = 1; i < d->num_rooms; i++) {
    connect_two_rooms(d, d->rooms + i - 1, d->rooms + i);
  }

  create_cycle(d);

  return 0;
}

int gaussian[5][5] = {
  {  1,  4,  7,  4,  1 },
  {  4, 16, 26, 16,  4 },
  {  7, 26, 41, 26,  7 },
  {  4, 16, 26, 16,  4 },
  {  1,  4,  7,  4,  1 }
};

typedef struct queue_node {
  int x, y;
  struct queue_node *next;
} queue_node_t;

static int smooth_hardness(dungeon_t *d)
{
  int32_t i, x, y;
  int32_t s, t, p, q;
  queue_node_t *head, *tail, *tmp;
  uint8_t hardness[DUNGEON_Y][DUNGEON_X];
#ifdef DUMP_HARDNESS_IMAGES
  FILE *out;
#endif
  memset(&hardness, 0, sizeof (hardness));

  /* Seed with some values */
  for (i = 1; i < 255; i += 5) {
    do {
      x = rand() % DUNGEON_X;
      y = rand() % DUNGEON_Y;
    } while (hardness[y][x]);
    hardness[y][x] = i;
    if (i == 1) {
      head = tail = malloc(sizeof (*tail));
    } else {
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
    }
    tail->next = NULL;
    tail->x = x;
    tail->y = y;
  }

#ifdef DUMP_HARDNESS_IMAGES
  out = fopen("seeded.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", DUNGEON_X, DUNGEON_Y);
  fwrite(&hardness, sizeof (hardness), 1, out);
  fclose(out);
#endif

  /* Diffuse the vaules to fill the space */
  while (head) {
    x = head->x;
    y = head->y;
    i = hardness[y][x];

    if (x - 1 >= 0 && y - 1 >= 0 && !hardness[y - 1][x - 1]) {
      hardness[y - 1][x - 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y - 1;
    }
    if (x - 1 >= 0 && !hardness[y][x - 1]) {
      hardness[y][x - 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y;
    }
    if (x - 1 >= 0 && y + 1 < DUNGEON_Y && !hardness[y + 1][x - 1]) {
      hardness[y + 1][x - 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y + 1;
    }
    if (y - 1 >= 0 && !hardness[y - 1][x]) {
      hardness[y - 1][x] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x;
      tail->y = y - 1;
    }
    if (y + 1 < DUNGEON_Y && !hardness[y + 1][x]) {
      hardness[y + 1][x] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x;
      tail->y = y + 1;
    }
    if (x + 1 < DUNGEON_X && y - 1 >= 0 && !hardness[y - 1][x + 1]) {
      hardness[y - 1][x + 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y - 1;
    }
    if (x + 1 < DUNGEON_X && !hardness[y][x + 1]) {
      hardness[y][x + 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y;
    }
    if (x + 1 < DUNGEON_X && y + 1 < DUNGEON_Y && !hardness[y + 1][x + 1]) {
      hardness[y + 1][x + 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y + 1;
    }

    tmp = head;
    head = head->next;
    free(tmp);
  }

  /* And smooth it a bit with a gaussian convolution */
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      for (s = t = p = 0; p < 5; p++) {
        for (q = 0; q < 5; q++) {
          if (y + (p - 2) >= 0 && y + (p - 2) < DUNGEON_Y &&
              x + (q - 2) >= 0 && x + (q - 2) < DUNGEON_X) {
            s += gaussian[p][q];
            t += hardness[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
          }
        }
      }
      d->hardness[y][x] = t / s;
    }
  }
  /* Let's do it again, until it's smooth like Kenny G. */
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      for (s = t = p = 0; p < 5; p++) {
        for (q = 0; q < 5; q++) {
          if (y + (p - 2) >= 0 && y + (p - 2) < DUNGEON_Y &&
              x + (q - 2) >= 0 && x + (q - 2) < DUNGEON_X) {
            s += gaussian[p][q];
            t += hardness[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
          }
        }
      }
      d->hardness[y][x] = t / s;
    }
  }

#ifdef DUMP_HARDNESS_IMAGES
  out = fopen("diffused.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", DUNGEON_X, DUNGEON_Y);
  fwrite(&hardness, sizeof (hardness), 1, out);
  fclose(out);

  out = fopen("smoothed.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", DUNGEON_X, DUNGEON_Y);
  fwrite(&d->hardness, sizeof (d->hardness), 1, out);
  fclose(out);
#endif

  return 0;
}

static int empty_dungeon(dungeon_t *d)
{
  uint8_t x, y;

  smooth_hardness(d);
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      mapxy(x, y) = ter_wall;
      if (y == 0 || y == DUNGEON_Y - 1 ||
          x == 0 || x == DUNGEON_X - 1) {
        mapxy(x, y) = ter_wall_immutable;
        hardnessxy(x, y) = 255;
      }
    }
  }

  return 0;
}

static int place_rooms(dungeon_t *d)
{
  pair_t p;
  uint32_t i;
  int success;
  room_t *r;
  uint8_t hardness[DUNGEON_Y][DUNGEON_X];
  uint32_t x, y;
  struct timeval tv, start;

  /* Placing rooms is 2D bin packing.  Bin packing (2D or otherwise) is NP-  *
   * Complete, meaning (among other things) that there is no known algorithm *
   * to solve the problem in less than exponential time.  There are          *
   * hacks and approximation algorithms that can function more efficiently,  *
   * but we're going to forgoe those in favor of using a timeout.  If we     *
   * can't place all of our rooms in 1 second (and some change), we'll abort *
   * this attempt and start over.                                            */
  gettimeofday(&start, NULL);

  memcpy(&hardness, &d->hardness, sizeof (hardness));

  for (success = 0; !success; ) {
    success = 1;
    for (i = 0; success && i < d->num_rooms; i++) {
      r = d->rooms + i;
      r->position[dim_x] = 1 + rand() % (DUNGEON_X - 2 - r->size[dim_x]);
      r->position[dim_y] = 1 + rand() % (DUNGEON_Y - 2 - r->size[dim_y]);
      for (p[dim_y] = r->position[dim_y] - 1;
           success && p[dim_y] < r->position[dim_y] + r->size[dim_y] + 1;
           p[dim_y]++) {
        for (p[dim_x] = r->position[dim_x] - 1;
             success && p[dim_x] < r->position[dim_x] + r->size[dim_x] + 1;
             p[dim_x]++) {
          if (mappair(p) >= ter_floor) {
            gettimeofday(&tv, NULL);
            if ((tv.tv_sec - start.tv_sec) > 1) {
              memcpy(&d->hardness, &hardness, sizeof (hardness));
              return 1;
            }
            success = 0;
            /* empty_dungeon() regenerates the hardness map, which   *
             * is prohibitively expensive to do in a loop like this, *
             * so instead, we'll use a copy.                         */
            memcpy(&d->hardness, &hardness, sizeof (hardness));
            for (y = 1; y < DUNGEON_Y - 1; y++) {
              for (x = 1; x < DUNGEON_X - 1; x++) {
                mapxy(x, y) = ter_wall;
              }
            }
          } else if ((p[dim_y] != r->position[dim_y] - 1)              &&
                     (p[dim_y] != r->position[dim_y] + r->size[dim_y]) &&
                     (p[dim_x] != r->position[dim_x] - 1)              &&
                     (p[dim_x] != r->position[dim_x] + r->size[dim_x])) {
            mappair(p) = ter_floor_room;
            hardnesspair(p) = 0;
          }
        }
      }
    }
  }

  return 0;
}

static int make_rooms(dungeon_t *d)
{
  uint32_t i;

  for (i = MIN_ROOMS; i < MAX_ROOMS && rand_under(6, 8); i++)
    ;
  d->num_rooms = i;
  d->rooms = malloc(sizeof (*d->rooms) * d->num_rooms);

  for (i = 0; i < d->num_rooms; i++) {
    d->rooms[i].size[dim_x] = ROOM_MIN_X;
    d->rooms[i].size[dim_y] = ROOM_MIN_Y;
    while (rand_under(3, 4) && d->rooms[i].size[dim_x] < ROOM_MAX_X) {
      d->rooms[i].size[dim_x]++;
    }
    while (rand_under(3, 4) && d->rooms[i].size[dim_y] < ROOM_MAX_Y) {
      d->rooms[i].size[dim_y]++;
    }
  }

  return 0;
}

int gen_dungeon(dungeon_t *d)
{
  do {
    make_rooms(d);
  } while (place_rooms(d));
  connect_rooms(d);

  return 0;
}

void render_dungeon(dungeon_t *d)
{
  pair_t p;
  int i;
  for (p[dim_y] = 0; p[dim_y] < DUNGEON_Y; p[dim_y]++) {
    for (p[dim_x] = 0; p[dim_x] < DUNGEON_X; p[dim_x]++) {
      for(i = 0; i < d->nummon; ++i){
        if(p[dim_x] == d->monsters[i].position[dim_x] &&
           p[dim_y] == d->monsters[i].position[dim_y] &&
           d->monsters[i].health == 1){
             putchar(d->monsters[i].characteristics);
           }
      }
      if (p[dim_x] ==  d->pc.position[dim_x] &&
          p[dim_y] ==  d->pc.position[dim_y] && d->pc.health == 1) {
        putchar('@');
      } else {
        switch (mappair(p)) {
        case ter_wall:
        case ter_wall_immutable:
          putchar(' ');
          break;
        case ter_floor:
        case ter_floor_room:
          putchar('.');
          break;
        case ter_floor_hall:
          putchar('#');
          break;
        case ter_debug:
          putchar('*');
          fprintf(stderr, "Debug character at %d, %d\n", p[dim_y], p[dim_x]);
          break;
        case ter_monster:
        break;
        }
      }
    }
    putchar('\n');
  }
}

void delete_dungeon(dungeon_t *d)
{
  free(d->rooms);
}

void init_dungeon(dungeon_t *d)
{
  empty_dungeon(d);
}

int write_dungeon_map(dungeon_t *d, FILE *f)
{
  uint32_t x, y;

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      fwrite(&d->hardness[y][x], sizeof (unsigned char), 1, f);
    }
  }

  return 0;
}

int write_rooms(dungeon_t *d, FILE *f)
{
  uint32_t i;
  uint8_t p;

  for (i = 0; i < d->num_rooms; i++) {
    /* write order is xpos, ypos, width, height */
    p = d->rooms[i].position[dim_x];
    fwrite(&p, 1, 1, f);
    p = d->rooms[i].position[dim_y];
    fwrite(&p, 1, 1, f);
    p = d->rooms[i].size[dim_x];
    fwrite(&p, 1, 1, f);
    p = d->rooms[i].size[dim_y];
    fwrite(&p, 1, 1, f);
  }

  return 0;
}

uint32_t calculate_dungeon_size(dungeon_t *d)
{
  return (20 /* The semantic, version, and size */     +
          (DUNGEON_X * DUNGEON_Y) /* The hardnesses */ +
          (d->num_rooms * 4) /* Four bytes per room */);
}

int write_dungeon(dungeon_t *d, char *file)
{
  char *home;
  char *filename;
  FILE *f;
  size_t len;
  uint32_t be32;

  if (!file) {
    if (!(home = getenv("HOME"))) {
      fprintf(stderr, "\"HOME\" is undefined.  Using working directory.\n");
      home = ".";
    }

    len = (strlen(home) + strlen(SAVE_DIR) + strlen(DUNGEON_SAVE_FILE) +
           1 /* The NULL terminator */                                 +
           2 /* The slashes */);

    filename = malloc(len * sizeof (*filename));
    sprintf(filename, "%s/%s/", home, SAVE_DIR);
    makedirectory(filename);
    strcat(filename, DUNGEON_SAVE_FILE);

    if (!(f = fopen(filename, "w"))) {
      perror(filename);
      free(filename);

      return 1;
    }
    free(filename);
  } else {
    if (!(f = fopen(file, "w"))) {
      perror(file);
      exit(-1);
    }
  }

  /* The semantic, which is 12 bytes, 0-11 */
  fwrite(DUNGEON_SAVE_SEMANTIC, 1, strlen(DUNGEON_SAVE_SEMANTIC), f);

  // /* The version, 4 bytes, 12-15 */
  // be32 = htobe32(DUNGEON_SAVE_VERSION);
  // fwrite(&be32, sizeof (be32), 1, f);
  //
  // /* The size of the file, 4 bytes, 16-19 */
  // be32 = htobe32(calculate_dungeon_size(d));
  // fwrite(&be32, sizeof (be32), 1, f);

  /* The dungeon map, 16800 bytes, 20-16819 */
  write_dungeon_map(d, f);

  /* And the rooms, num_rooms * 4 bytes, 16820-end */
  write_rooms(d, f);

  fclose(f);

  return 0;
}

int read_dungeon_map(dungeon_t *d, FILE *f)
{
  uint32_t x, y;

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      fread(&d->hardness[y][x], sizeof (d->hardness[y][x]), 1, f);
      if (d->hardness[y][x] == 0) {
        /* Mark it as a corridor.  We can't recognize room cells until *
         * after we've read the room array, which we haven't done yet. */
        d->map[y][x] = ter_floor_hall;
      } else if (d->hardness[y][x] == 255) {
        d->map[y][x] = ter_wall_immutable;
      } else {
        d->map[y][x] = ter_wall;
      }
    }
  }


  return 0;
}

int read_rooms(dungeon_t *d, FILE *f)
{
  uint32_t i;
  uint32_t x, y;
  uint8_t p;

  for (i = 0; i < d->num_rooms; i++) {
    fread(&p, 1, 1, f);
    d->rooms[i].position[dim_x] = p;
    fread(&p, 1, 1, f);
    d->rooms[i].position[dim_y] = p;
    fread(&p, 1, 1, f);
    d->rooms[i].size[dim_x] = p;
    fread(&p, 1, 1, f);
    d->rooms[i].size[dim_y] = p;
    /* After reading each room, we need to reconstruct them in the dungeon. */
    for (y = d->rooms[i].position[dim_y];
         y < d->rooms[i].position[dim_y] + d->rooms[i].size[dim_y];
         y++) {
      for (x = d->rooms[i].position[dim_x];
           x < d->rooms[i].position[dim_x] + d->rooms[i].size[dim_x];
           x++) {
        mapxy(x, y) = ter_floor_room;
      }
    }
  }

  return 0;
}

int calculate_num_rooms(uint32_t dungeon_bytes)
{
  return ((dungeon_bytes -
          (20 /* The semantic, version, and size */       +
           (DUNGEON_X * DUNGEON_Y) /* The hardnesses */)) /
          4 /* Four bytes per room */);
}

int read_dungeon(dungeon_t *d, char *file)
{
  char semantic[13];
  uint32_t be32;
  FILE *f;
  char *home;
  size_t len;
  char *filename;
  struct stat buf;

  if (!file) {
    if (!(home = getenv("HOME"))) {
      fprintf(stderr, "\"HOME\" is undefined.  Using working directory.\n");
      home = ".";
    }

    len = (strlen(home) + strlen(SAVE_DIR) + strlen(DUNGEON_SAVE_FILE) +
           1 /* The NULL terminator */                                 +
           2 /* The slashes */);

    filename = malloc(len * sizeof (*filename));
    sprintf(filename, "%s/%s/%s", home, SAVE_DIR, DUNGEON_SAVE_FILE);

    if (!(f = fopen(filename, "r"))) {
      perror(filename);
      free(filename);
      exit(-1);
    }

    if (stat(filename, &buf)) {
      perror(filename);
      exit(-1);
    }

    free(filename);
  } else {
    if (!(f = fopen(file, "r"))) {
      perror(file);
      exit(-1);
    }
    if (stat(file, &buf)) {
      perror(file);
      exit(-1);
    }
  }

  d->num_rooms = 0;

  fread(semantic, sizeof(semantic) - 1, 1, f);
  semantic[12] = '\0';
  // if (strncmp(semantic, DUNGEON_SAVE_SEMANTIC, 12)) {
  //   fprintf(stderr, "Not an RLG327 save file.\n");
  //   exit(-1);
  // }
  // fread(&be32, sizeof (be32), 1, f);
  // if (be32toh(be32) != 0) { /* Since we expect zero, be32toh() is a no-op. */
  //   fprintf(stderr, "File version mismatch.\n");
  //   exit(-1);
  // }
  // fread(&be32, sizeof (be32), 1, f);
  // if (buf.st_size != be32toh(be32)) {
  //   fprintf(stderr, "File size mismatch.\n");
  //   exit(-1);
  // }
  read_dungeon_map(d, f);
  d->num_rooms = calculate_num_rooms(buf.st_size);
  d->rooms = malloc(sizeof (*d->rooms) * d->num_rooms);
  read_rooms(d, f);

  fclose(f);

  return 0;
}

int read_pgm(dungeon_t *d, char *pgm)
{
  FILE *f;
  char s[80];
  uint8_t gm[103][158];
  uint32_t x, y;
  uint32_t i;

  if (!(f = fopen(pgm, "r"))) {
    perror(pgm);
    exit(-1);
  }

  if (!fgets(s, 80, f) || strncmp(s, "P5", 2)) {
    fprintf(stderr, "Expected P5\n");
    exit(-1);
  }
  if (!fgets(s, 80, f) || s[0] != '#') {
    fprintf(stderr, "Expected comment\n");
    exit(-1);
  }
  if (!fgets(s, 80, f) || strncmp(s, "158 103", 5)) {
    fprintf(stderr, "Expected 158 103\n");
    exit(-1);
  }
  if (!fgets(s, 80, f) || strncmp(s, "255", 2)) {
    fprintf(stderr, "Expected 255\n");
    exit(-1);
  }

  fread(gm, 1, 158 * 103, f);

  fclose(f);

  /* In our gray map, treat black (0) as corridor, white (255) as room, *
   * all other values as a hardness.  For simplicity, treat every white *
   * cell as its own room, so we have to count white after reading the  *
   * image in order to allocate the room array.                         */
  for (d->num_rooms = 0, y = 0; y < 103; y++) {
    for (x = 0; x < 158; x++) {
      if (!gm[y][x]) {
        d->num_rooms++;
      }
    }
  }
  d->rooms = malloc(sizeof (*d->rooms) * d->num_rooms);

  for (i = 0, y = 0; y < 103; y++) {
    for (x = 0; x < 158; x++) {
      if (!gm[y][x]) {
        d->rooms[i].position[dim_x] = x + 1;
        d->rooms[i].position[dim_y] = y + 1;
        d->rooms[i].size[dim_x] = 1;
        d->rooms[i].size[dim_y] = 1;
        i++;
        d->map[y + 1][x + 1] = ter_floor_room;
        d->hardness[y + 1][x + 1] = 0;
      } else if (gm[y][x] == 255) {
        d->map[y + 1][x + 1] = ter_floor_hall;
        d->hardness[y + 1][x + 1] = 0;
      } else {
        d->map[y + 1][x + 1] = ter_wall;
        d->hardness[y + 1][x + 1] = gm[y][x];
      }
    }
  }

  for (x = 0; x < 160; x++) {
    d->map[0][x] = ter_wall_immutable;
    d->hardness[0][x] = 255;
    d->map[104][x] = ter_wall_immutable;
    d->hardness[104][x] = 255;
  }
  for (y = 1; y < 104; y++) {
    d->map[y][0] = ter_wall_immutable;
    d->hardness[y][0] = 255;
    d->map[y][159] = ter_wall_immutable;
    d->hardness[y][159] = 255;
  }

  return 0;
}

int move_cmp(const void *key, const void *with) {
  if(((character_t *) key)->turn == ((character_t *) with)->turn){
    if(((character_t *) key)->seq > ((character_t *) with)->seq){
      return 1;
    }
    else{
      return -1;
    }
  }
  return ((character_t *) key)->turn - ((character_t *) with)->turn;
}

void excute(dungeon_t *d, int nummon)
{
  character_t *current_move;
  d->monsters = malloc(sizeof(character_t) * nummon);
  heap_t event_queue;
  heap_init(&event_queue, move_cmp, NULL);
  int target = rand_range(0, d->nummon-1);
  d->pc.last_position[dim_x] = d->pc.position[dim_x];
  d->pc.last_position[dim_y] = d->pc.position[dim_y];
  d->pc.is_pc = 1;
  d->pc.speed = 10;
  d->pc.seq = 0;
  d->pc.turn = 0;
  d->pc.health = 1;
  d->pc.characteristics = '@';
  d->pc.target = target;

  heap_insert(&event_queue, &d->pc);

  int i;
  for(i = 0; i < nummon; i++){
    //random the position for the monsters
    int a = rand() % d->num_rooms;
    d->monsters[i].room = a;
    d->monsters[i].position[dim_x] = (d->rooms[a].position[dim_x] +
                            (rand() % d->rooms[a].size[dim_x]));
    d->monsters[i].position[dim_y] = (d->rooms[a].position[dim_y] +
                            (rand() % d->rooms[a].size[dim_y]));
    while(mappair(d->monsters[i].position) == ter_monster &&
          d->monsters[i].room == d->pc.room){
      int a = rand() % d->num_rooms;
      d->monsters[i].room = a;
      d->monsters[i].position[dim_x] = (d->rooms[a].position[dim_x] +
                              (rand() % d->rooms[a].size[dim_x]));
      d->monsters[i].position[dim_y] = (d->rooms[a].position[dim_y] +
                              (rand() % d->rooms[a].size[dim_y]));
    }
    mapxy(d->monsters[i].position[dim_x], d->monsters[i].position[dim_y]) = ter_monster;
    d->monsters[i].is_pc = 0;
    d->monsters[i].speed = rand_range(5,20);
    d->monsters[i].seq = ++seqs;
    d->monsters[i].turn = 0;
    d->monsters[i].health = 1;
    d->monsters[i].is_intelligence = rand_range(0,1);
    d->monsters[i].is_telepathy = rand_range(0,1);
    d->monsters[i].is_tunneling = rand_range(0,1);
    d->monsters[i].is_erratic = rand_range(0,1);
    char *characteristics = malloc(5);

    int length = snprintf( NULL, 0, "%d", d->monsters[i].is_intelligence );
    char* is_intelligence = malloc( length + 1 );
    snprintf( is_intelligence, length + 1, "%d", d->monsters[i].is_intelligence );

    char* is_telepathy = malloc( length + 1 );
    snprintf( is_telepathy, length + 1, "%d", d->monsters[i].is_telepathy );

    char* is_tunneling = malloc( length + 1 );
    snprintf( is_tunneling, length + 1, "%d", d->monsters[i].is_tunneling );

    char* is_erratic = malloc( length + 1 );
    snprintf( is_erratic, length + 1, "%d", d->monsters[i].is_erratic );

    strcat(characteristics, is_erratic);
    strcat(characteristics, is_tunneling);
    strcat(characteristics, is_telepathy);
    strcat(characteristics, is_intelligence);
    free(is_intelligence);
    free(is_telepathy);
    free(is_tunneling);
    free(is_erratic);

    if(strcmp(characteristics, "0000") == 0){
      d->monsters[i].characteristics = '0';
    }
    else if(strcmp(characteristics, "0001") == 0){
      d->monsters[i].characteristics = '1';
    }
    else if(strcmp(characteristics, "0010") == 0){
      d->monsters[i].characteristics = '2';
    }
    else if(strcmp(characteristics, "0011") == 0){
      d->monsters[i].characteristics = '3';
    }
    else if(strcmp(characteristics, "0100") == 0){
      d->monsters[i].characteristics = '4';
    }
    else if(strcmp(characteristics, "0101") == 0){
      d->monsters[i].characteristics = '5';
    }
    else if(strcmp(characteristics, "0110") == 0){
      d->monsters[i].characteristics = '6';
    }
    else if(strcmp(characteristics, "0111") == 0){
      d->monsters[i].characteristics = '7';
    }
    else if(strcmp(characteristics, "1000") == 0){
      d->monsters[i].characteristics = '8';
    }
    else if(strcmp(characteristics, "1001") == 0){
      d->monsters[i].characteristics = '9';
    }
    else if(strcmp(characteristics, "1010") == 0){
      d->monsters[i].characteristics = 'a';
    }
    else if(strcmp(characteristics, "1011") == 0){
      d->monsters[i].characteristics = 'b';
    }
    else if(strcmp(characteristics, "1100") == 0){
      d->monsters[i].characteristics = 'c';
    }
    else if(strcmp(characteristics, "1101") == 0){
      d->monsters[i].characteristics = 'd';
    }
    else if(strcmp(characteristics, "1110") == 0){
      d->monsters[i].characteristics = 'e';
    }
    else if(strcmp(characteristics, "1111") == 0){
      d->monsters[i].characteristics = 'f';
    }
    free(characteristics);
    heap_insert(&event_queue, &d->monsters[i]);
  }
  render_dungeon(d);

  while(d->pc.health == 1 && check_monsters_alive(d) == 1 && event_queue.size > 0){
    current_move = heap_remove_min(&event_queue);
    if(current_move->health == 0) continue;
    usleep(8333);
    if(current_move->is_pc == 1){
      pc_move(d);
      render_dungeon(d);
    }
    else{
      monster_move(d, current_move);
    }

    current_move->turn += 1000/(current_move->speed);

    heap_insert(&event_queue, current_move);

  }
  if(d->pc.health == 0){
    printf("You lose!\n");
  }
  if(check_monsters_alive(d) == 0){
    printf("You win!\n");
  }
  free(d->monsters);
}

int check_monsters_alive(dungeon_t *d)
{
  int i;
  for(i = 0; i < d->nummon; i++){
    if(d->monsters[i].health == 1) return 1;
  }
  return 0;
}

void pc_move(dungeon_t *d)
{
  if (d->pc.health == 0) return;
  pair_t next_position;
  next_position[dim_x] = d->pc.position[dim_x];
  next_position[dim_y] = d->pc.position[dim_y];
  if(d->monsters[d->pc.target].health == 0) d->pc.target = rand_range(0, d->nummon-1);
  pair_t to;
  to[dim_x] = d->monsters[d->pc.target].position[dim_x];
  to[dim_y] = d->monsters[d->pc.target].position[dim_y];
  smart_move(d, &d->pc, to, next_position);
  if(mappair(next_position) == ter_monster){
    int i;
    for(i = 0; i < d->nummon; ++i){
      if(d->monsters[i].position[dim_x] == next_position[dim_x] &&
         d->monsters[i].position[dim_y] == next_position[dim_y] &&
         d->monsters[i].health == 1){
           d->monsters[i].health = 0;
           mappair(d->monsters[i].position) = ter_floor;

         }
    }
    d->pc.last_position[dim_y] = d->pc.position[dim_y];
    d->pc.last_position[dim_x] = d->pc.position[dim_x];
    d->pc.position[dim_x] = next_position[dim_x];
    d->pc.position[dim_y] = next_position[dim_y];
  }
  d->pc.last_position[dim_y] = d->pc.position[dim_y];
  d->pc.last_position[dim_x] = d->pc.position[dim_x];

  d->pc.position[dim_x] = next_position[dim_x];
  d->pc.position[dim_y] = next_position[dim_y];
}

int line_of_sight(character_t *pc, character_t *monster)
{
  if(pc->position[dim_y] == monster->position[dim_y]){
    return 1;
  }
  else if(pc->position[dim_x] == monster->position[dim_x]){
    return 1;
  }
  else if(pc->position[dim_x] - monster->position[dim_x] == pc->position[dim_y] - monster->position[dim_y]){
    return 1;
  }
  else if(monster->position[dim_x] - pc->position[dim_x] == pc->position[dim_y] - monster->position[dim_y]){
    return 1;
  }
  else return -1;
}

void monster_move(dungeon_t *d, character_t *monster)
{
  if(monster->health == 0) return;
  pair_t next_position;
  next_position[dim_x] = monster->position[dim_x];
  next_position[dim_y] = monster->position[dim_y];
  pair_t temp;
  temp[dim_x] = next_position[dim_x];
  temp[dim_y] = next_position[dim_y];
  monster->last_position[dim_x] = 0;
  monster->last_position[dim_y] = 0;
  int ran;
  switch (monster->characteristics) {
    case '0':
      if(line_of_sight(&d->pc, monster) == 1){
        stupid_move(d, monster, d->pc.position, next_position);
      }
      else stupid_move(d, monster, monster->last_position, next_position);
      break;
    case '1':
      if(line_of_sight(&d->pc, monster) == 1){
        smart_move(d, monster, d->pc.position, next_position);
      }
      else smart_move(d, monster, monster->last_position, next_position);
      break;
    case '2':
      stupid_move(d, monster, d->pc.position, next_position);
      break;
    case '3':
      smart_move(d, monster, d->pc.position, next_position);
      break;
    case '4':
      if(line_of_sight(&d->pc, monster) == 1){
        stupid_tunneling_move(d, monster, d->pc.position, next_position);
      }
      else stupid_tunneling_move(d, monster, monster->last_position, next_position);
      break;
    case '5':
      if(line_of_sight(&d->pc, monster) == 1){
        smart_tunneling_move(d, monster, d->pc.position, next_position);
      }
      else smart_tunneling_move(d, monster, monster->last_position, next_position);
      break;
    case '6':
      stupid_tunneling_move(d, monster, d->pc.position, next_position);
      break;
    case '7':
      smart_tunneling_move(d, monster, d->pc.position, next_position);
      break;
    case '8':
      ran = rand_range(0, 1);
      if(ran){
        if(line_of_sight(&d->pc, monster) == 1){
          stupid_move(d, monster, d->pc.position, next_position);
        }
        else stupid_move(d, monster, monster->last_position, next_position);
      }
      else{
        next_position[dim_x]++;
      }
      break;
    case '9':
      ran = rand_range(0, 1);
      if(ran){
        if(line_of_sight(&d->pc, monster) == 1){
          smart_move(d, monster, d->pc.position, next_position);
        }
        else smart_move(d, monster, monster->last_position, next_position);
        break;
      }
      else{
        next_position[dim_x]++;
      }
      break;
    case 'a':
      ran = rand_range(0, 1);
      if(ran){
        stupid_move(d, monster, d->pc.position, next_position);
      }
        else{
          next_position[dim_x]--;
        }
      break;
    case 'b':
      ran = rand_range(0, 1);
      if(ran){
        smart_move(d, monster, d->pc.position, next_position);
      }
        else{
          next_position[dim_x]--;
        }
      break;
    case 'c':
      ran = rand_range(0, 1);
      if(ran){
        if(line_of_sight(&d->pc, monster) == 1){
          stupid_tunneling_move(d, monster, d->pc.position, next_position);
        }
        else stupid_tunneling_move(d, monster, monster->last_position, next_position);
      }
      else{
        next_position[dim_x]--;
        next_position[dim_y]--;
      }
      break;
    case 'd':
      ran = rand_range(0, 1);
      if(ran){
        if(line_of_sight(&d->pc, monster) == 1){
          smart_tunneling_move(d, monster, d->pc.position, next_position);
        }
        else smart_tunneling_move(d, monster, monster->last_position, next_position);
      }
      else{
        next_position[dim_x]--;
        next_position[dim_y]++;
      }
      break;
    case 'e':
      ran = rand_range(0, 1);
      if(ran){
        stupid_tunneling_move(d, monster, d->pc.position, next_position);
      }
      else{
        next_position[dim_x]++;
        next_position[dim_y]++;
      }
      break;
    case 'f':
      ran = rand_range(0, 1);
      if(ran){
        smart_tunneling_move(d, monster, d->pc.position, next_position);
      }
      else{
        next_position[dim_x]++;
        next_position[dim_y]--;
      }
      break;
  }
  if(mappair(next_position) == ter_wall && monster->is_tunneling == 0){
    next_position[dim_x] = temp[dim_x];
    next_position[dim_y] = temp[dim_y];
  }
  if(!(monster->position[dim_x] == next_position[dim_x] && monster->position[dim_y] == next_position[dim_y])){
    int i, j;
    j = 0;
    for(i = 0; i < d->num_rooms; ++i){
      if((monster->position[dim_x] >= d->rooms[i].position[dim_x] &&
          monster->position[dim_x] <= d->rooms[i].position[dim_x] + d->rooms[i].size[dim_x]) &&
          (monster->position[dim_y] >= d->rooms[i].position[dim_y] &&
          monster->position[dim_y] <= d->rooms[i].position[dim_y] + d->rooms[i].size[dim_y])){
           j = 1;
         }
    }
    if(j == 1) mappair(monster->position) = ter_floor_room;
    else mappair(monster->position) = ter_floor_hall;
    monster->position[dim_x] = next_position[dim_x];
    monster->position[dim_y] = next_position[dim_y];
    mappair(next_position) = ter_monster;
  }
  else if(next_position[dim_x] == d->pc.position[dim_x] && next_position[dim_y] == d->pc.position[dim_y]){
    d->pc.health = 0;
    mappair(monster->position) = ter_floor;
    int i, j;
    j = 0;
    for(i = 0; i < d->num_rooms; ++i){
      if((monster->position[dim_x] >= d->rooms[i].position[dim_x] &&
          monster->position[dim_x] <= d->rooms[i].position[dim_x] + d->rooms[i].size[dim_x]) &&
          (monster->position[dim_y] >= d->rooms[i].position[dim_y] &&
          monster->position[dim_y] <= d->rooms[i].position[dim_y] + d->rooms[i].size[dim_y])){
           j = 1;
         }
    }
    if(j == 1) mappair(monster->position) = ter_floor_room;
    else mappair(monster->position) = ter_floor_hall;
    monster->position[dim_x] = next_position[dim_x];
    monster->position[dim_y] = next_position[dim_y];
    mappair(next_position) = ter_monster;
  }
}

void stupid_move(dungeon_t *d, character_t *monster, pair_t next, pair_t next_position)
{
  pair_t temp;
  temp[dim_x] = next_position[dim_x];
  temp[dim_y] = next_position[dim_y];
  if(next[dim_x] == 0 && next[dim_y] == 0){
    return;
  }
  else{
    if (monster->position[dim_x] > next[dim_x]) {
      if (monster->position[dim_y] > next[dim_y]) {
        if (rand_range(0,1))
          next_position[dim_x]--;
        else
          next_position[dim_y]--;
      } else if (monster->position[dim_y] < next[dim_y]) {
        if (rand_range(0,1))
          next_position[dim_x]--;
        else
          next_position[dim_y]++;
      } else {
        next_position[dim_x]--;
      }
    } else {
      if (monster->position[dim_y] > next[dim_y]) {
        if (rand_range(0,1))
          next_position[dim_x]++;
        else
          next_position[dim_y]--;
      } else if (monster->position[dim_y] < next[dim_y]) {
        if (rand_range(0,1))
          next_position[dim_x]++;
        else
          next_position[dim_y]++;
      } else {
        next_position[dim_x]++;
      }
    }
  }
  if(mappair(next_position) == ter_wall || mappair(next_position) == ter_wall_immutable){
    next_position[dim_x] = temp[dim_x];
    next_position[dim_y] = temp[dim_y];
  }
}


static int32_t nontunneling_distance_cmp(const void *key, const void *with) {
  return ((non_tunneling_distance_t *) key)->cost - ((non_tunneling_distance_t *) with)->cost;
}
void smart_move(dungeon_t *d, character_t *monster, pair_t to, pair_t next_position)
{
  if(to[dim_x] == 0 && to[dim_y] == 0){
    return;
  }
  static non_tunneling_distance_t path[DUNGEON_Y][DUNGEON_X], *p;
  static uint32_t initialized = 0;
  heap_t h;
  uint32_t x, y;
  pair_t from;
  from[dim_x] = monster->position[dim_x];
  from[dim_y] = monster->position[dim_y];
  if (!initialized) {
    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        path[y][x].pos[dim_y] = y;
        path[y][x].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      path[y][x].cost = INT_MAX;
    }
  }

  path[from[dim_y]][from[dim_x]].cost = 0;

  heap_init(&h, nontunneling_distance_cmp, NULL);

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (mapxy(x, y) != ter_wall_immutable) {
        path[y][x].hn = heap_insert(&h, &path[y][x]);
      } else {
        path[y][x].hn = NULL;
      }
    }
  }

  while ((p = heap_remove_min(&h))) {
    p->hn = NULL;

    if ((p->pos[dim_y] == to[dim_y]) && p->pos[dim_x] == to[dim_x]) {
      for (x = to[dim_x], y = to[dim_y];
           (x != from[dim_x]) || (y != from[dim_y]);
           p = &path[y][x], x = p->from[dim_x], y = p->from[dim_y]) {
             next_position[dim_x] = x;
             next_position[dim_y] = y;
      }
      heap_delete(&h);
    }

    int hard = hardnesspair(p->pos);
    int weight;
    if(hard == 0) weight = 1;
    else weight = INT_MAX;

    if ((path[p->pos[dim_y] - 1][p->pos[dim_x]].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x]].cost >
            p->cost + weight) && hardnessxy(p->pos[dim_x], p->pos[dim_y] - 1) == 0) {
      path[p->pos[dim_y] - 1][p->pos[dim_x]].cost = p->cost + weight;
      path[p->pos[dim_y] - 1][p->pos[dim_x]].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] - 1][p->pos[dim_x]].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
      [p->pos[dim_x]].hn);
    }

    if ((path[p->pos[dim_y]][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y]][p->pos[dim_x] - 1].cost >
            p->cost + weight) && hardnessxy(p->pos[dim_x] - 1, p->pos[dim_y]) == 0)
                {
      path[p->pos[dim_y]][p->pos[dim_x] - 1].cost =
          p->cost + weight;
      path[p->pos[dim_y]][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]]
      [p->pos[dim_x] - 1].hn);
    }

    if ((path[p->pos[dim_y]][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y]][p->pos[dim_x] + 1].cost >
            p->cost + weight) && hardnessxy(p->pos[dim_x] + 1, p->pos[dim_y]) == 0) {
      path[p->pos[dim_y]][p->pos[dim_x] + 1].cost =
          p->cost + weight;
      path[p->pos[dim_y]][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]]
      [p->pos[dim_x] + 1].hn);
    }

    if ((path[p->pos[dim_y] + 1][p->pos[dim_x]].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x]].cost >
            p->cost + weight) && hardnessxy(p->pos[dim_x], p->pos[dim_y] + 1) == 0) {
      path[p->pos[dim_y] + 1][p->pos[dim_x]].cost =
          p->cost + weight;
      path[p->pos[dim_y] + 1][p->pos[dim_x]].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] + 1][p->pos[dim_x]].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
      [p->pos[dim_x]].hn);
    }

    if ((path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost >
            p->cost + weight) && hardnessxy(p->pos[dim_x] + 1, p->pos[dim_y] + 1) == 0) {
      path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost =
          p->cost + weight;
      path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
      [p->pos[dim_x] + 1].hn);
    }

    if ((path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost >
            p->cost + weight) && hardnessxy(p->pos[dim_x] - 1, p->pos[dim_y] + 1) == 0) {
      path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost =
          p->cost + weight;
      path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
      [p->pos[dim_x] - 1].hn);
    }

    if ((path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost >
            p->cost + weight) && hardnessxy(p->pos[dim_x] + 1, p->pos[dim_y] - 1) == 0) {
      path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost =
          p->cost + weight;
      path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
      [p->pos[dim_x] + 1].hn);
    }

    if ((path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost >
            p->cost + weight) && hardnessxy(p->pos[dim_x] - 1, p->pos[dim_y] - 1) == 0) {
      path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost =
          p->cost + weight;
      path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
      [p->pos[dim_x] - 1].hn);
    }
  }
}

void stupid_tunneling_move(dungeon_t *d, character_t *monster, pair_t next, pair_t next_position)
{
  pair_t temp;
  temp[dim_x] = next_position[dim_x];
  temp[dim_y] = next_position[dim_y];
  if(next[dim_x] == 0 && next[dim_y] == 0){
    return;
  }
  else{
    if (monster->position[dim_x] > next[dim_x]) {
      if (monster->position[dim_y] > next[dim_y]) {
        if (rand_range(0,1))
          next_position[dim_x]--;
        else
          next_position[dim_y]--;
      } else if (monster->position[dim_y] < next[dim_y]) {
        if (rand_range(0,1))
          next_position[dim_x]--;
        else
          next_position[dim_y]++;
      } else {
        next_position[dim_x]--;
      }
    } else {
      if (monster->position[dim_y] > next[dim_y]) {
        if (rand_range(0,1))
          next_position[dim_x]++;
        else
          next_position[dim_y]--;
      } else if (monster->position[dim_y] < next[dim_y]) {
        if (rand_range(0,1))
          next_position[dim_x]++;
        else
          next_position[dim_y]++;
      } else {
        next_position[dim_x]++;
      }
    }
  }
  if(mappair(next_position) == ter_wall_immutable){
    next_position[dim_x] = temp[dim_x];
    next_position[dim_y] = temp[dim_y];
  }
  hardnesspair(next_position) -= 85;
  if(mappair(next_position) == ter_wall && hardnesspair(next_position) <= 0){
    return;
  }
  if(mappair(next_position) == ter_wall && hardnesspair(next_position) > 0){
    next_position[dim_x] = temp[dim_x];
    next_position[dim_y] = temp[dim_y];
  }
}

static int32_t tunneling_distance_cmp(const void *key, const void *with) {
  return ((tunneling_distance_t *) key)->cost - ((tunneling_distance_t *) with)->cost;
}
void smart_tunneling_move(dungeon_t *d, character_t *monster, pair_t to, pair_t next_position)
{
  static tunneling_distance_t path[DUNGEON_Y][DUNGEON_X], *p;
  static uint32_t initialized = 0;
  heap_t h;
  uint32_t x, y;
  pair_t from;
  from[dim_x] = monster->position[dim_x];
  from[dim_y] = monster->position[dim_y];

  if (!initialized) {
    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        path[y][x].pos[dim_y] = y;
        path[y][x].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      path[y][x].cost = INT_MAX;
    }
  }

  path[from[dim_y]][from[dim_x]].cost = 0;

  heap_init(&h, tunneling_distance_cmp, NULL);

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (mapxy(x, y) != ter_wall_immutable) {
        path[y][x].hn = heap_insert(&h, &path[y][x]);
      } else {
        path[y][x].hn = NULL;
      }
    }
  }

  while ((p = heap_remove_min(&h))) {
    p->hn = NULL;

    if ((p->pos[dim_y] == to[dim_y]) && p->pos[dim_x] == to[dim_x]) {
      for (x = to[dim_x], y = to[dim_y];
           (x != from[dim_x]) || (y != from[dim_y]);
           p = &path[y][x], x = p->from[dim_x], y = p->from[dim_y]) {
             next_position[dim_x] = x;
             next_position[dim_y] = y;
      }
      heap_delete(&h);
    }

    int hard = hardnesspair(p->pos);
    int weight;
    if((hard >= 1 && hard <=84) || hard == 0) {
      weight = 1;
    }
    else if( hard >= 85 && hard <=170) {
      weight = 2;
    }
    else if( hard >= 171 && hard <=254) {
      weight = 3;
    }
    else weight = INT_MAX;

    if ((path[p->pos[dim_y] - 1][p->pos[dim_x]].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x]].cost >
            p->cost + weight)) {
      path[p->pos[dim_y] - 1][p->pos[dim_x]].cost = p->cost + weight;
      path[p->pos[dim_y] - 1][p->pos[dim_x]].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] - 1][p->pos[dim_x]].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
      [p->pos[dim_x]].hn);
    }

    if ((path[p->pos[dim_y]][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y]][p->pos[dim_x] - 1].cost >
            p->cost + weight))
                {
      path[p->pos[dim_y]][p->pos[dim_x] - 1].cost =
          p->cost + weight;
      path[p->pos[dim_y]][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]]
      [p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y]][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y]][p->pos[dim_x] + 1].cost >
            p->cost + weight)) {
      path[p->pos[dim_y]][p->pos[dim_x] + 1].cost =
          p->cost + weight;
      path[p->pos[dim_y]][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]]
      [p->pos[dim_x] + 1].hn);
    }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x]].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x]].cost >
            p->cost + weight)) {
      path[p->pos[dim_y] + 1][p->pos[dim_x]].cost =
          p->cost + weight;
      path[p->pos[dim_y] + 1][p->pos[dim_x]].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] + 1][p->pos[dim_x]].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
      [p->pos[dim_x]].hn);
    }

    if ((path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost >
            p->cost + weight)) {
      path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost =
          p->cost + weight;
      path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
      [p->pos[dim_x] + 1].hn);
    }

    if ((path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost >
            p->cost + weight)) {
      path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost =
          p->cost + weight;
      path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
      [p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost >
            p->cost + weight)) {
      path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost =
          p->cost + weight;
      path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
      [p->pos[dim_x] + 1].hn);
    }
    if ((path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost >
            p->cost + weight)) {
      path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost =
          p->cost + weight;
      path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
      [p->pos[dim_x] - 1].hn);
    }
  }
}
