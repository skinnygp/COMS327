#ifdef __cplusplus
extern "C"
{
  #endif
  #include <stdlib.h>

  #include "character.h"
  #include "heap.h"
  #include "npc.h"
  #include "pc.h"
  #include "dungeon.h"
  #ifdef __cplusplus
}
  #endif





void character_delete(character_t *v)
{
  delete v;
}

uint32_t can_see(dungeon_t *d, character_t *voyeur, pair_t exhibitionist)
{
  /* Application of Bresenham's Line Drawing Algorithm.  If we can draw *
   * a line from v to e without intersecting any walls, then v can see  *
   * e.  Unfortunately, Bresenham isn't symmetric, so line-of-sight     *
   * based on this approach is not reciprocal (Helmholtz Reciprocity).  *
   * This is a very real problem in roguelike games, and one we're      *
   * going to ignore for now.  Algorithms that are symmetrical are far  *
   * more expensive.                                                    */

  pair_t first, second;
  pair_t del, f;
  int16_t a, b, c, i;

  first[dim_x] = getX(voyeur);
  first[dim_y] = getY(voyeur);
  second[dim_x] = exhibitionist[dim_x];
  second[dim_y] = exhibitionist[dim_y];

  if ((abs(first[dim_x] - second[dim_x]) > VISUAL_RANGE) ||
      (abs(first[dim_y] - second[dim_y]) > VISUAL_RANGE)) {
    return 0;
  }

  /*
  mappair(first) = ter_debug;
  mappair(second) = ter_debug;
  */

  if (second[dim_x] > first[dim_x]) {
    del[dim_x] = second[dim_x] - first[dim_x];
    f[dim_x] = 1;
  } else {
    del[dim_x] = first[dim_x] - second[dim_x];
    f[dim_x] = -1;
  }

  if (second[dim_y] > first[dim_y]) {
    del[dim_y] = second[dim_y] - first[dim_y];
    f[dim_y] = 1;
  } else {
    del[dim_y] = first[dim_y] - second[dim_y];
    f[dim_y] = -1;
  }

  if (del[dim_x] > del[dim_y]) {
    a = del[dim_y] + del[dim_y];
    c = a - del[dim_x];
    b = c - del[dim_x];
    for (i = 0; i <= del[dim_x]; i++) {
      /*need add*/
      if ((mappair(first) < ter_floor) && i && (i != del[dim_x])) {
        return 0;
      }
      /*      mappair(first) = ter_debug;*/
      first[dim_x] += f[dim_x];
      if (c < 0) {
        c += a;
      } else {
        c += b;
        first[dim_y] += f[dim_y];
      }
    }
    return 1;
  } else {
    a = del[dim_x] + del[dim_x];
    c = a - del[dim_y];
    b = c - del[dim_y];
    for (i = 0; i <= del[dim_y]; i++) {

      /*need add*/

      if ((mappair(first) < ter_floor) && i && (i != del[dim_y])) {
        return 0;
      }
      /*      mappair(first) = ter_debug;*/
      first[dim_y] += f[dim_y];
      if (c < 0) {
        c += a;
      } else {
        c += b;
        first[dim_x] += f[dim_x];
      }
    }
    return 1;
  }

  return 1;
}

char getSymbol(character_t *c)
{
  return c->symbol;
}

char setSymbol(character_t *c, char symbol)
{
  return c->symbol = symbol;
}
int getX(character_t *c)
{
  return c->position[dim_x];
}
int getY(character_t *c)
{
  return c->position[dim_y];
}
int8_t setX(character_t *c, int8_t x)
{
  return c->position[dim_x] = x;
}
int8_t setY(character_t *c, int8_t y)
{
  return c->position[dim_y] = y;
}
int32_t getSpeed(character_t *c)
{
  return c->speed;
}
int32_t setSpeed(character_t *c, int32_t speed)
{
  return c->speed = speed;
}
uint32_t isAlive(character_t *c)
{
  return c->alive;
}
uint32_t setAlive(character_t *c, uint32_t isAlive)
{
  return c->alive = isAlive;
}
uint32_t getSeqNum(character_t *c)
{
  return c->sequence_number;
}
uint32_t setSeqNum(character_t *c, uint32_t SN)
{
  return c->sequence_number = SN;
}
