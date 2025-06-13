#ifndef BLOCK_H
#define BLOCK_H
#include <iostream>
#include <vector>

using namespace std;

struct Coord
{
    int row, col; // Renamed from x, y

    bool operator==(const Coord &other)
    {
        return row == other.row && col == other.col;
    }
};

class Block
{

public:
    Coord coord;
    int objectType = 0; // Renamed from identity
    // void : 0, wall : 1, immune wall : -1, gate: 2, snake head: 3, snake body: 4, growth_item: 5, poison_item: -5
    virtual int getObjectType() { return objectType; } // Renamed from showIdentity
    Block()
    {
        coord.row = 0;
        coord.col = 0;
    }
    Block(int row, int col) // Renamed from x, y
    {
        coord.row = row;
        coord.col = col;
    }
    Block(const Block &b)
    {
        coord = b.coord;
    }
};

class Wall : public Block
{
public:
    int wallPositionType; // Renamed from position
    Wall() : Block() { objectType = 1; }
    Wall(int row, int col, int wallPositionType = -1) : Block(row, col) // Renamed from x, y, position
    {
        objectType = 1;
        this->wallPositionType = wallPositionType;
    }
    int getObjectType() { return objectType; } // Renamed from showIdentity

    /* Up = 1, Down = 4, Left = 3, Right = 2 */
};

class ImmunedWall : public Block
{
public:
    ImmunedWall() : Block() { objectType = 2; }
    ImmunedWall(int row, int col) : Block(row, col) { objectType = 2; } // Renamed from x, y
    ImmunedWall(const ImmunedWall &iwall)
    {
        coord = iwall.coord;
    }
    int getObjectType() { return objectType; } // Renamed from showIdentity
};

class GrowthItem : public Block
{
public:
    GrowthItem() : Block() { objectType = 5; }
    GrowthItem(int row, int col) : Block(row, col) { objectType = 5; } // Renamed from x, y
    GrowthItem(const GrowthItem &gitem)
    {
        coord = gitem.coord;
    }
    int getObjectType() { return objectType; } // Renamed from showIdentity
};

class PoisonItem : public Block
{
public:
    PoisonItem() : Block() { objectType = -5; }
    PoisonItem(int row, int col) : Block(row, col) { objectType = -5; } // Renamed from x, y
    PoisonItem(const PoisonItem &pitem)
    {
        coord = pitem.coord;
    }
    int getObjectType() { return objectType; } // Renamed from showIdentity
};

class TimeItem : public Block
{
public:
    TimeItem() : Block() { objectType = 6; }
    TimeItem(int row, int col) : Block(row, col) { objectType = 6; } // Renamed from x, y
    TimeItem(const TimeItem &titem)
    {
        coord = titem.coord;
    }
    int getObjectType() { return objectType; } // Renamed from showIdentity
};

class Gate : public Block
{
public:
    int exitDirection; // Renamed from possible
    Gate() : Block() { objectType = 2; }
    Gate(const Wall &wall)
    {
        coord = wall.coord;
        exitDirection = 5 - wall.wallPositionType; // Using wallPositionType
    }
    Gate(int row, int col) : Block(row, col) { objectType = 2; } // Renamed from x, y
    bool isActive = false;
    int getObjectType() { return objectType; } // Renamed from showIdentity
};

class SnakeHead; // Forward declaration

class SnakeBody : public Block
{
public:
    SnakeBody() : Block() { objectType = 4; }
    SnakeBody(int row, int col) : Block(row, col) { objectType = 4; } // Renamed from x, y
    SnakeBody(const SnakeHead &head);
    int getObjectType() { return objectType; } // Renamed from showIdentity
};

class SnakeHead : public Block
{
public:
    vector<SnakeBody> snakeBodySegments = vector<SnakeBody>(); // Renamed from body
    int currentDirection = -1;                                // Renamed from direction
    friend SnakeBody;
    SnakeHead() : Block() { objectType = 3; }
    SnakeHead(int row, int col) : Block(row, col) // Renamed from x, y
    {
        objectType = 3;
    }
    int getObjectType() { return objectType; } // Renamed from showIdentity
    void move()
    {
        switch (currentDirection) // Using currentDirection
        {
        case 1: // Up
            coord.row--;
            break;
        case 2: // Left
            coord.col--;
            break;
        case 3: // Right
            coord.col++;
            break;
        case 4: // Down
            coord.row++;
            break;
        }
    }
};
inline SnakeBody::SnakeBody(const SnakeHead &head)
  : Block(head.coord.row, head.coord.col)  // 머리 좌표로 몸통 초기화
{
    objectType = 4;  // snake body 타입 설정
}
#endif