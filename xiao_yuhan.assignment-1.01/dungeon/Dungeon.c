#include <stdio.h>
/**
 * rand() adn srand()
 */
#include <stdlib.h>
/**
 * time()
 */
#include <time.h>

/**
 * struct for room
 */
typedef struct room {
  int x;
  int y;
  int width;
  int height;
  int isCreated;
} room_t;
/**
 * create dungeon
 */
char dungeon[105][160];
/**
 * All rooms in the dungeon
 */
room_t rooms[18];

/**
 * the number of rooms exist in this dungeon
 */
int numberOfRooms;
/**
 * the number of time tried to add a room
 */
int tryTimes;

void createEmptyDungeon(void);
room_t randomRoom();
void addRoom(void);
int findNeighbors(int a, int b);
void addRoomsToDungeon(room_t r);
void printDungeon(void);

/**
 * create an empty dungeon
 */
void createEmptyDungeon(void)
{
  int i, j;
  for(j = 0; j < 105; ++j){
    for(i = 0; i< 160; ++i){
      dungeon[j][i] = ' ';
    }
  }
}
/**
 * add rooms into the dungeon
 */
void addRoomsToDungeon(room_t r)
{
  int i, j, k, check;
  check = 0;
  for(j = r.y; j < r.y + r.height; ++j){
    for(i = r.x; i < r.x + r.width; ++i){
      if(dungeon[j][i] == '.'){
        ++check;
      }
      if(findNeighbors(i, j) > 0){
        return;
      }
    }
  }
  if(check > 0){
    return;
  }
  else{
    for(j = r.y; j < r.y + r.height; ++j){
      for(i = r.x; i < r.x + r.width; ++i){
        dungeon[j][i] = '.';
      }
    }
    int a = 0;
    while(rooms[a].isCreated == 1){
      ++a;
    }
    rooms[a] = r;
  }
}

/**
 * print the dungeon
 */
void printDungeon(void)
{
  int i, j;
  for(j = 0; j < 104; j++){
    for(i = 0; i < 159; i++){
      printf("%c", dungeon[j][i]);
    }
    printf("\n");
  }
}

/**
 * create a random room
 */
room_t randomRoom()
{
  room_t temp;
  temp.x = rand() % 160;
  temp.y = rand() % 105;
  temp.width = rand() % (50 - 7) + 7;
  temp.height = rand() % (26 - 5) + 5;
  temp.isCreated = 1;
  ++tryTimes;
  return temp;
}

/**
 * add a random room into the array rooms
 */
void addRoom(void)
{
  room_t add;
  add = randomRoom();
  int x_1, y_1, x_2, y_2;
  /**
  * check if the room is valid
  */
  x_1 = add.x;
  y_1 = add.y;
  x_2 = add.x + add.width;
  y_2 = add.y + add.height;
  if(x_2 > 159 || y_2 > 104){
    return;
  }
  else{
    addRoomsToDungeon(add);
  }
}

int findNeighbors(int a, int b)
{
  int count = 0;
  if(a != 0 && a != 159 && b != 0 && b != 104){
    if(dungeon[b][a-1] == '.') ++count;
    if(dungeon[b][a+1] == '.') ++count;
    if(dungeon[b-1][a] == '.') ++count;
    if(dungeon[b+1][a] == '.') ++count;
    if(dungeon[b-1][a-1] == '.') ++count;
    if(dungeon[b+1][a-1] == '.') ++count;
    if(dungeon[b-1][a+1] == '.') ++count;
    if(dungeon[b+1][a+1] == '.') ++count;
  }
  else if(a == 0 && b != 0 && b != 104){
    if(dungeon[b][a+1] == '.') ++count;
    if(dungeon[b-1][a] == '.') ++count;
    if(dungeon[b+1][a] == '.') ++count;
    if(dungeon[b-1][a+1] == '.') ++count;
    if(dungeon[b+1][a+1] == '.') ++count;
  }
  else if(a == 159 && b!= 0 &&b != 104){
    if(dungeon[b][a-1] == '.') ++count;
    if(dungeon[b-1][a] == '.') ++count;
    if(dungeon[b+1][a] == '.') ++count;
    if(dungeon[b-1][a-1] == '.') ++count;
    if(dungeon[b+1][a-1] == '.') ++count;
  }
  /**
   * upper bound
   */
  else if(b == 0 && a != 0 && a != 159){
    if(dungeon[b][a-1] == '.') ++count;
    if(dungeon[b][a+1] == '.') ++count;
    if(dungeon[b+1][a] == '.') ++count;
    if(dungeon[b+1][a-1] == '.') ++count;
    if(dungeon[b+1][a+1] == '.') ++count;
  }
  /**
   * lower bound
   */
  else if(b == 104 && a != 0 && a != 159){
    if(dungeon[b][a-1] == '.') ++count;
    if(dungeon[b][a+1] == '.') ++count;
    if(dungeon[b-1][a] == '.') ++count;
    if(dungeon[b-1][a-1] == '.') ++count;
    if(dungeon[b-1][a+1] == '.') ++count;
  }
  /**
   * four special points
   */
  else if(a == 0 && b == 0){
    if(dungeon[b][a+1] == '.') ++count;
    if(dungeon[b+1][a] == '.') ++count;
    if(dungeon[b+1][a+1] == '.') ++count;
  }
  else if(a == 0 && b == 104){
    if(dungeon[b][a+1] == '.') ++count;
    if(dungeon[b-1][a] == '.') ++count;
    if(dungeon[b-1][a+1] == '.') ++count;
  }
  else if(a == 159 && b == 0){
    if(dungeon[b][a-1] == '.') ++count;
    if(dungeon[b+1][a] == '.') ++count;
    if(dungeon[b+1][a-1] == '.') ++count;
  }
  else{
    if(dungeon[b][a-1] == '.') ++count;
    if(dungeon[b-1][a] == '.') ++count;
    if(dungeon[b-1][a-1] == '.') ++count;
  }
  return count;
}

int main(int argc, char *argv[])
{
  int seed;
  if(argc == 2){
    seed = atoi(argv[1]);
  }
  else{
    seed = time(NULL);
  }
  srand(seed); /*seed*/
  room_t temp = randomRoom();
  tryTimes = 0;
  createEmptyDungeon();
  while(tryTimes < 50 || rooms[9].isCreated == 0){
    if(rooms[17].isCreated == 1){
      break;
    }
    addRoom();
  }
  printDungeon();
  return 0;
}
