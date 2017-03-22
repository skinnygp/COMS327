#ifndef CHARACTER_H
# define CHARACTER_H


#ifdef __cplusplus
extern "C"{
  #endif

  # include <stdint.h>

  # include "dims.h"

#ifdef __cplusplus
}
  #endif

#ifdef __cplusplus
class character {
 public:
  char symbol;
  pair_t position;
  int32_t speed;
  uint32_t alive;
  /* Characters use to have a next_turn for the move queue.  Now that it is *
   * an event queue, there's no need for that here.  Instead it's in the    *
   * event.  Similarly, sequence_number was introduced in order to ensure   *
   * that the queue remains stable.  Also no longer necessary here, but in  *
   * this case, we'll keep it, because it provides a bit of interesting     *
   * metadata: locally, how old is this character; and globally, how many   *
   * characters have been created by the game.                              */
  uint32_t sequence_number;
};

extern "C"{
  # else
  typedef void character;
  #endif
  typedef struct dungeon dungeon_t;
  int32_t compare_characters_by_next_turn(const void *character1,
                                        const void *character2);
  uint32_t can_see(dungeon_t *d, character *voyeur, character *exhibitionist);
  void character_delete(character *v);
  char getSymbol(character *c);
  char setSymbol(character *c, char symbol);
  /*pair_t getPosition(character *c);*/
  int8_t getX(character *c);
  int8_t getY(character *c);
  int8_t setX(character *c, int8_t x);
  int8_t setY(character *c, int8_t y);
  int32_t getSpeed(character *c);
  int32_t setSpeed(character *c, int32_t speed);
  uint32_t isAlive(character *c);
  uint32_t setAlive(character *c, uint32_t isAlive);
  uint32_t getSeqNum(character *c);
  uint32_t setSeqNum(character *c, uint32_t SN);
  #ifdef __cplusplus
 }
  #endif

 #endif
