/**
 * @author Yuhan Xiao
 * @lastModifiedTime 5:15PM, Jan 18, 2017.
 */

#include <stdio.h>
/**
 * For function atoi()
 */
#include <stdlib.h>
/**
 * For functions strcmp() and strcpy()
 */
#include <string.h>
/**
 * For function usleep()
 */
#include <unistd.h>

/**
 * Structure stands for each cell
 */
struct point {
  int x;
  int y;
  char isAlive[4];
};

/**
 * Array of structure point, contains the alive cells at very beginning
 */
struct point alive[1920];

/**
 * First table to print and edit
 */
char table[24][80];

/**
 * Second table to print and edit
 */
char newTable[24][80];

/**
 * 'yes' if the first table is initialized
 */
char initial[4];

void execute(void);
void addToArray(struct point p);
void print(void);
void create(void);
void initialTable(void);
void initialNewTable(void);
int checkAlive(int a, int b);
int findNeighbors(int a, int b);

/**
 * Conway's Game of Life on an 80 * 24 torus. Space for each dead cell and 'O' for each live cell.
 * Read a set of cell coordinates from the command line as x and y pairs and initialized as live.
 */
int main(int argc, char *argv[])
{
  /**
   * check if user entered the set of cell coordinates
   */
  if (argc < 2) {
    fprintf(stderr, "Input at least one cell coordinates!\n");
    exit(1);
  }
  /**
   * check if user entered the set of cell coordinates
   */
  if(argc%2 == 0){
    fprintf(stderr, "Missing one Y coordinate!\n");
    exit(1);
  }
  /**
   * read the set
   */
  int x, y, i;
  for(i = 1; i < argc; i=i+2){
    x = atoi(argv[i]);
    y = atoi(argv[i+1]);
    /**
     * if the coordinates are out of range, make it back
     */
    if(x > 79) x = x - 79;
    if(y > 23) y = y - 23;
    struct point temp;
    temp.x = x;
    temp.y = y;
    strcpy(temp.isAlive, "yes");
    /**
     * add every cell coordinates to the alive array
     */
    addToArray(temp);
  }
  /**
   * set all cells to be dead in both tables
   */
  initialTable();
  initialNewTable();
  /**
   * set given cells alive in the first table
   */
  create();
  /**
   * stand for the running time
   */
  int Time = 0;
  /**
   * run as long as there is at least one live cell
   */
  while(1){
    /**
     * execute the game once. Find the next state and print it
     */
    execute();
    printf("Time: %d\n", Time);
    Time++;
    usleep(83333);
  }
  return 0;
}

/**
 * add a point to array called alive
 */
void addToArray(struct point p){
  int i;
  i = 0;
  /**
   * find the last index
   */
  while(strcmp(alive[i].isAlive, "yes") == 0){
      ++i;
  }
  /**
   * put the point at the last index
   */
  alive[i] = p;
}

/**
 * Play the game
 */
void execute(void)
{
  /**
   * print the table at beginning
   */
  print();
  int i, j;
  /**
   * if the first table is initialized, modifity it by checking alive cells in the second table
   */
  if(strcmp(initial, "yes") == 0){
    for(j = 0; j < 24; j++){
      for(i = 0; i < 80; i++){
	/**
	 * follow the rules
	 */
        if(newTable[j][i] == 'O'){
          if(findNeighbors(i, j) == 0 || findNeighbors(i, j) == 1){
            table[j][i] = ' ';
          }
          else if(findNeighbors(i, j) == 2 || findNeighbors(i, j) == 3){
            table[j][i] = 'O';
          }
          else if(findNeighbors(i, j) > 3){
            table[j][i] = ' ';
          }
        }
        else{
          if(findNeighbors(i, j) == 3){
            table[j][i] = 'O';
          }
        }
      }
    }
    /**
     * initialize the second table
     */
    initialNewTable();
    strcpy(initial, "no");
  }
  /**
   * if the first table is not initialized, modifity the second table by checking alive cells in the first table
   */
  else{
    for(j = 0; j < 24; j++){
      for(i = 0; i < 80; i++){
        if(table[j][i] == 'O'){
          if(findNeighbors(i, j) == 0 || findNeighbors(i, j) == 1){
            newTable[j][i] = ' ';
          }
          else if(findNeighbors(i, j) == 2 || findNeighbors(i, j) == 3){
            newTable[j][i] = 'O';
          }
          else if(findNeighbors(i, j) > 3){
            newTable[j][i] = ' ';
          }
        }
        else{
          if(findNeighbors(i, j) == 3){
            newTable[j][i] = 'O';
          }
        }
      }
    }
    /**
     * initialize the first table
     */
    initialTable();
    strcpy(initial, "yes");
  }
}

