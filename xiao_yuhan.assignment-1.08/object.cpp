#include <vector>
#include "object.h"
#include "dungeon.h"
#include "utils.h"
#include <cstring>

int16_t *object_get_pos(object *o)
{
  return o->position;
}
object::object(const object_description &o)
{
  symbol = object_symbol[o.get_type()];
  name = o.get_name().c_str();
  desc = o.get_description().c_str();
  type = o.get_type();
  color = o.get_color();
  hit = o.get_hit().roll();
  damage = &o.get_damage();
  dodge = o.get_dodge().roll();
  def = o.get_defence().roll();
  weight = o.get_weight().roll();
  speed = o.get_speed().roll();
  attr = o.get_attribute().roll();
  val = o.get_value().roll();
}
void display_object(dungeon_t *d)
{
  int x, y;
  for(y = 0; y < DUNGEON_Y; y++){
    for(x = 0; x < DUNGEON_X; x++){
      if(d->object_map[y][x]){
        if(charxy(x, y)) object_set_undisplay(d->object_map[y][x]);
        else object_set_display(d->object_map[y][x]);
      }
    }
  }
}
void gen_object(dungeon_t *d)
{
  int i;
  for (i = 0; i < 100; i++){
    object *o;
    const std::vector<object_description> &v = d->object_descriptions;
    const object_description &od = v[rand_range(0, v.size() - 1)];
    o = new object(od);
    pair_t p;
    uint32_t room;
    room = rand_range(1, d->num_rooms - 1);
    p[dim_y] = rand_range(d->rooms[room].position[dim_y],
                          (d->rooms[room].position[dim_y] +
                           d->rooms[room].size[dim_y] - 1));
    p[dim_x] = rand_range(d->rooms[room].position[dim_x],
                          (d->rooms[room].position[dim_x] +
                           d->rooms[room].size[dim_x] - 1));
    o->position[dim_y] = p[dim_y];
    o->position[dim_x] = p[dim_x];
    if(charpair(p)) o->display = 0;
    else o->display = 1;
    d->object_map[p[dim_y]][p[dim_x]] = o;
  }
}
void object_delete(object *o)
{
  delete o;
}
char object_get_symbol(object *o)
{
  return o->symbol;
}
const char *object_get_name(object *o)
{
  return o->name;
}
object_type_t object_get_type(object *o)
{
  return o->type;
}
uint32_t object_get_color(object *o)
{
  return o->color;
}
int32_t object_get_hit(object *o)
{
  return o->hit;
}
const dice *object_get_damage(object *o)
{
  return o->damage;
}
int32_t object_get_dodge(object *o)
{
  return o->dodge;
}
int32_t object_get_def(object *o)
{
  return o->def;
}
int32_t object_get_weight(object *o)
{
  return o->weight;
}
int32_t object_get_speed(object *o)
{
  return o->speed;
}
int32_t object_get_attr(object *o)
{
  return o->attr;
}
int32_t object_get_val(object *o)
{
  return o->val;
}
int16_t object_get_position_x(object *o)
{
  return o->position[dim_x];
}
int16_t object_get_position_y(object *o)
{
  return o->position[dim_y];
}
int32_t object_get_display(object *o)
{
  return o->display;
}
int32_t object_set_display(object *o)
{
  return o->display = 1;
}
int32_t object_set_undisplay(object *o)
{
  return o->display = 0;
}
