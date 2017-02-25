#ifndef LIST_H
# define LIST_H

# ifdef __cplusplus
extern "C" {
# endif

# include <stdlib.h>
# include <stdint.h>
# include <stdbool.h>

struct list_item;
typedef struct list_item list_item_t;

typedef struct list {
  list_item_t *head, *tail;
  uint32_t length;
  int32_t (*compare)(const void *key, const void *with);
  void (*datum_delete)(void *);
} list_t;

typedef list_item_t * list_iterator_t;
typedef list_item_t * list_reverse_iterator_t;

void list_init(list_t *l,
	       int32_t (*compare)(const void *key, const void *with),
	       void (*datum_delete)(void *datum));
void list_delete(list_t *l);
void list_reset(list_t *l);
void list_insert_head(list_t *l, void *v);
void list_insert_tail(list_t *l, void *v);
void list_insert_before(list_t *l, list_item_t *next, void *v);
void list_insert_after(list_t *l, list_item_t *pred, void *v);
void list_insert_sorted(list_t *l, void *v);
void list_insert_item_head(list_t *l, list_item_t *i);
void list_remove_item(list_t *l, list_item_t *i);
void list_remove_head(list_t *l);
void list_remove_tail(list_t *l);
list_item_t *list_find_item(list_t *l, void *datum);
list_item_t *list_get_head_item(list_t *l);
list_item_t *list_get_tail_item(list_t *l);
void *list_find(list_t *l, void *datum);
void *list_get_head(list_t *l);
void *list_get_tail(list_t *l);
uint32_t list_get_length(list_t *l);
bool list_is_empty(list_t *l);
bool list_not_empty(list_t *l);
void list_visit_items(list_t *l, void (*visitor)(void *v));

void *list_item_get_datum(list_item_t *li);

void list_iterator_init(list_t *l, list_iterator_t *li);
void list_iterator_delete(list_iterator_t *li);
void list_iterator_next(list_iterator_t *li);
void list_iterator_prev(list_iterator_t *li);
void *list_iterator_get_datum(list_iterator_t *li);
bool list_iterator_is_valid(list_iterator_t *li);
void list_reverse_iterator_init(list_t *l, list_iterator_t *li);
void list_reverse_iterator_delete(list_iterator_t *li);
void list_reverse_iterator_next(list_iterator_t *li);
void list_reverse_iterator_prev(list_iterator_t *li);
void *list_reverse_iterator_get_datum(list_iterator_t *li);
bool list_reverse_iterator_is_valid(list_reverse_iterator_t *li);

# ifdef __cplusplus
}
# endif

#endif
