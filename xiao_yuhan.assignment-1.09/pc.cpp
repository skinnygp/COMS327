#include <stdlib.h>
#include <ncurses.h>
#include <string>

#include "dungeon.h"
#include "pc.h"
#include "utils.h"
#include "move.h"
#include "path.h"
#include "io.h"
#include "object.h"

int pc::check_space()
{
  int i;
  for(i = 0; i < 10; i++){
    if(!carry_slot[i]) return i;
  }
  return -1;
}
int pc::pick_up_object(dungeon_t *d)
{
  if(check_space() != -1 && objpair(position)){
    carry_slot[check_space()] = objpair(position);
    io_queue_message("Picking up a %s ...... Done!", objpair(position)->get_name());
    objpair(position) = 0;
  }
  else if(check_space() == -1 && objpair(position)){
    io_queue_message("Bag is full. Unable to pick up %s!", objpair(position)->get_name());
  }
  return 0;
}
int pc::renew_speed()
{
  int i;
  speed = 10;
  for(i = 0; i < 12; i++){
    if(equipment_slot[i]) speed += equipment_slot[i]->get_speed();
  }
  if(speed < 1) speed = 1;
  return 0;
}
int pc::wear_object(int index)
{
  if (!carry_slot[index]) {
    io_queue_message("No object in this position!");
    return 1;
  }
  object *ob;
  uint32_t in;
  in = carry_slot[index]->get_type() - 1;
  if(in > objtype_SCROLL - 1){
    io_queue_message("This object cannot be worn");
    return 1;
  }
  if(carry_slot[index]->get_type() == objtype_RING && equipment_slot[in] && !equipment_slot[in++]) in++;
  ob = carry_slot[index];
  carry_slot[index] = equipment_slot[in];
  equipment_slot[in] = ob;
  io_queue_message("Wearing object %s ...... Done!", equipment_slot[in]->get_name());
  renew_speed();
  return 0;
}
int pc::take_off_object(object *o)
{
  if (!o) {
    io_queue_message("No object in this position!");
    return 1;
  }
  else if(check_space() == -1){
    io_queue_message("Baf is full! Unable to take off this object!");
    return 1;
  }
  carry_slot[check_space()] = o;
  io_queue_message("Taking off object %s ...... Done!", o->get_name());
  renew_speed();

  return 0;
}
int pc::drop_object(dungeon_t *d, object *o)
{
  if (!o) {
    io_queue_message("No object in this position!");
    return 1;
  }
  else if(objpair(position)){
    io_queue_message("Cannot drop here! There is an object on the floor!");
    return 1;
  }
  objpair(position) = o;
  io_queue_message("Droping off object %s ...... Done!", o->get_name());
  return 0;

}
int pc::expunge_object(object *o)
{
  if (!o) {
    io_queue_message("No object in this position!");
    return 1;
  }
  io_queue_message("Expunging object %s ...... Done!", o->get_name());
  delete o;
  return 0;
}


void pc_delete(pc *pc)
{
  if (pc) {
    free(pc);
  }
}

uint32_t pc_is_alive(dungeon_t *d)
{
  return d->PC->alive;
}

void place_pc(dungeon_t *d)
{
  character_set_y(d->PC, rand_range(d->rooms->position[dim_y],
                                   (d->rooms->position[dim_y] +
                                    d->rooms->size[dim_y] - 1)));
  character_set_x(d->PC, rand_range(d->rooms->position[dim_x],
                                   (d->rooms->position[dim_x] +
                                    d->rooms->size[dim_x] - 1)));
  io_calculate_offset(d);
  io_update_offset(d);

  pc_init_known_terrain(d->PC);

  io_display(d);
}

void config_pc(dungeon_t *d)
{
  static dice pc_dice(0, 1, 4);

  d->PC = new pc;

  memset(d->PC, 0, sizeof (*d->PC));
  d->PC->symbol = '@';

  place_pc(d);

  d->PC->speed = PC_SPEED;
  d->PC->alive = 1;
  d->PC->sequence_number = 0;
  d->PC->kills[kill_direct] = d->PC->kills[kill_avenged] = 0;
  d->PC->color.push_back(COLOR_WHITE);
  d->PC->damage = &pc_dice;
  d->PC->name = "Isabella Garcia-Shapiro";

  d->PC->hp = 5000;

  int i;
  for(i = 0; i < 12; i++){
    d->PC->equipment_slot[i] = NULL;
  }
  for(i = 0; i < 10; i++){
    d->PC->carry_slot[i] = NULL;
  }

  d->character_map[character_get_y(d->PC)][character_get_x(d->PC)] = d->PC;

  dijkstra(d);
  dijkstra_tunnel(d);

  io_calculate_offset(d);
}

