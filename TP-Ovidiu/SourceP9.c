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

typedef struct QueuePoints{
    point nod; 
    struct QueuePoints* parent; // here will be a trace of the parent of each node
}QueuePoints;

typedef struct Maze{
    unsigned int _high, _weight;
    unsigned int **Cost;
    unsigned int **Walls;
    unsigned int **Checked;
    unsigned int _size;
    unsigned int _curentPozQueue;
    point initial, final;
    QueuePoints *queue;
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
void linkList(); // This function will build a queue for nods
void DispPath(); // Show/Write final path
void Dispose();
void Revers(QueuePoints*);
// variales definitions area
Maze maze;
FILE *out;
// main program area

int main(){
    ReadFromFile(); //read input data

    _inMaze_FillMap(maze.Cost, INF);// Every cost in costmap become INF
    _inMaze_FillMap(maze.Checked, 0); // every checked square in Chekedmap become 0 (unchecked)

    maze.queue = (QueuePoints*) malloc(sizeof(QueuePoints) * maze._size); // alloc memory for list

    linkList(); // add all nodes in a linked list 
    buildCost(); // calculate the cost matrix;

    ///DispPath();

    fopen_s(&out, outputFile, "wt");
    fprintf_s(out, "%d\n", maze.Cost[maze.queue[maze._curentPozQueue].nod.y][maze.queue[maze._curentPozQueue].nod.y]);
    Revers(&maze.queue[maze._curentPozQueue]);
    fclose(out);
    Dispose();
    return 0;
}

// function definition
void linkList(){
    unsigned int i = 0, j = 0, k = 0;
    for(i = 0; i < maze._high; i++)
    {
        for(j = 0; j < maze._weight; j++)
        {
            maze.queue[k].nod.x = j;
            maze.queue[k].nod.y = i;
            maze.queue[k].parent = (k == 0) ? &maze.queue[k] : &maze.queue[k-1];
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
    maze.Cost[maze.initial.x][maze.initial.y] = maze.Walls[maze.initial.x][maze.initial.y]; // set the cost of the first node to 0

    ChangePriority(maze.initial.y, maze.initial.x); // change priority of a nod in queue list

    while(maze._curentPozQueue != maze._size)
    {
        if(maze.queue[maze._curentPozQueue].nod.x == maze.final.x && maze.queue[maze._curentPozQueue].nod.y == maze.final.y)
            break;
        for(temp = 0; temp < 8;){
            down = step[temp++];
            right = step[temp++];
            if(maze.queue[maze._curentPozQueue].nod.y + down >= 0 && maze.queue[maze._curentPozQueue].nod.y + down < maze._high)
                if(maze.queue[maze._curentPozQueue].nod.x + right >= 0 && maze.queue[maze._curentPozQueue].nod.x + right < maze._weight)
                    if(maze.Checked[maze.queue[maze._curentPozQueue].nod.y + down][maze.queue[maze._curentPozQueue].nod.x + right] == 0)
                        if(maze.Cost[maze.queue[maze._curentPozQueue].nod.y + down][maze.queue[maze._curentPozQueue].nod.x + right] >(maze.Cost[maze.queue[maze._curentPozQueue].nod.y][maze.queue[maze._curentPozQueue].nod.x] + maze.Walls[maze.queue[maze._curentPozQueue].nod.y + down][maze.queue[maze._curentPozQueue].nod.x + right]))
                        {
                            maze.Cost[maze.queue[maze._curentPozQueue].nod.y + down][maze.queue[maze._curentPozQueue].nod.x + right] = maze.Cost[maze.queue[maze._curentPozQueue].nod.y][maze.queue[maze._curentPozQueue].nod.x] + maze.Walls[maze.queue[maze._curentPozQueue].nod.y + down][maze.queue[maze._curentPozQueue].nod.x + right];                           
                            ChangePriority(maze.queue[maze._curentPozQueue].nod.y + down, maze.queue[maze._curentPozQueue].nod.x + right);
                        }
        }
        maze.Checked[maze.queue[maze._curentPozQueue].nod.y][maze.queue[maze._curentPozQueue].nod.x] = 1;
        maze._curentPozQueue++;

    }
}

void ChangePriority(int y, int x){
    int i = maze._curentPozQueue;
    QueuePoints aux;
    while(maze.queue[i].nod.x != x || maze.queue[i].nod.y != y)
    {
        i++;
    }

    maze.queue[i].parent = &maze.queue[maze._curentPozQueue];
    aux = maze.queue[i];

    while(i-- > 0 && (maze.Cost[maze.queue[i].nod.y][maze.queue[i].nod.x] > maze.Cost[aux.nod.y][aux.nod.x]))
    {
        maze.queue[i + 1] = maze.queue[i];
    }
    maze.queue[i + 1] = aux;
}

void Revers(QueuePoints *temp){
    if(temp->nod.x != maze.initial.x || temp->nod.y != maze.initial.y){
        Revers(temp->parent);
    }
    fprintf_s(out, "(%d %d)", temp->nod.y, temp->nod.x);
}
/*
void DispPath(){
   // uint k;
    QueuePoints* temp;
    
    //Here you can see the cost matrix
    //for(uint i = 0; i < maze._high; i++)
    //{
    //    for(uint j = 0; j < maze._weight; j++)
    //    {
    //        printf_s("%d ", maze.Cost[i][j]);
    //    }
    //    printf_s("\n");
    //}
  ///  printf_s("\n"); printf_s("\n"); printf_s("\n");
    for(uint i = 0; i < maze._size; i++)
    {
    ///    printf_s("(%d %d)", maze.queue[i].nod.y, maze.queue[i].nod.x);
        if(maze.queue[i].nod.x == maze.final.x && maze.queue[i].nod.y == maze.final.y){
            k = i;
        }
    }
   /// printf_s("\n\n\n");

    temp = &maze.queue[maze._curentPozQueue];
    fopen_s(&out, outputFile, "wt");
   /* do
    {
        fprintf_s(out, "(%d %d)", temp->nod.y, temp->nod.x);
        temp = temp->parent;
    } while(temp->nod.x != maze.initial.x || temp->nod.y != maze.initial.y);
    *
    Revers(temp);
    fclose(out);

}*/

// Aici nu mai stiu daca e bine ce am facut, Dar ce stiu e ca e inutil, memoria alocat se va imprastia la finalul executiei
void Dispose(){
    free(maze.queue);
    free(maze.Checked[0]);
    free(maze.Checked);
    free(maze.Cost);
    free(maze.Walls);
}