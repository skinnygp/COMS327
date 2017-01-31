#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <endian.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>

/* Adds a "room shrinking" phase if it has trouble placing all the rooms. *
 * This allows rooms to become smaller than are specified in the problem  *
 * statement as a minimum room size, so strictly speaking, this is not    *
 * compliant.                                                             */

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

#define rand_under(numerator, denominator) \
  (rand() < ((RAND_MAX / denominator) * numerator))
#define rand_range(min, max) ((rand() % (((max) + 1) - (min))) + (min))

typedef enum dim {
  dim_x,
  dim_y,
  num_dims
} dim_t;

typedef int16_t pair_t[num_dims];

typedef enum terrain_type {
  ter_debug,
  ter_wall,
  ter_wall_no_room,
  ter_wall_no_floor,
  ter_wall_immutable,
  ter_floor,
  ter_floor_room,
  ter_floor_hall,
  ter_floor_tentative
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
  uint8_t map[DUNGEON_Y][DUNGEON_X];
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

int point_in_room_p(room_t *r, pair_t p)
{
  return ((r->position[dim_x] <= p[dim_x]) &&
          ((r->position[dim_x] + r->size[dim_x]) > p[dim_x]) &&
          (r->position[dim_y] <= p[dim_y]) &&
          ((r->position[dim_y] + r->size[dim_y]) > p[dim_y]));
}

/* Randomly decides whether to choose the x or the y direction first,   *
 * then draws the corridor connecting two points with one or two lines. */
int connect_two_points(dungeon_t *d,
                       pair_t min_x, pair_t max_x,
                       pair_t min_y, pair_t max_y)
{
  uint32_t i;
  uint32_t x_first;

  x_first = rand() & 1;

  for (i = min_x[dim_x]; i <= max_x[dim_x]; i++) {
    if (d->map[x_first ? min_y[dim_y] : max_y[dim_y]][i] == ter_wall) {
      d->map[x_first ? min_y[dim_y] : max_y[dim_y]][i] = ter_floor_hall;
      d->hardness[x_first ? min_y[dim_y] : max_y[dim_y]][i] = 0;
    }
  }
  for (i = min_y[dim_y]; i <= max_y[dim_y]; i++) {
    if (d->map[i][x_first ? max_y[dim_x] : min_y[dim_x]] == ter_wall) {
      d->map[i][x_first ? max_y[dim_x] : min_y[dim_x]] = ter_floor_hall;
      d->hardness[i][x_first ? max_y[dim_x] : min_y[dim_x]] = 0;
    }
  }

  return 0;
}

/* Recursively subdivides the space between two points until some minimum *
 * closeness is reached, then draws the corridors connecting them.        */
int connect_two_points_recursive(dungeon_t *d, pair_t e1, pair_t e2)
{
  pair_t mid;
  int16_t *min_x, *min_y, *max_x, *max_y;

  min_x = ((e1[dim_x] < e2[dim_x]) ? e1 : e2);
  max_x = ((min_x == e1) ? e2 : e1);
  min_y = ((e1[dim_y] < e2[dim_y]) ? e1 : e2);
  max_y = ((min_y == e1) ? e2 : e1);

  if ((max_x[dim_x] - min_x[dim_x]) + 
      (max_y[dim_y] - min_y[dim_y]) < 15
) {
    return connect_two_points(d, min_x, max_x, min_y, max_y);
  }

  mid[dim_x] = rand_range(min_x[dim_x], max_x[dim_x]);
  mid[dim_y] = rand_range(min_y[dim_y], max_y[dim_y]);

  connect_two_points_recursive(d, e1, mid);
  connect_two_points_recursive(d, mid, e2);

  return 0;
}

/* Chooses a random point inside each room and connects them with a *
 * corridor.  Random internal points prevent corridors from exiting *
 * rooms in predictable locations.                                  */
int connect_two_rooms(dungeon_t *d, room_t *r1, room_t *r2)
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

  return connect_two_points_recursive(d, e1, e2);
}