uint32_t pc_next_pos(dungeon_t *d, pair_t dir)
{
  static uint32_t have_seen_corner = 0;
  static uint32_t count = 0;

  dir[dim_y] = dir[dim_x] = 0;

  if (in_corner(d, d->PC)) {
    if (!count) {
      count = 1;
    }
    have_seen_corner = 1;
  }

  /* First, eat anybody standing next to us. */
  if (charxy(character_get_x(d->PC) - 1, character_get_y(d->PC) - 1)) {
    dir[dim_y] = -1;
    dir[dim_x] = -1;
  } else if (charxy(character_get_x(d->PC), character_get_y(d->PC) - 1)) {
    dir[dim_y] = -1;
  } else if (charxy(character_get_x(d->PC) + 1, character_get_y(d->PC) - 1)) {
    dir[dim_y] = -1;
    dir[dim_x] = 1;
  } else if (charxy(character_get_x(d->PC) - 1, character_get_y(d->PC))) {
    dir[dim_x] = -1;
  } else if (charxy(character_get_x(d->PC) + 1, character_get_y(d->PC))) {
    dir[dim_x] = 1;
  } else if (charxy(character_get_x(d->PC) - 1, character_get_y(d->PC) + 1)) {
    dir[dim_y] = 1;
    dir[dim_x] = -1;
  } else if (charxy(character_get_x(d->PC), character_get_y(d->PC) + 1)) {
    dir[dim_y] = 1;
  } else if (charxy(character_get_x(d->PC) + 1, character_get_y(d->PC) + 1)) {
    dir[dim_y] = 1;
    dir[dim_x] = 1;
  } else if (!have_seen_corner || count < 250) {
    /* Head to a corner and let most of the NPCs kill each other off */
    if (count) {
      count++;
    }
    if (!against_wall(d, d->PC) && ((rand() & 0x111) == 0x111)) {
      dir[dim_x] = (rand() % 3) - 1;
      dir[dim_y] = (rand() % 3) - 1;
    } else {
      dir_nearest_wall(d, d->PC, dir);
    }
  }else {
    /* And after we've been there, let's head toward the center of the map. */
    if (!against_wall(d, d->PC) && ((rand() & 0x111) == 0x111)) {
      dir[dim_x] = (rand() % 3) - 1;
      dir[dim_y] = (rand() % 3) - 1;
    } else {
      dir[dim_x] = ((character_get_x(d->PC) > DUNGEON_X / 2) ? -1 : 1);
      dir[dim_y] = ((character_get_y(d->PC) > DUNGEON_Y / 2) ? -1 : 1);
    }
  }

  /* Don't move to an unoccupied location if that places us next to a monster */
  if (!charxy(character_get_x(d->PC) + dir[dim_x],
              character_get_y(d->PC) + dir[dim_y]) &&
      ((charxy(character_get_x(d->PC) + dir[dim_x] - 1,
               character_get_y(d->PC) + dir[dim_y] - 1) &&
        (charxy(character_get_x(d->PC) + dir[dim_x] - 1,
                character_get_y(d->PC) + dir[dim_y] - 1) != d->PC)) ||
       (charxy(character_get_x(d->PC) + dir[dim_x] - 1,
               character_get_y(d->PC) + dir[dim_y]) &&
        (charxy(character_get_x(d->PC) + dir[dim_x] - 1,
                character_get_y(d->PC) + dir[dim_y]) != d->PC)) ||
       (charxy(character_get_x(d->PC) + dir[dim_x] - 1,
               character_get_y(d->PC) + dir[dim_y] + 1) &&
        (charxy(character_get_x(d->PC) + dir[dim_x] - 1,
                character_get_y(d->PC) + dir[dim_y] + 1) != d->PC)) ||
       (charxy(character_get_x(d->PC) + dir[dim_x],
               character_get_y(d->PC) + dir[dim_y] - 1) &&
        (charxy(character_get_x(d->PC) + dir[dim_x],
                character_get_y(d->PC) + dir[dim_y] - 1) != d->PC)) ||
       (charxy(character_get_x(d->PC) + dir[dim_x],
               character_get_y(d->PC) + dir[dim_y] + 1) &&
        (charxy(character_get_x(d->PC) + dir[dim_x],
                character_get_y(d->PC) + dir[dim_y] + 1) != d->PC)) ||
       (charxy(character_get_x(d->PC) + dir[dim_x] + 1,
               character_get_y(d->PC) + dir[dim_y] - 1) &&
        (charxy(character_get_x(d->PC) + dir[dim_x] + 1,
                character_get_y(d->PC) + dir[dim_y] - 1) != d->PC)) ||
       (charxy(character_get_x(d->PC) + dir[dim_x] + 1,
               character_get_y(d->PC) + dir[dim_y]) &&
        (charxy(character_get_x(d->PC) + dir[dim_x] + 1,
                character_get_y(d->PC) + dir[dim_y]) != d->PC)) ||
       (charxy(character_get_x(d->PC) + dir[dim_x] + 1,
               character_get_y(d->PC) + dir[dim_y] + 1) &&
        (charxy(character_get_x(d->PC) + dir[dim_x] + 1,
                character_get_y(d->PC) + dir[dim_y] + 1) != d->PC)))) {
    dir[dim_x] = dir[dim_y] = 0;
  }

  return 0;
}

