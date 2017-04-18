#ifndef IO_H
# define IO_H

typedef struct dungeon dungeon_t;
class character;

void io_combat(dungeon_t *d, character *c);
void io_start(dungeon_t *d);
void io_welcome(dungeon_t *d);
void io_init_terminal(void);
void io_reset_terminal(void);
void io_display(dungeon_t *d);
void io_handle_input(dungeon_t *d);
void io_queue_message(const char *format, ...);
void io_calculate_offset(dungeon_t *d);
void io_update_offset(dungeon_t *d);

#endif