int connect_rooms(dungeon_t *d)
{
  uint32_t i, j;
  uint32_t dist, dist_tmp;
  uint32_t connected;
  uint32_t r1, r2;
  uint32_t min_con, max_con;

  connected = 0;

  /* Find the closest pair and connect them.  Do this until everybody *
   * is connected to room zero.  Because of the nature of the path    *
   * drawing algorithm, it's possible for two paths to cross, or for  *
   * a path to touch another room, but what this produces is *almost* *
   * a free tree.                                                     */
  while (!connected) {
    dist = DUNGEON_X + DUNGEON_Y;
    for (i = 0; i < d->num_rooms; i++) {
      for (j = i + 1; j < d->num_rooms; j++) {
        if (d->rooms[i].connected != d->rooms[j].connected) {
          dist_tmp = (abs(d->rooms[i].position[dim_x] -
                          d->rooms[j].position[dim_x]) +
                      abs(d->rooms[i].position[dim_y] -
                          d->rooms[j].position[dim_y]));
          if (dist_tmp < dist) {
            r1 = i;
            r2 = j;
            dist = dist_tmp;
          }
        }
      }
    }
    connect_two_rooms(d, d->rooms + r1, d->rooms + r2);
    min_con = (d->rooms[r1].connected < d->rooms[r2].connected ?
               d->rooms[r1].connected : d->rooms[r2].connected);
    max_con = (d->rooms[r1].connected > d->rooms[r2].connected ?
               d->rooms[r1].connected : d->rooms[r2].connected);
    for (connected = 1, i = 1; i < d->num_rooms; i++) {
      if (d->rooms[i].connected == max_con) {
        d->rooms[i].connected = min_con;
      }
      if (d->rooms[i].connected) {
        connected = 0;
      }
    }
  }

  /* Now let's add a handful of random, extra connections to create *
   * loops and a more exciting look for the dungeon.  Use a do loop *
   * to guarantee that it always adds at least one such path.       */

  do {
    r1 = rand_range(0, d->num_rooms - 1);
    while ((r2 = rand_range(0, d->num_rooms - 1)) == r1)
      ;
    connect_two_rooms(d, d->rooms + r1, d->rooms + r2);
  } while (rand_under(1, 2));

  return 0;
}

int place_room(dungeon_t *d, room_t *r)
{
  pair_t p;
  uint32_t tries;
  uint32_t qualified;

  for (;;) {
    for (tries = 0; tries < MAX_PLACEMENT_ATTEMPTS; tries++) {
      p[dim_x] = rand() % DUNGEON_X;
      p[dim_y] = rand() % DUNGEON_Y;
      if (p[dim_x] - ROOM_SEPARATION < 0                      ||
          p[dim_x] + r->size[dim_x] + ROOM_SEPARATION > DUNGEON_X ||
          p[dim_y] - ROOM_SEPARATION < 0                      ||
          p[dim_y] + r->size[dim_y] + ROOM_SEPARATION > DUNGEON_Y) {
        continue;
      }
      r->position[dim_x] = p[dim_x];
      r->position[dim_y] = p[dim_y];
      for (qualified = 1, p[dim_y] = r->position[dim_y] - ROOM_SEPARATION;
           p[dim_y] < r->position[dim_y] + r->size[dim_y] + ROOM_SEPARATION;
           p[dim_y]++) {
        for (p[dim_x] = r->position[dim_x] - ROOM_SEPARATION;
             p[dim_x] < r->position[dim_x] + r->size[dim_x] + ROOM_SEPARATION;
             p[dim_x]++) {
          if (d->map[p[dim_y]][p[dim_x]] >= ter_floor) {
            qualified = 0;
          }
        }
      }
    }
    if (!qualified) {
      if (rand_under(1, 2)) {
        if (r->size[dim_x] > 2) {
          r->size[dim_x]--;
        } else if (r->size[dim_y] > 2) {
          r->size[dim_y]--;
        }
      } else {
        if (r->size[dim_y] > 2) {
          r->size[dim_y]--;
        } else if (r->size[dim_x] > 2) {
          r->size[dim_x]--;
        }
      }
    } else {
      for (p[dim_y] = r->position[dim_y];
           p[dim_y] < r->position[dim_y] + r->size[dim_y];
           p[dim_y]++) {
        for (p[dim_x] = r->position[dim_x];
             p[dim_x] < r->position[dim_x] + r->size[dim_x];
             p[dim_x]++) {
          d->map[p[dim_y]][p[dim_x]] = ter_floor_room;
          d->hardness[p[dim_y]][p[dim_x]] = 0;
        }
      }
      for (p[dim_x] = r->position[dim_x] - 1;
           p[dim_x] < r->position[dim_x] + r->size[dim_x] + 1;
           p[dim_x]++) {
        d->map[r->position[dim_y] - 1][p[dim_x]]                =
          d->map[r->position[dim_y] + r->size[dim_y] + 1][p[dim_x]] =
          ter_wall;
      }
      for (p[dim_y] = r->position[dim_y] - 1;
           p[dim_y] < r->position[dim_y] + r->size[dim_y] + 1;
           p[dim_y]++) {
        d->map[p[dim_y]][r->position[dim_x] - 1]                =
          d->map[p[dim_y]][r->position[dim_x] + r->size[dim_x] + 1] =
          ter_wall;
      }

      return 0;
    }
  }

  return 0;
}

