#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <vector>
#include <algorithm>
#include "block.h" // Assuming block.h is already modified

using namespace std;

struct MapDimensions
{
    int height, width; // Renamed h, w
};

class Map
{
public:
    MapDimensions mapSize; // Renamed size
    SnakeHead snakeHeadObject; // Renamed head
    vector<ImmunedWall> immuneWalls; // Renamed iWall
    vector<Wall> regularWalls; // Renamed wall
    vector<Gate> gameGates = vector<Gate>(2); // Renamed gate
    GrowthItem growthItemObject; // Renamed gItem
    PoisonItem poisonItemObject; // Renamed pItem
    TimeItem timeItemObject; // Renamed tItem
    Map(int mapHeight = 21, int mapWidth = 21, int initialWallCount = 0); // Renamed h, w, wallCount
    Map(const Map &m);

    void print_map();
};

// void : 0, wall : 1, immune wall : -1, gate: 2, snake head: 3, snake body: 4

Map::Map(int mapHeight, int mapWidth, int initialWallCount) // Renamed h, w, wallCount
{
    mapSize.height = mapHeight; // Using mapSize.height
    mapSize.width = mapWidth;   // Using mapSize.width
    for (int i = 1; i <= mapHeight; i++) // Using mapHeight
    {
        for (int j = 1; j <= mapWidth; j++) // Using mapWidth
        {
            if (i == 1 || i == mapHeight) // Using mapHeight
            {
                if (j == 1 || j == mapWidth) // Using mapWidth
                    immuneWalls.push_back(ImmunedWall(i, j)); // Using immuneWalls
                else
                    regularWalls.push_back(Wall(i, j, (i == 1 ? 1 : 4))); // Using regularWalls
            }
            else
            {
                if (j == 1 || j == mapWidth) // Using mapWidth
                    regularWalls.push_back(Wall(i, j, (j == 1 ? 2 : 3))); // Using regularWalls
            }
        }
    }
    snakeHeadObject = SnakeHead(mapHeight / 2, mapWidth / 2); // Renamed head, using snakeHeadObject
    snakeHeadObject.snakeBodySegments.push_back(SnakeBody(mapHeight / 2 + 1, mapWidth / 2)); // Using snakeBodySegments
    snakeHeadObject.snakeBodySegments.push_back(SnakeBody(mapHeight / 2 + 2, mapWidth / 2)); // Using snakeBodySegments
    snakeHeadObject.snakeBodySegments.push_back(SnakeBody(mapHeight / 2 + 3, mapWidth / 2)); // Using snakeBodySegments
    while (initialWallCount--) // Using initialWallCount
    {
        int row = rand() % mapHeight + 1, col = rand() % mapWidth + 1, length = rand() % 6 + 4; // Renamed x, y, using mapHeight, mapWidth
        int direction = rand() % 4 + 1; // Up = 1, Down = 4, Left = 2, Right = 3
        while (length--)
        {
            if (row >= 1 && row < mapHeight && col >= 1 && col < mapWidth) // Using row, col, mapHeight, mapWidth
            {
                Coord tempCoord; // Renamed tmp
                tempCoord.row = row; // Using row
                tempCoord.col = col; // Using col
                if (snakeHeadObject.coord == tempCoord) // Using snakeHeadObject
                {
                    continue;
                }
                bool isOccupiedBySnake = false; // Renamed isMerged
                for (auto it = snakeHeadObject.snakeBodySegments.begin(); it != snakeHeadObject.snakeBodySegments.end(); it++) // Using snakeBodySegments
                {
                    if (it->coord == tempCoord)
                    {
                        isOccupiedBySnake = true;
                    }
                }
                if (isOccupiedBySnake)
                    continue;
                regularWalls.push_back(Wall(row, col)); // Using regularWalls
            }
            switch (direction)
            {
            case 1:
                row--; // Using row
                break;
            case 2:
                col--; // Using col
                break;
            case 3:
                col++; // Using col
                break;
            case 4:
                row++; // Using row
                break;
            }
        }
    }
}

Map::Map(const Map &m)
{
    mapSize = m.mapSize; // Using mapSize
    snakeHeadObject = m.snakeHeadObject; // Using snakeHeadObject
    immuneWalls = m.immuneWalls; // Using immuneWalls
    regularWalls = m.regularWalls; // Using regularWalls
    gameGates = m.gameGates; // Using gameGates
    growthItemObject = m.growthItemObject; // Using growthItemObject
    poisonItemObject = m.poisonItemObject; // Using poisonItemObject
    timeItemObject = m.timeItemObject; // Using timeItemObject
}

void Map::print_map()
{
    // Implementation for printing the map would go here
    // This function was empty in the original provided file, so no changes other than potential variable use within it.
}

#endif