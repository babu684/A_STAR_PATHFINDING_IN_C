#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>
#define ROWS 8
#define COLS 8
#define MAX_NODES ROWS * COLS

typedef struct {
    int x, y, g, h, f, parentX, parentY;                        
} Node;

typedef struct {
    int x, y, f; 
} OpenListNode;

int grid[ROWS][COLS];

#pragma region Helper_Funcs
bool isValid(int row, int col) 
{
    return (row >= 0) && (row < ROWS) && (col >= 0) && (col < COLS) && (grid[row][col] == 0);
}

int calculateH(int row, int col, int destRow, int destCol) 
{
    return abs(row - destRow) + abs(col - destCol);
}

void printPath(Node nodeDetails[ROWS][COLS], int destRow, int destCol) 
{
    char displayGrid[ROWS][COLS];
    for(int i = 0; i < ROWS; i++)
    {
        for(int j = 0; j < COLS; j++)
        {
            if(grid[i][j] == 0)     displayGrid[i][j] = '*';
            else                    displayGrid[i][j] = '#';
        }
    }
    int row = destRow;
    int col = destCol;
    int pathX[MAX_NODES];
    int pathY[MAX_NODES];
    int steps = 0;
    while (!(nodeDetails[row][col].parentX == row && nodeDetails[row][col].parentY == col)) 
    {
        displayGrid[row][col] = '.';
        pathX[steps] = row;
        pathY[steps] = col;
        steps++;
        int tempRow = nodeDetails[row][col].parentX;
        int tempCol = nodeDetails[row][col].parentY;
        row = tempRow;
        col = tempCol;
    }
    pathX[steps] = row;
    pathY[steps] = col;

    printf("Total Cost: %d\n\n", steps);
    for (int i = steps; i >= 0; i--) 
    {
        printf("(%d, %d)", pathX[i], pathY[i]);
        if(i > 0)   printf(" -> ");
    }
    printf("\n\n");

    for(int i = 0; i < ROWS; i++)
    {
        for(int j = 0; j < COLS; j++)
        {
            printf("%c ", displayGrid[i][j]);
        }
        printf("\n");
    }
    printf("YAYYY WE REACHED");
}

void loadGridFromFile(const char *filename) 
{
    FILE *file = fopen(filename, "r");
    if (file == NULL) 
    {
        printf("Error: Could not open file %s\n", filename);
        exit(1);
    }
    for (int i = 0; i < ROWS; i++) 
    {
        for (int j = 0; j < COLS; j++) 
        {
            fscanf(file, "%d", &grid[i][j]);
        }
    }
    fclose(file);
    printf("Map loaded successfully\n");
}
#pragma endregion Helper_Funcs

void aStarSearch(int startRow, int startCol, int destRow, int destCol) 
{
    if (!isValid(startRow, startCol) || !isValid(destRow, destCol)) 
    {
        return;
    }

    #pragma region SHIT_TON_OF_SETUP
    bool closedList[ROWS][COLS];
    Node nodeDetails[ROWS][COLS];
    for (int i = 0; i < ROWS; i++) 
    {
        for (int j = 0; j < COLS; j++) 
        {
            closedList[i][j] = false;
            nodeDetails[i][j].f = INT_MAX; 
            nodeDetails[i][j].g = INT_MAX;
            nodeDetails[i][j].h = INT_MAX;
            nodeDetails[i][j].parentX = -1;
            nodeDetails[i][j].parentY = -1;
        }
    }
    int i = startRow, j = startCol;
    nodeDetails[i][j].f = 0;
    nodeDetails[i][j].g = 0;
    nodeDetails[i][j].h = 0;
    nodeDetails[i][j].parentX = i;
    nodeDetails[i][j].parentY = j;
    OpenListNode openList[MAX_NODES];
    int openListCount = 0;
    openList[openListCount].x = i;
    openList[openListCount].y = j;
    openList[openListCount].f = 0;
    openListCount++;
    bool foundDest = false;
    #pragma endregion SHIT_TON_OF_SETUP

    while (openListCount > 0) 
    {
        int minIndex = 0;
        int minF = openList[0].f;
        
        for(int k = 1; k < openListCount; k++) 
        {
            if(openList[k].f < minF) 
            {
                minF = openList[k].f;
                minIndex = k;
            }
        }

        int pX = openList[minIndex].x;
        int pY = openList[minIndex].y;

        openList[minIndex] = openList[openListCount - 1];
        openListCount--;
        closedList[pX][pY] = true;

        int adj[] = {-1, 1, 0, 0};

        for (int k = 0; k < 4; k++) 
        {
            int newX = pX + adj[k];
            int newY = pY + adj[3 - k];

            if (isValid(newX, newY)) 
            {
                
                if (newX == destRow && newY == destCol) 
                {
                    nodeDetails[newX][newY].parentX = pX;
                    nodeDetails[newX][newY].parentY = pY;
                    printPath(nodeDetails, destRow, destCol);
                    foundDest = true;
                    return;
                }
                
                if (closedList[newX][newY] == false) 
                {
                    int gNew = nodeDetails[pX][pY].g + 1; 
                    int hNew = calculateH(newX, newY, destRow, destCol);
                    int fNew = gNew + hNew;

                    if (nodeDetails[newX][newY].f == INT_MAX || nodeDetails[newX][newY].f > fNew) 
                    {
                        #pragma region moving
                        nodeDetails[newX][newY].g = gNew;
                        nodeDetails[newX][newY].h = hNew;
                        nodeDetails[newX][newY].f = fNew;
                        nodeDetails[newX][newY].parentX = pX;
                        nodeDetails[newX][newY].parentY = pY;
                        openList[openListCount].x = newX;
                        openList[openListCount].y = newY;
                        openList[openListCount].f = fNew;
                        openListCount++;
                        #pragma endregion moving
                    }
                }
            }
        }
    }

    if (!foundDest) printf("Failed to find the Destination Cell\n");
}

int main() 
{
    loadGridFromFile("map.txt");
    printf("\nGrid Layout (0=Walk, 1=Wall):\n");
    for(int i=0; i<ROWS; i++){
        for(int j=0; j<COLS; j++){
            printf("%d ", grid[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    aStarSearch(0, 0, 7, 7);
    return 0;
}