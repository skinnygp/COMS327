/**
 * @author Yuhan Xiao
 * @lastModifiedTime 10:12PM, Jan 24, 2017.
 */


#include <stdio.h>
/**
 * rand() and srand()
 */
#include <stdlib.h>
/**
 * time()
 */
#include <time.h>

/**
 * struct for point
 */
typedef struct point {
  int x;
  int y;
} point_t;

/**
 * struct for room
 */
typedef struct room {
  int x;
  int y;
  int width;
  int height;
  int isCreated;
  int isConnected;
  point_t forConnect;
} room_t;


/**
 * Map for dungeon
 */
char dungeon[105][160];
/**
 * All rooms in the dungeon
 */
room_t rooms[20];

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
point_t randomPoint(room_t r);
void connectTwoRooms(room_t start, room_t end);
int findClosestRoomX(room_t room);
int findClosestRoomY(room_t room);

/**
 * Dungeon generator: random a map for this dungeon.
 */
int main(int argc, char *argv[])
{
  numberOfRooms = 0;
  int seed;
  if(argc == 2){
    seed = atoi(argv[1]);
  }
  else{
    seed = time(NULL);
  }
  srand(seed); /*seed*/
  tryTimes = 0;
  createEmptyDungeon();
  /**
   * While tryTimes is less than 60 and the 10th room has not been created
   * Create rooms for this dungeon
   */
  while(tryTimes < 60 || rooms[9].isCreated == 0){
    addRoom();
  }
  int num;
  /**
   * connect 1 to 2; 2 to 3; 3 to 4 ... n to n+1
   */
  for(num = 0; num < numberOfRooms-1; ++num){
    connectTwoRooms(rooms[num], rooms[num+1]);
    rooms[num].isConnected = 1;
    rooms[num+1].isConnected = 1;
  }
  /**
   * connect the closest rooms in both X and Y direction
   */
  for(num = 0; num < numberOfRooms; ++num){
    int needConnectX = findClosestRoomX(rooms[num]);
    int needConnectY = findClosestRoomX(rooms[num]);
    connectTwoRooms(rooms[num], rooms[needConnectX]);
    connectTwoRooms(rooms[num], rooms[needConnectY]);
  }
  printDungeon();
  return 0;
}

/**
 * find the closest room in X direction
 */
int findClosestRoomX(room_t room)
{
  point_t point = room.forConnect;
  int x_pos = point.x;
  int minDistance = 10000;
  int minIndex = 0;
  int number;
  for(number = 0; number < numberOfRooms-1; ++number){
    int xxx = rooms[number].forConnect.x;
    if(xxx != x_pos){
      int distance = (xxx - x_pos) * (xxx - x_pos);
      if(minDistance >= distance){
        minIndex = number;
        minDistance = distance;
      }
    }
  }
  return minIndex;
}

/**
 * find the closest room in Y direction
 */
int findClosestRoomY(room_t room)
{
  point_t point = room.forConnect;
  int y_pos = point.y;
  int minDistance = 10000;
  int minIndex = 0;
  int number;
  for(number = 0; number < numberOfRooms-1; ++number){
    int yyy = rooms[number].forConnect.y;
    if(yyy != y_pos){
      int distance = (yyy - y_pos) * (yyy - y_pos);
      if(minDistance >= distance){
        minIndex = number;
        minDistance = distance;
      }
    }
  }
  return minIndex;
}
/**
 * random a point in a room
 */
point_t randomPoint(room_t r)
{
  point_t p;
  p.x = rand() % (r.width) + r.x;
  p.y = rand() % (r.height) + r.y;
  return p;
}

/**
 * connect two rooms with corridors
 */
void connectTwoRooms(room_t start, room_t end)
{
  int i, j, x_start, x_end, y_start, y_end;
  int randomD;
  randomD = rand() % 1;
  x_start = start.forConnect.x;
  x_end = end.forConnect.x;
  y_start = start.forConnect.y;
  y_end = end.forConnect.y;
  if(randomD == 0){
    if(x_start - x_end < 0){
      for(i = x_start; i < x_end; ++i){
        if(dungeon[y_start][i] == '#') return;
        if(dungeon[y_start][i] == ' ') dungeon[y_start][i] = '#';
      }
    }
    else{
      for(i = x_start; i > x_end; --i){
        if(dungeon[y_start][i] == '#') return;
        if(dungeon[y_start][i] == ' ') dungeon[y_start][i] = '#';
      }
    }
    if(y_start - y_end < 0){
      for(j = y_start; j < y_end; ++j){
        if(dungeon[j][x_end] == '#') return;
        if(dungeon[j][x_end] == ' ') dungeon[j][x_end] = '#';
      }
    }
    else{
      for(j = y_start; j > y_end; --j){
        if(dungeon[j][x_end] == '#') return;
        if(dungeon[j][x_end] == ' ') dungeon[j][x_end] = '#';
      }
    }
  }
  else{
    if(y_start - y_end < 0){
      for(j = y_start; j < y_end; ++j){
        if(dungeon[j][x_end] == '#') return;
        if(dungeon[j][x_end] == ' ') dungeon[j][x_end] = '#';
      }
    }
    else{
      for(j = y_start; j > y_end; --j){
        if(dungeon[j][x_end] == '#') return;
        if(dungeon[j][x_end] == ' ') dungeon[j][x_end] = '#';
      }
    }
    if(x_start - x_end < 0){
      for(i = x_start; i < x_end; ++i){
        if(dungeon[y_start][i] == '#') return;
        if(dungeon[y_start][i] == ' ') dungeon[y_start][i] = '#';
      }
    }
    else{
      for(i = x_start; i > x_end; --i){
        if(dungeon[y_start][i] == '#') return;
        if(dungeon[y_start][i] == ' ') dungeon[y_start][i] = '#';
      }
    }
  }
}

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
  int i, j, check;
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
    ++numberOfRooms;
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
  temp.x = rand() % 160 + 1;
  temp.y = rand() % 105 + 1;
  temp.width = rand() % (50 - 7) + 7;
  temp.height = rand() % (26 - 5) + 5;
  temp.isCreated = 1;
  temp.forConnect = randomPoint(temp);
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
  x_2 = x_1 + add.width;
  y_2 = y_1 + add.height;
  if(x_2 >= 159 || y_2 >= 104){
    return;
  }
  else{
    addRoomsToDungeon(add);
  }
}

/**
 * count the number of neighbors of a point
 */
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
