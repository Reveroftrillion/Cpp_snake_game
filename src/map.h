#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <vector>
#include <algorithm>
#include "block.h"          // 공통 오브젝트·상수 정의

using namespace std;

/*──────────────────────────────
   MAP CLASS  –  전체 필드와 로직
──────────────────────────────*/
struct MapDimensions {
    int height, width;
};

class Map {
public:
    /* ── 필드 ── */
    MapDimensions mapSize;                 // 맵 가로·세로
    SnakeHead     snakeHeadObject;         // 스네이크 머리 + 몸통
    vector<ImmunedWall> immuneWalls;       // 모서리 면역 벽
    vector<Wall>        regularWalls;      // 일반 벽
    vector<Gate>        gameGates = vector<Gate>(2);

    GrowthItem  growthItemObject;
    PoisonItem  poisonItemObject;
    TimeItem    timeItemObject;
    ShieldItem  shieldItemObject;          // 🟠 보호막 아이템

    /* ── 생성자 ── */
    Map(int h = 21, int w = 21, int wallCnt = 0);
    Map(const Map &m);                     // 깊은 복사

    /* ── 메서드 ── */
    void spawnShieldItem();                // 보호막 스폰
    void print_map();                      // (터미널 디버그용) 
};

/*──────────────────────────────
   IMPLEMENTATION
──────────────────────────────*/
inline Map::Map(int H, int W, int wallCnt)
{
    mapSize = { H, W };

    /* 1) 테두리 벽 배치 ── 삼항 → if/else 로 변경 */
    for (int r = 1; r <= H; ++r) {
        for (int c = 1; c <= W; ++c) {

            if (r == 1 || r == H) {                 // 위·아래 줄
                if (c == 1 || c == W) {             // 네 모서리 = 면역 벽
                    immuneWalls.emplace_back(r, c);
                } else {                            // 상·하 경계 = 일반 벽
                    int pos = (r == 1 ? 1 : 4);     // Up=1, Down=4
                    regularWalls.emplace_back(r, c, pos);
                }
            }
            else if (c == 1 || c == W) {            // 좌·우 경계
                int pos = (c == 1 ? 2 : 3);         // Left=2, Right=3
                regularWalls.emplace_back(r, c, pos);
            }

        }
    }

    /* 2) 스네이크 초기화 (길이 4) */
    snakeHeadObject = SnakeHead(H / 2, W / 2);
    for (int k = 1; k <= 3; ++k)
        snakeHeadObject.snakeBodySegments.emplace_back(H / 2 + k, W / 2);

    /* 3) 내부 랜덤 벽 */
    while (wallCnt--) {
        int r = rand() % H + 1, c = rand() % W + 1;
        int len = rand() % 6 + 4, dir = rand() % 4 + 1;   // 1U 2L 3R 4D
        while (len--) {
            if (r >= 1 && r < H && c >= 1 && c < W) {
                Coord p{ r, c };
                bool onSnake = (p == snakeHeadObject.coord);
                for (auto &seg : snakeHeadObject.snakeBodySegments)
                    onSnake |= (p == seg.coord);
                if (!onSnake) regularWalls.emplace_back(r, c);
            }
            switch (dir) { case 1: --r; break; case 2: --c; break;
                           case 3: ++c; break; case 4: ++r; break; }
        }
    }
}

/* 복사 생성자 – STL 컨테이너가 알아서 깊은 복사 */
inline Map::Map(const Map &m) = default;

/*──────────────────────────────
   🟠 보호막 아이템 스폰
──────────────────────────────*/
inline void Map::spawnShieldItem()
{
    /* 이미 맵 위에 존재하면 건너뜀 */
    if (shieldItemObject.coord.row != 0) return;

    while (true) {
        int r = rand() % mapSize.height + 1;
        int c = rand() % mapSize.width  + 1;
        Coord p{ r, c };

        /* ① 스네이크, ② 벽, ③ 다른 아이템과 겹치는지 검사 */
        bool bad = (p == snakeHeadObject.coord);
        for (auto &seg : snakeHeadObject.snakeBodySegments) bad |= (p == seg.coord);
        for (auto &w : regularWalls)    bad |= (p == w.coord);
        for (auto &w : immuneWalls)     bad |= (p == w.coord);
        if (p == growthItemObject.coord ||
            p == poisonItemObject.coord ||
            p == timeItemObject.coord)
            bad = true;

        if (!bad) {                     // 빈 칸이면 배치 성공
            shieldItemObject = ShieldItem(r, c);
            break;
        }
    }
}

/*──────────────────────────────
   (선택) 맵 출력 – 디버그용
──────────────────────────────*/
inline void Map::print_map()
{
    for (int r = 1; r <= mapSize.height; ++r) {
        for (int c = 1; c <= mapSize.width; ++c) {
            char ch = ' ';
            Coord p{ r, c };

            if (p == snakeHeadObject.coord)            ch = 'H';
            else {
                bool printed = false;
                for (auto &seg : snakeHeadObject.snakeBodySegments)
                    if (p == seg.coord) { ch = 'o'; printed = true; break; }
                if (!printed) {
                    for (auto &w : immuneWalls)  if (p == w.coord) { ch = '#'; break; }
                    for (auto &w : regularWalls) if (p == w.coord) { ch = '#'; break; }
                    if (p == growthItemObject.coord)   ch = '+';
                    if (p == poisonItemObject.coord)   ch = '-';
                    if (p == timeItemObject.coord)     ch = 'T';
                    if (p == shieldItemObject.coord)   ch = 'S';
                }
            }
            std::cout << ch;
        }
        std::cout << '\n';
    }
}

#endif  /* MAP_H */