int place_rooms(dungeon_t *d)
{
  uint32_t i;

  for (i = 0; i < d->num_rooms; i++) {
    place_room(d, d->rooms + i);
  }

  return 0;
}

int make_rooms(dungeon_t *d)
{
  uint32_t i;

  for (i = 0; i < MIN_ROOMS; i++) {
    d->rooms[i].exists = 1;
  }
  for (; i < MAX_ROOMS && rand_under(6, 8); i++) {
    d->rooms[i].exists = 1;
  }
  d->num_rooms = i;

  for (; i < MAX_ROOMS; i++) {
    d->rooms[i].exists = 0;
  }

  for (i = 0; i < d->num_rooms; i++) {
    d->rooms[i].size[dim_x] = ROOM_MIN_X;
    d->rooms[i].size[dim_y] = ROOM_MIN_Y;
    while (rand_under(7, 8)) {
      d->rooms[i].size[dim_x]++;
    }
    while (rand_under(3, 4)) {
      d->rooms[i].size[dim_y]++;
    }
    /* Initially, every room is connected only to itself. */
    d->rooms[i].connected = i;
  }

  return 0;
}

int empty_dungeon(dungeon_t *d)
{
  uint8_t x, y;

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      d->map[y][x] = ter_wall;
      d->hardness[y][x] = rand_range(1, 254);
      if (y == 0 || y == DUNGEON_Y - 1 ||
          x == 0 || x == DUNGEON_X - 1) {
        d->map[y][x] = ter_wall_immutable;
        d->hardness[y][x] = 255;
      }
    }
  }

  return 0;
}

int gen_dungeon(dungeon_t *d)
{
  /*
  pair_t p1, p2;

  p1[dim_x] = rand_range(1, 158);
  p1[dim_y] = rand_range(1, 94);
  p2[dim_x] = rand_range(1, 158);
  p2[dim_y] = rand_range(1, 94);
  */

  empty_dungeon(d);

  /*
  connect_two_points_recursive(d, p1, p2);
  return 0;
  */

  make_rooms(d);
  place_rooms(d);
  connect_rooms(d);

  return 0;
}

void render_dungeon(dungeon_t *d)
{
  pair_t p;

  for (p[dim_y] = 0; p[dim_y] < DUNGEON_Y; p[dim_y]++) {
    for (p[dim_x] = 0; p[dim_x] < DUNGEON_X; p[dim_x]++) {
      switch (d->map[p[dim_y]][p[dim_x]]) {
      case ter_wall:
      case ter_wall_no_room:
      case ter_wall_no_floor:
      case ter_wall_immutable:
        putchar('#');
        break;
      case ter_floor:
      case ter_floor_room:
      case ter_floor_hall:
      case ter_floor_tentative:
        putchar('.');
        break;
      case ter_debug:
        putchar('*');
        break;
      }
    }
    putchar('\n');
  }
}

int write_dungeon_map(dungeon_t *d, FILE *f)
{
  uint32_t x, y;
  uint8_t zero = 0;
  uint8_t non_zero = 1;
  uint32_t found_error;

  for (found_error = 0, y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      /* First byte is zero if wall, non-zero if floor */
      switch (d->map[y][x]) {
      case ter_debug:
        if (!found_error) {
          found_error = 1;
          fprintf(stderr, "Bad cell in dungeon.  Output may be corrupt.\n");
        }
        /* Allow to fall through and write a wall cell. */
      case ter_wall:
      case ter_wall_no_room:
      case ter_wall_no_floor:
      case ter_wall_immutable:
        fwrite(&zero, sizeof (zero), 1, f);
        break;
      case ter_floor:
      case ter_floor_room:
      case ter_floor_hall:
      case ter_floor_tentative:
        fwrite(&non_zero, sizeof (non_zero), 1, f);
        break;
      default:
        if (!found_error) {
          found_error = 1;
          fprintf(stderr, "Bad cell in dungeon.  Output may be corrupt.\n");
        }
        fwrite(&zero, sizeof (zero), 1, f);
      }

      /* Second byte is non-zero if room, zero otherwise */
      switch (d->map[y][x]) {
      case ter_debug:
      case ter_wall:
      case ter_wall_no_room:
      case ter_wall_no_floor:
      case ter_wall_immutable:
      case ter_floor:
        fwrite(&zero, sizeof (zero), 1, f);
        break;
      case ter_floor_room:
        fwrite(&non_zero, sizeof (non_zero), 1, f);
        break;
      case ter_floor_hall:
      case ter_floor_tentative:
      default:
        fwrite(&zero, sizeof (zero), 1, f);
      }

      /* Third byte is non-zero if a corridor, zero otherwise */
      switch (d->map[y][x]) {
      case ter_debug:
      case ter_wall:
      case ter_wall_no_room:
      case ter_wall_no_floor:
      case ter_wall_immutable:
      case ter_floor:
      case ter_floor_room:
        fwrite(&zero, sizeof (zero), 1, f);
        break;
      case ter_floor_hall:
        fwrite(&non_zero, sizeof (non_zero), 1, f);
        break;
      case ter_floor_tentative:
      default:
        fwrite(&zero, sizeof (zero), 1, f);
      }

      /* And the fourth byte is the hardness */
      fwrite(&d->hardness[y][x], sizeof (d->hardness[y][x]), 1, f);
      
    }
  }

  return 0;
}

