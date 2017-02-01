#include <stdio.h>
#include <string.h>
#include <endian.h>

/* ./fileio -wt <file>*
   ./fileio -rt <file>*
   ./fileio -wb <file>*
   ./fileio -rb <file>*/

typedef enum action {
  action_read_binary,
  action_write_binary,
  action_read_text,
  action_write_text,
} action_t;

int main(int argc, char *argv[])
{
  FILE *f;
  struct {
    int i;
    int j;
    int k;
  } s = {3,1,4};
  action_t action;

  if (argc != 3){
    fprintf(stderr, "Wrong number of parameters.\n");
    return -1;
  }

  if (!strcmp(argv[1], "-wt")) {
    action = action_write_text;

  }
  else if(!strcmp(argv[1], "-rt")){
    action = action_read_text;
  }

  else if(!strcmp(argv[1], "-wb")){
    action = action_write_binary;
  }
  else if(!strcmp(argv[1], "-rb")){
    action = action_read_binary;
  }
  else{
    fprintf(stderr, "Unrecongnized ");
    return -1;
  }

  if (action == action_write_text || action == action_write_binary){
    if (!(f = fopen(argv[2], "w"))) {
      fprintf(stderr, "Failed to open %s.\n", argv[2]);
    }
  }
  if (action == action_read_text || action == action_read_binary){
    if (!(f = fopen(argv[2], "r"))) {
      fprintf(stderr, "Failed to open %s.\n", argv[2]);
    }
  }

  switch(action){

  case action_read_text:
    fscanf(f, "%d %d %d", &s.i, &s.j, &s.k);
    break;
  case action_read_binary:
    &s.i = be32toh(i);
    fread(&s.i, sizeof (s.i), 1, f);
    fread(&s.j, sizeof (s.j), 1, f);
    fread(&s.k, sizeof (s.k), 1, f);
    break;
  case action_write_text:
    fprintf(f, "%d %d %d", s.i, s.j, s.k);
    break;
  case action_write_binary:
    i = htobe32(s.i);
    fwrite(&s.i, sizeof (s.i), 1, f);
    fwrite(&s.j, sizeof (s.j), 1, f);
    fwrite(&s.k, sizeof (s.k), 1, f);
    break;
  }

  return 0;
}