/**
* initialize the first table
*/
void initialTable(void)
{
  int i, j;
  for(j = 0; j < 24; j++){
    for(i = 0; i < 80; i++){
      table[j][i] = ' ';
    }
  }
}

/**
* initialize the second table
*/
void initialNewTable(void)
{
  int i, j;
  for(j = 0; j < 24; j++){
    for(i = 0; i < 80; i++){
      newTable[j][i] = ' ';
    }
  }
}

/**
 * Find the number of neibors of a cell
 * @param int a, b
 * @return int
 */
int findNeighbors(int a, int b)
{
  int  count = 0;
  /**
   * if the first table is initialized, use the second table to find neighors
   */
  if(strcmp(initial, "yes") == 0){
    /**
     * if the cell is not at the boundary
     */
    if(a != 0 && a != 79 && b != 0 && b != 23){
      if(newTable[b][a-1] == 'O') ++count;
      if(newTable[b][a+1] == 'O') ++count;
      if(newTable[b-1][a] == 'O') ++count;
      if(newTable[b+1][a] == 'O') ++count;
      if(newTable[b-1][a-1] == 'O') ++count;
      if(newTable[b+1][a-1] == 'O') ++count;
      if(newTable[b-1][a+1] == 'O') ++count;
      if(newTable[b+1][a+1] == 'O') ++count;
    }
    /**
     * if the cell is at the left bound
     */
    else if(a == 0 && b != 0 && b != 23){
      if(newTable[b][79] == 'O') ++count;
      if(newTable[b][a+1] == 'O') ++count;
      if(newTable[b-1][a] == 'O') ++count;
      if(newTable[b+1][a] == 'O') ++count;
      if(newTable[b-1][79] == 'O') ++count;
      if(newTable[b+1][79] == 'O') ++count;
      if(newTable[b-1][a+1] == 'O') ++count;
      if(newTable[b+1][a+1] == 'O') ++count;
    }
    /**
     * right bound
     */
    else if(a == 79 && b!= 0 &&b != 23){
      if(newTable[b][a-1] == 'O') ++count;
      if(newTable[b][0] == 'O') ++count;
      if(newTable[b-1][a] == 'O') ++count;
      if(newTable[b+1][a] == 'O') ++count;
      if(newTable[b-1][a-1] == 'O') ++count;
      if(newTable[b+1][a-1] == 'O') ++count;
      if(newTable[b-1][0] == 'O') ++count;
      if(newTable[b+1][0] == 'O') ++count;
    }
    /**
     * upper bound
     */
    else if(b == 0 && a != 0 && a != 79){
      if(newTable[b][a-1] == 'O') ++count;
      if(newTable[b][a+1] == 'O') ++count;
      if(newTable[23][a] == 'O') ++count;
      if(newTable[b+1][a] == 'O') ++count;
      if(newTable[23][a-1] == 'O') ++count;
      if(newTable[b+1][a-1] == 'O') ++count;
      if(newTable[23][a+1] == 'O') ++count;
      if(newTable[b+1][a+1] == 'O') ++count;
    }
    /**
     * lower bound
     */
    else if(b == 23 && a != 0 && a != 79){
      if(newTable[b][a-1] == 'O') ++count;
      if(newTable[b][a+1] == 'O') ++count;
      if(newTable[b-1][a] == 'O') ++count;
      if(newTable[0][a] == 'O') ++count;
      if(newTable[b-1][a-1] == 'O') ++count;
      if(newTable[0][a-1] == 'O') ++count;
      if(newTable[b-1][a+1] == 'O') ++count;
      if(newTable[0][a+1] == 'O') ++count;
    }
    /**
     * four special points
     */
    else if(a == 0 && b == 0){
      if(newTable[b][79] == 'O') ++count;
      if(newTable[b][a+1] == 'O') ++count;
      if(newTable[23][a] == 'O') ++count;
      if(newTable[b+1][a] == 'O') ++count;
      if(newTable[23][79] == 'O') ++count;
      if(newTable[b+1][79] == 'O') ++count;
      if(newTable[23][a+1] == 'O') ++count;
      if(newTable[b+1][a+1] == 'O') ++count;
    }
    else if(a == 0 && b == 23){
      if(newTable[b][79] == 'O') ++count;
      if(newTable[b][a+1] == 'O') ++count;
      if(newTable[b-1][a] == 'O') ++count;
      if(newTable[0][a] == 'O') ++count;
      if(newTable[b-1][79] == 'O') ++count;
      if(newTable[0][79] == 'O') ++count;
      if(newTable[b-1][a+1] == 'O') ++count;
      if(newTable[0][a+1] == 'O') ++count;
    }
    else if(a == 79 && b == 0){
      if(newTable[b][a-1] == 'O') ++count;
      if(newTable[b][0] == 'O') ++count;
      if(newTable[23][a] == 'O') ++count;
      if(newTable[b+1][a] == 'O') ++count;
      if(newTable[23][a-1] == 'O') ++count;
      if(newTable[b+1][a-1] == 'O') ++count;
      if(newTable[23][0] == 'O') ++count;
      if(newTable[b+1][0] == 'O') ++count;
    }
    else{
      if(newTable[b][a-1] == 'O') ++count;
      if(newTable[b][0] == 'O') ++count;
      if(newTable[b-1][a] == 'O') ++count;
      if(newTable[0][a] == 'O') ++count;
      if(newTable[b-1][a-1] == 'O') ++count;
      if(newTable[0][a-1] == 'O') ++count;
      if(newTable[b-1][0] == 'O') ++count;
      if(newTable[0][0] == 'O') ++count;
    }
  }
  /**
   * if the first table is not initialized, use it to find the neighbors
   */
  else{
    /**
     * same as above
     */
    if(a != 0 && a != 79 && b != 0 && b != 23){
      if(table[b][a-1] == 'O') ++count;
      if(table[b][a+1] == 'O') ++count;
      if(table[b-1][a] == 'O') ++count;
      if(table[b+1][a] == 'O') ++count;
      if(table[b-1][a-1] == 'O') ++count;
      if(table[b+1][a-1] == 'O') ++count;
      if(table[b-1][a+1] == 'O') ++count;
      if(table[b+1][a+1] == 'O') ++count;
    }
    else if(a == 0 && b != 0 && b != 23){
      if(table[b][79] == 'O') ++count;
      if(table[b][a+1] == 'O') ++count;
      if(table[b-1][a] == 'O') ++count;
      if(table[b+1][a] == 'O') ++count;
      if(table[b-1][79] == 'O') ++count;
      if(table[b+1][79] == 'O') ++count;
      if(table[b-1][a+1] == 'O') ++count;
      if(table[b+1][a+1] == 'O') ++count;
    }
    else if(a == 79 && b!= 0 &&b != 23){
      if(table[b][a-1] == 'O') ++count;
      if(table[b][0] == 'O') ++count;
      if(table[b-1][a] == 'O') ++count;
      if(table[b+1][a] == 'O') ++count;
      if(table[b-1][a-1] == 'O') ++count;
      if(table[b+1][a-1] == 'O') ++count;
      if(table[b-1][0] == 'O') ++count;
      if(table[b+1][0] == 'O') ++count;
    }
    else if(b == 0 && a != 0 && a != 79){
      if(table[b][a-1] == 'O') ++count;
      if(table[b][a+1] == 'O') ++count;
      if(table[23][a] == 'O') ++count;
      if(table[b+1][a] == 'O') ++count;
      if(table[23][a-1] == 'O') ++count;
      if(table[b+1][a-1] == 'O') ++count;
      if(table[23][a+1] == 'O') ++count;
      if(table[b+1][a+1] == 'O') ++count;
    }
    else if(b == 23 && a != 0 && a != 79){
      if(table[b][a-1] == 'O') ++count;
      if(table[b][a+1] == 'O') ++count;
      if(table[b-1][a] == 'O') ++count;
      if(table[0][a] == 'O') ++count;
      if(table[b-1][a-1] == 'O') ++count;
      if(table[0][a-1] == 'O') ++count;
      if(table[b-1][a+1] == 'O') ++count;
      if(table[0][a+1] == 'O') ++count;
    }
    else if(a == 0 && b == 0){
      if(table[b][79] == 'O') ++count;
      if(table[b][a+1] == 'O') ++count;
      if(table[23][a] == 'O') ++count;
      if(table[b+1][a] == 'O') ++count;
      if(table[23][79] == 'O') ++count;
      if(table[b+1][79] == 'O') ++count;
      if(table[23][a+1] == 'O') ++count;
      if(table[b+1][a+1] == 'O') ++count;
    }
    else if(a == 0 && b == 23){
      if(table[b][79] == 'O') ++count;
      if(table[b][a+1] == 'O') ++count;
      if(table[b-1][a] == 'O') ++count;
      if(table[0][a] == 'O') ++count;
      if(table[b-1][79] == 'O') ++count;
      if(table[0][79] == 'O') ++count;
      if(table[b-1][a+1] == 'O') ++count;
      if(table[0][a+1] == 'O') ++count;
    }
    else if(a == 79 && b == 0){
      if(table[b][a-1] == 'O') ++count;
      if(table[b][0] == 'O') ++count;
      if(table[23][a] == 'O') ++count;
      if(table[b+1][a] == 'O') ++count;
      if(table[23][a-1] == 'O') ++count;
      if(table[b+1][a-1] == 'O') ++count;
      if(table[23][0] == 'O') ++count;
      if(table[b+1][0] == 'O') ++count;
    }
    else{
      if(table[b][a-1] == 'O') ++count;
      if(table[b][0] == 'O') ++count;
      if(table[b-1][a] == 'O') ++count;
      if(table[0][a] == 'O') ++count;
      if(table[b-1][a-1] == 'O') ++count;
      if(table[0][a-1] == 'O') ++count;
      if(table[b-1][0] == 'O') ++count;
      if(table[0][0] == 'O') ++count;
    }
  }
  return count;
}

/**
 * Create the very first time table
 */
void create(void)
{
  int i, j;
  for(j = 0; j < 24; j++){
    for(i = 0; i < 80; i++){
      if(checkAlive(i, j) == 0) table[j][i] = 'O';
      else table[j][i] = ' ';
    }
  }
}

/**
 * check if the point is alive
 */
int checkAlive(int a, int b)
{
  /**
   * run into the alive array to check if it is alive
   */
  int i;
  for(i = 0; i < 1920; ++i){
    if(alive[i].x == a && alive[i].y == b){
      return strcmp(alive[i].isAlive, "yes");
    }
  }
  return 1;
}

/**
 * print the table
 */
void print(void)
{
  /**
   * if the first table is initialized, print the second table
   */
  if(strcmp(initial, "yes") == 0){
    int i, j;
    for(j = 0; j < 24; j++){
      for(i = 0; i < 80; i++){
        printf("%c ", newTable[j][i]);
      }
      printf("\n");
    }
  }
  /**
   * if the first table is not initialized, print it
   */
  else{
    int i, j;
    for(j = 0; j < 24; j++){
      for(i = 0; i < 80; i++){
        printf("%c ", table[j][i]);
      }
      printf("\n");
    }
  }
}
