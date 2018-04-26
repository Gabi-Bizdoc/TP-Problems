#include <stdio.h>
#include <stdlib.h>
#define INF 0xFFFFFFFF
#define inputFile "in.txt"
#define outputFile "out.txt"

// typedef definition area

typedef struct point{
    unsigned int x;
    unsigned int y;
}point;

typedef struct Maze{
    unsigned int _high, _weight;
    unsigned int **Cost;
    unsigned int **Walls;
    unsigned int **Checked;
    unsigned int _size;
    unsigned int _curentPozQueue;
    point initial, final;
    point *queue;
}Maze;

// containers functions area
typedef unsigned int uint;
uint* CreateArray(int length); // Return an unsigned int array with a specified size
uint** CreateMatrix(uint rows, uint columns); // Return a matrix with specified rows and columns
point* CreatePointsArray(int length);
void ChangePriority(int, int);

int step[] = {-1, 0, 1, 0, 0, -1, 0, 1};

// functions declaration area

void ReadFromFile();
void _inMaze_FillMap(uint **map, int value); // Fills a matrix from Maze with a specified value
void buildCost(); // Fills every cell with the minim cost from begin to that cell
void linkList();
void DispPath();

// variales definitions area
Maze maze;
// main program area

int main(){
    ReadFromFile(); //read input data

    _inMaze_FillMap(maze.Cost, INF);// Every cost in costmap become INF
    _inMaze_FillMap(maze.Checked, 0); // every checked square in Chekedmap become 0 (unchecked)

    maze.queue = (point*) malloc(sizeof(point) * maze._size); // alloc memory for M*N points

    linkList(); // add all nodes in a linked list 
    buildCost(); // calculate the cost matrix;

    DispPath();

    _getch();
    return 0;
}

// function definition
void linkList(){
    unsigned int i = 0, j = 0, k = 0;
    for(i = 0; i < maze._high; i++)
    {
        for(j = 0; j < maze._weight; j++)
        {
            maze.queue[k].x = j;
            maze.queue[k].y = i;
            k++;
        }
    }
}


uint* CreateArray(int length){
    uint *arr;
    arr = (uint *) malloc(sizeof(uint) * length);
    return arr;
}
point* CreatePointsArray(int length){
    point *arr;
    arr = (point *) malloc(sizeof(point) * length);
    return arr;
}

uint** CreateMatrix(uint rows, uint columns){
    //  int r = 3, c = 4;
    uint **arr;
    uint i;

    arr = (uint **) malloc(sizeof(uint *) * rows);
    arr[0] = (uint *) malloc(sizeof(uint) * columns * rows);

    for(i = 0; i < rows; i++)
        arr[i] = (*arr + columns * i);
    // After this 'for' arr[i][j] is same as *(*(arr+i)+j)
    return arr;
}

void ReadFromFile(){
    unsigned int i, j;
    FILE *input;
    fopen_s(&input, inputFile, "rt");
    // read dimensions
    fscanf_s(input, "%u", &maze._high);
    fscanf_s(input, "%u", &maze._weight);

    maze.Walls = CreateMatrix(maze._high, maze._weight);
    maze.Cost = CreateMatrix(maze._high, maze._weight);
    maze.Checked = CreateMatrix(maze._high, maze._weight);

    // read initial and final point
    fscanf_s(input, "%u%u", &maze.initial.y, &maze.initial.x);
    fscanf_s(input, "%u%u", &maze.final.y, &maze.final.x);

    // read map
    for(i = 0; i < maze._high; i++){
        for(j = 0; j < maze._weight; j++){
            fscanf_s(input, "%u", &maze.Walls[i][j]);
        }
    }

    maze._size = maze._high * maze._weight;
    fclose(input);
}

void _inMaze_FillMap(uint **map, int value){
    uint i, j;

    for(i = 0; i < maze._high; i++)
        for(j = 0; j < maze._weight; j++)
            map[i][j] = value;

    map[maze.initial.y][maze.initial.x] = 0;
}

void buildCost(){
    int down, right, temp;
    maze.Checked[maze.initial.x][maze.initial.y] = 0; // set the cost of the first node to 0

    ChangePriority(maze.initial.y, maze.initial.x); // change priority of a nod in queue list

    while(maze._curentPozQueue != maze._size)
    {
        for(temp = 0; temp < 8;){
            down = step[temp++];
            right = step[temp++];
            if(maze.queue[maze._curentPozQueue].y + down >= 0 && maze.queue[maze._curentPozQueue].y + down < maze._high)
                if(maze.queue[maze._curentPozQueue].x + right >= 0 && maze.queue[maze._curentPozQueue].x + right < maze._weight)
                    if(maze.Checked[maze.queue[maze._curentPozQueue].y + down][maze.queue[maze._curentPozQueue].x + right] == 0)
                        if(maze.Cost[maze.queue[maze._curentPozQueue].y + down][maze.queue[maze._curentPozQueue].x + right] >(maze.Cost[maze.queue[maze._curentPozQueue].y][maze.queue[maze._curentPozQueue].x] + maze.Walls[maze.queue[maze._curentPozQueue].y + down][maze.queue[maze._curentPozQueue].x + right])){
                            maze.Cost[maze.queue[maze._curentPozQueue].y + down][maze.queue[maze._curentPozQueue].x + right] = maze.Cost[maze.queue[maze._curentPozQueue].y][maze.queue[maze._curentPozQueue].x] + maze.Walls[maze.queue[maze._curentPozQueue].y + down][maze.queue[maze._curentPozQueue].x + right];
                            ChangePriority(maze.queue[maze._curentPozQueue].y + down, maze.queue[maze._curentPozQueue].x + right);
                        }
        }
        maze.Checked[maze.queue[maze._curentPozQueue].y][maze.queue[maze._curentPozQueue].x] = 1;
        maze._curentPozQueue++;
    }
}

void ChangePriority(int y, int x){
    int i = maze._curentPozQueue;
    point aux;
    while(maze.queue[i].x != x || maze.queue[i].y != y)
    {
        i++;
    }

    aux = maze.queue[i];

    while(i-- > 0 && (maze.Cost[maze.queue[i].y][maze.queue[i].x] > maze.Cost[aux.y][aux.x]))
    {
        maze.queue[i + 1] = maze.queue[i];
    }

    maze.queue[i + 1] = aux;
}

int GetPoz(int y, int x){
    for(uint i = 0; i < maze._size; i++){
        if(maze.queue[i].x == x && maze.queue[i].y == y)
            return i;
    }
}

void DispPath(){
    uint k;
    for(uint i = 0; i < maze._high; i++)
    {
        for(uint j = 0; j < maze._weight; j++)
        {
            printf_s("%d ", maze.Cost[i][j]);
        }
        printf_s("\n");
    }
    printf_s("\n"); printf_s("\n"); printf_s("\n");

    for(uint i = 0; i < maze._size; i++)
    {
        printf_s("(%d %d)", maze.queue[i].y, maze.queue[i].x);
        if(maze.queue[i].x == maze.final.x && maze.queue[i].y == maze.final.y){
            k = i;
        }
    }
    //printf_s("\n\n\n");

}