uint32_t pc_in_room(dungeon_t *d, uint32_t room)
{
  if ((room < d->num_rooms)                                     &&
      (character_get_x(d->PC) >= d->rooms[room].position[dim_x]) &&
      (character_get_x(d->PC) < (d->rooms[room].position[dim_x] +
                                d->rooms[room].size[dim_x]))    &&
      (character_get_y(d->PC) >= d->rooms[room].position[dim_y]) &&
      (character_get_y(d->PC) < (d->rooms[room].position[dim_y] +
                                d->rooms[room].size[dim_y]))) {
    return 1;
  }

  return 0;
}

void pc_learn_terrain(pc *p, pair_t pos, terrain_type_t ter)
{
  p->known_terrain[pos[dim_y]][pos[dim_x]] = ter;
  p->visible[pos[dim_y]][pos[dim_x]] = 1;
}

void pc_reset_visibility(pc *p)
{
  uint32_t y, x;

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      p->visible[y][x] = 0;
    }
  }
}

terrain_type_t pc_learned_terrain(pc *p, int16_t y, int16_t x)
{
  if (y < 0 || y >= DUNGEON_Y || x < 0 || x >= DUNGEON_X) {
    io_queue_message("Invalid value to %s: %d, %d", __FUNCTION__, y, x);
  }

  return p->known_terrain[y][x];
}

void pc_init_known_terrain(pc *p)
{
  uint32_t y, x;

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      p->known_terrain[y][x] = ter_unknown;
      p->visible[y][x] = 0;
    }
  }
}

void pc_observe_terrain(pc *p, dungeon_t *d)
{
  pair_t where;
  int16_t y_min, y_max, x_min, x_max;

  y_min = p->position[dim_y] - PC_VISUAL_RANGE;
  if (y_min < 0) {
    y_min = 0;
  }
  y_max = p->position[dim_y] + PC_VISUAL_RANGE;
  if (y_max > DUNGEON_Y - 1) {
    y_max = DUNGEON_Y - 1;
  }
  x_min = p->position[dim_x] - PC_VISUAL_RANGE;
  if (x_min < 0) {
    x_min = 0;
  }
  x_max = p->position[dim_x] + PC_VISUAL_RANGE;
  if (x_max > DUNGEON_X - 1) {
    x_max = DUNGEON_X - 1;
  }

  for (where[dim_y] = y_min; where[dim_y] <= y_max; where[dim_y]++) {
    where[dim_x] = x_min;
    can_see(d, p->position, where, 1, 1);
    where[dim_x] = x_max;
    can_see(d, p->position, where, 1, 1);
  }
  /* Take one off the x range because we alreay hit the corners above. */
  for (where[dim_x] = x_min - 1; where[dim_x] <= x_max - 1; where[dim_x]++) {
    where[dim_y] = y_min;
    can_see(d, p->position, where, 1, 1);
    where[dim_y] = y_max;
    can_see(d, p->position, where, 1, 1);
  }
}

int32_t is_illuminated(pc *p, int16_t y, int16_t x)
{
  return p->visible[y][x];
}

void pc_see_object(character *the_pc, object *o)
{
  if (o) {
    o->has_been_seen();
  }
}
