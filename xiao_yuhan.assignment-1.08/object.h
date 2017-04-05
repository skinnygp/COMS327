#ifndef OBJECT_H
# define OBJECT_H

# include <stdint.h>

# include "dims.h"
# include "descriptions.h"
class dice;
class object{
 public:
   char symbol;
   const char *name;
   const char *desc;
   object_type_t type;
   uint32_t color;
   int32_t hit;
   const dice *damage;
   int32_t dodge;
   int32_t def;
   int32_t weight;
   int32_t speed;
   int32_t attr;
   int32_t val;
   pair_t position;
   int32_t display;
   object(const object_description &o);
};

typedef struct dungeon dungeon_t;

void display_object(dungeon_t *d);
void gen_object(dungeon_t *d);
void object_delete(object *o);
char object_get_symbol(object *o);
const char *object_get_name(object *o);
object_type_t object_get_type(object *o);
uint32_t object_get_color(object *o);
int32_t object_get_hit(object *o);
const dice *object_get_damage(object *o);
int32_t object_get_dodge(object *o);
int32_t object_get_def(object *o);
int32_t object_get_weight(object *o);
int32_t object_get_speed(object *o);
int32_t object_get_attr(object *o);
int32_t object_get_val(object *o);
int16_t *object_get_pos(object *o);
int16_t object_get_position_x(object *o);
int16_t object_get_position_y(object *o);
int32_t object_get_display(object *o);
int32_t object_set_display(object *o);
int32_t object_set_undisplay(object *o);
#endif