int write_rooms(dungeon_t *d, FILE *f)
{
  uint32_t i;

  for (i = 0; i < d->num_rooms; i++) {
    /* write order is xpos, ypos, width, height */
    fwrite(&d->rooms[i].position[dim_x], 1, 1, f);
    fwrite(&d->rooms[i].position[dim_y], 1, 1, f);
    fwrite(&d->rooms[i].size[dim_x], 1, 1, f);
    fwrite(&d->rooms[i].size[dim_y], 1, 1, f);
  }

  return 0;
}

uint32_t calculate_dungeon_size(dungeon_t *d)
{
  return ((DUNGEON_X * DUNGEON_Y * 4 /* hard-coded value from the spec */)  +
          2 /* Also hard-coded from the spec; storage for the room count */ +
          (4 /* Also hard-coded, bytes per room */ * d->num_rooms));
}

int makedirectory(char *dir)
{
  char *slash;

  for (slash = dir + strlen(dir); slash > dir && *slash != '/'; slash--)
    ;

  if (slash == dir) {
    return 0;
  }

  if (mkdir(dir, 0700)) {
    if (errno != ENOENT && errno != EEXIST) {
      fprintf(stderr, "mkdir(%s): %s\n", dir, strerror(errno));
      return 1;
    }
    if (*slash != '/') {
      return 1;
    }
    *slash = '\0';
    if (makedirectory(dir)) {
      *slash = '/';
      return 1;
    }

    *slash = '/';
    if (mkdir(dir, 0700) && errno != EEXIST) {
      fprintf(stderr, "mkdir(%s): %s\n", dir, strerror(errno));
      return 1;
    }
  }

  return 0;
}

int write_dungeon(dungeon_t *d)
{
  char *home;
  char *filename;
  FILE *f;
  size_t len;
  uint32_t be32;
  uint16_t be16;

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

  /* The semantic, which is 6 bytes, 0-5 */
  fwrite(DUNGEON_SAVE_SEMANTIC, 1, strlen(DUNGEON_SAVE_SEMANTIC), f);

  /* The version, 4 bytes, 6-9 */
  be32 = htobe32(DUNGEON_SAVE_VERSION);
  fwrite(&be32, sizeof (be32), 1, f);

  /* The size of the rest of the file, 4 bytes, 10-13 */
  be32 = htobe32(calculate_dungeon_size(d));
  fwrite(&be32, sizeof (be32), 1, f);

  /* The dungeon map, 61440 bytes, 14-61453 */
  write_dungeon_map(d, f);

  /* The room count, 2 bytes, 61454-61455 */
  be16 = htobe16(d->num_rooms);
  fwrite(&be16, sizeof (be16), 1, f);

  /* And the rooms, be16 * 4 bytes, 61456-end */
  write_rooms(d, f);

  fclose(f);

  return 0;
}

