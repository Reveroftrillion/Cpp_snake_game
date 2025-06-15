#ifndef BLOCK_H
#define BLOCK_H

#include <iostream>
#include <vector>
using namespace std;

/*────────────────────────  공통 상수 · ENUM  ────────────────────────*/
/*  objectType (게임 오브젝트 식별용)  */
constexpr int OBJ_VOID         = 0;
constexpr int OBJ_WALL         = 1;
constexpr int OBJ_IMMUNE_WALL  = -1;   // 모서리 면역벽
constexpr int OBJ_GATE         = 2;
constexpr int OBJ_SNAKE_HEAD   = 3;
constexpr int OBJ_SNAKE_BODY   = 4;
constexpr int OBJ_GROWTH       = 5;
constexpr int OBJ_POISON       = -5;
constexpr int OBJ_TIME         = 6;
constexpr int OBJ_SHIELD       = 7;    // 🟠 보호막 아이템  ← 신규
constexpr int OBJ_RANDOM  = 8;          // ★ 랜덤 아이템
/*  색상 팔레트 번호(ncurses) – 필요 시 자유롭게 변경  */
constexpr int COL_SHIELD       = 9;
constexpr int COL_RANDOM  = 10;      
/*────────────────────────────────────────────────────────────────────*/

struct Coord
{
    int row, col;                      // (x, y) → (row, col) 로 변경
    bool operator==(const Coord &o) { return row == o.row && col == o.col; }
};

/*────────────────────────  Block 계층  ──────────────────────────────*/
class Block
{
public:
    Coord coord;
    int objectType = OBJ_VOID;

    virtual int getObjectType() { return objectType; }

    Block() : coord{0, 0} {}
    Block(int r, int c) : coord{r, c} {}
    Block(const Block &b) : coord{b.coord} {}
};

/*─── 벽류 ───────────────────────────────────────────────────────────*/
class Wall : public Block
{
public:
    int wallPositionType;              // Up=1, Right=2, Left=3, Down=4

    Wall() : Block()                   { objectType = OBJ_WALL; }
    Wall(int r, int c, int pos=-1)
        : Block(r, c), wallPositionType(pos)
    { objectType = OBJ_WALL; }

    int getObjectType() override { return objectType; }
};

class ImmunedWall : public Block
{
public:
    ImmunedWall() : Block()            { objectType = OBJ_GATE; }   // 유지 (기존 코드와 동일)
    ImmunedWall(int r, int c) : Block(r, c)
    { objectType = OBJ_GATE; }

    int getObjectType() override { return objectType; }
};

/*─── 아이템류 ───────────────────────────────────────────────────────*/
class GrowthItem  : public Block { public:
    GrowthItem() : Block(){ objectType = OBJ_GROWTH; }
    GrowthItem(int r,int c):Block(r,c){ objectType = OBJ_GROWTH; }
    int getObjectType() override { return objectType; }
};

class PoisonItem  : public Block { public:
    PoisonItem() : Block(){ objectType = OBJ_POISON; }
    PoisonItem(int r,int c):Block(r,c){ objectType = OBJ_POISON; }
    int getObjectType() override { return objectType; }
};

class TimeItem    : public Block { public:
    TimeItem() : Block(){ objectType = OBJ_TIME; }
    TimeItem(int r,int c):Block(r,c){ objectType = OBJ_TIME; }
    int getObjectType() override { return objectType; }
};

/* 🟠 보호막(Shield) 아이템 – 40 tick 무적 */
class ShieldItem  : public Block {
public:
    ShieldItem() : Block()              { objectType = OBJ_SHIELD; }
    ShieldItem(int r,int c):Block(r,c)  { objectType = OBJ_SHIELD; }
    int getObjectType() override { return objectType; }
};
class RandomItem : public Block {
public:
    RandomItem()             : Block(-1,-1){ objectType = OBJ_RANDOM; }
    RandomItem(int r,int c)  : Block(r, c){ objectType = OBJ_RANDOM; }
    int getObjectType() override { return objectType; }
};
/*─── 게이트 ─────────────────────────────────────────────────────────*/
class Gate : public Block
{
public:
    int  exitDirection;                 // 1=Up,2=Left,3=Right,4=Down (wallPositionType 이용)
    bool isActive = false;

    Gate() : Block() { objectType = OBJ_GATE; }

    Gate(const Wall &w)
    {
        coord         = w.coord;
        exitDirection = 5 - w.wallPositionType;      // 반대쪽
        objectType    = OBJ_GATE;
    }
    Gate(int r,int c):Block(r,c)        { objectType = OBJ_GATE; }

    int getObjectType() override { return objectType; }
};

/*─── 스네이크 ───────────────────────────────────────────────────────*/
class SnakeHead;                         // forward

class SnakeBody : public Block
{
public:
    SnakeBody() : Block()               { objectType = OBJ_SNAKE_BODY; }
    SnakeBody(int r,int c):Block(r,c)   { objectType = OBJ_SNAKE_BODY; }
    SnakeBody(const SnakeHead &head);    // 정의는 아래 inline

    int getObjectType() override { return objectType; }
};

class SnakeHead : public Block
{
public:
    vector<SnakeBody> snakeBodySegments;
    int currentDirection = -1;           // 1=Up,2=Left,3=Right,4=Down

    SnakeHead() : Block()               { objectType = OBJ_SNAKE_HEAD; }
    SnakeHead(int r,int c):Block(r,c)   { objectType = OBJ_SNAKE_HEAD; }

    int getObjectType() override { return objectType; }

    void move() {
        switch (currentDirection) {
            case 1: coord.row--; break;        // Up
            case 2: coord.col--; break;        // Left
            case 3: coord.col++; break;        // Right
            case 4: coord.row++; break;        // Down
        }
    }
};

inline SnakeBody::SnakeBody(const SnakeHead &head)
    : Block(head.coord.row, head.coord.col)
{
    objectType = OBJ_SNAKE_BODY;
}

#endif  // BLOCK_H