int read_dungeon_map(dungeon_t *d, FILE *f)
{
  uint32_t x, y;
  uint8_t open, room, corr;

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      fread(&open, sizeof (open), 1, f);
      fread(&room, sizeof (room), 1, f);
      fread(&corr, sizeof (corr), 1, f);
      fread(&d->hardness[y][x], sizeof (d->hardness[y][x]), 1, f);
      if (room) {
        d->map[y][x] = ter_floor_room;
      } else if (corr) {
        d->map[y][x] = ter_floor_hall;
      } else if (y == 0 || y == DUNGEON_Y - 1 ||
                 x == 0 || x == DUNGEON_X - 1) {
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

  for (i = 0; i < d->num_rooms; i++) {
    fread(&d->rooms[i].position[dim_x], 1, 1, f);
    fread(&d->rooms[i].position[dim_y], 1, 1, f);
    fread(&d->rooms[i].size[dim_x], 1, 1, f);
    fread(&d->rooms[i].size[dim_y], 1, 1, f);
  }

  return 0;
}

int read_dungeon(dungeon_t *d, char *file)
{
  char semantic[6];
  uint32_t be32;
  uint16_t be16;
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
  /* 16 is the 14 byte header + the two byte room count */
  if (buf.st_size < 16 + calculate_dungeon_size(d)) {
    fprintf(stderr, "Dungeon appears to be truncated.\n");
    exit(-1);
  }

  fread(semantic, sizeof(semantic), 1, f);
  semantic[6] = '\0';
  if (strncmp(semantic, DUNGEON_SAVE_SEMANTIC, 6)) {
    fprintf(stderr, "Not an RLG229 save file.\n");
    exit(-1);
  }
  fread(&be32, sizeof (be32), 1, f);
  if (be32toh(be32) != 0) { /* Since we expect zero, be32toh() is a no-op. */
    fprintf(stderr, "File version mismatch.\n");
    exit(-1);
  }
  fread(&be32, sizeof (be32), 1, f);
  if (buf.st_size - 14 != be32toh(be32)) {
    fprintf(stderr, "File size mismatch.\n");
    exit(-1);
  }
  read_dungeon_map(d, f);
  fread(&be16, sizeof (be16), 1, f);
  d->num_rooms = be16toh(be16);
  if (buf.st_size != 14 + calculate_dungeon_size(d)) {
    fprintf(stderr, "Incorrect file size.\n");
    exit(-1);
  }
  read_rooms(d, f);

  fclose(f);

  return 0;
}

void usage(char *name)
{
  fprintf(stderr,
          "Usage: %s [-r|--rand <seed>] [-l|--load [<file>]] [-s|--save]\n",
          name);

  exit(-1);
}

int main(int argc, char *argv[])
{
  dungeon_t d;
  time_t seed;
  struct timeval tv;
  uint32_t i;
  uint32_t do_load, do_save, do_seed;
  uint32_t long_arg;
  char *save_file;

  /* Default behavior: Seed with the time, generate a new dungeon, *
   * and don't write to disk.                                      */
  do_load = do_save = 0;
  do_seed = 1;
  save_file = NULL;

  /* The project spec requires '--load' and '--save'.  It's common  *
   * to have short and long forms of most switches (assuming you    *
   * don't run out of letters).  For now, we've got plenty.  Long   *
   * forms use whole words and take two dashes.  Short forms use an *
   * abbreviation after a single dash.  We'll add '--rand' (to      *
   * specify a random seed), which will take an argument of it's    *
   * own, and we'll add short forms for all three commands, '-l',   *
   * '-s', and '-r', respectively.  We're also going to allow an    *
   * optional argument to load to allow us to load non-default save *
   * files.  No means to save to non-default locations, however.    */
  if (argc > 1) {
    for (i = 1, long_arg = 0; i < argc; i++, long_arg = 0) {
      if (argv[i][0] == '-') { /* All switches start with a dash */
        if (argv[i][1] == '-') {
          argv[i]++;    /* Make the argument have a single dash so we can */
          long_arg = 1; /* handle long and short args at the same place.  */
        }
        switch (argv[i][1]) {
        case 'r':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-rand")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%lu", &seed) /* Argument is not an integer */) {
            usage(argv[0]);
          }
          do_seed = 0;
          break;
        case 'l':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-load"))) {
            usage(argv[0]);
          }
          do_load = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            save_file = argv[++i];
          }
          break;
        case 's':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-save"))) {
            usage(argv[0]);
          }
          do_save = 1;
          break;
        default:
          usage(argv[0]);
        }
      } else { /* No dash */
        usage(argv[0]);
      }
    }
  }

  if (do_seed) {
    /* Allows me to generate more than one dungeon *
     * per second, as opposed to time().           */
    gettimeofday(&tv, NULL);
    seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  }

  printf("Seed is %ld.\n", seed);
  srand(seed);

  if (do_load) {
    read_dungeon(&d, save_file);
  } else {
    gen_dungeon(&d);
  }

  render_dungeon(&d);

  if (do_save) {
    write_dungeon(&d);
  }

  return 0;
}
