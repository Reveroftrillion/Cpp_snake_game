#ifndef GAME_H
#define GAME_H

#include "map.h"
#include "block.h"
#include <iostream>
#include <vector>
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <cstring>

using namespace std;

/* ────────────── 보호막 전역 상수 ────────────── */
constexpr int SHIELD_DURATION    = 40;   // 무적 유지 tick
constexpr int SHIELD_SPAWN_CYCLE = 60;   // 스폰 주기
/* ───────────────────────────────────────────── */

class Game
{
public:
    Map   gameMap;

    int gateActiveDuration = 0;
    int growthItemCount    = 0;
    int poisonItemCount    = 0;
    int gatesUsedCount     = 0;
    int maxSnakeLength     = 3;
    int gameTimerSeconds   = 0;
    int gameSpeedDelay     = 200;
    float speedMultiplier  = 1;
    int speedBoostTimer    = 0;

    /* 미션 · 게임 상태 */
    char missionSnakeLengthStatus = ' ';
    char missionGrowthItemStatus  = ' ';
    char missionPoisonItemStatus  = ' ';
    char missionGateUseStatus     = ' ';
    string gameOverReason         = "";
    bool allMissionsCompleted     = false;

    /* 🟠 보호막 상태 */
    int shieldTick       = 0;     // 남은 무적 tick
    int shieldCount      = 0;     // 획득 횟수
    int shieldSpawnTimer = 0;     // 스폰 타이머
    int randomSpawnTimer = 0;
    /* ───────────────────────── ctor ───────────────────────── */
    Game()
    {
        gameMap = Map(21, 41, 2);
        initscr();
        clear();
        noecho();
        cbreak();
        curs_set(0);
        start_color();
        init_pair(1, COLOR_WHITE,   COLOR_WHITE);    // Wall
        init_pair(2, COLOR_BLACK,   COLOR_WHITE);    // Immuned Wall
        init_pair(3, COLOR_CYAN,    COLOR_CYAN);     // Snake Head
        init_pair(4, COLOR_GREEN,   COLOR_GREEN);    // Snake Body
        init_pair(5, COLOR_BLUE,    COLOR_BLUE);     // Growth Item
        init_pair(6, COLOR_RED,     COLOR_RED);      // Poison Item
        init_pair(7, COLOR_BLACK,   COLOR_MAGENTA);  // Gate
        init_pair(8, COLOR_YELLOW,  COLOR_YELLOW);   // Time Item
        init_pair(COL_SHIELD, COLOR_BLACK, COLOR_GREEN); // Shield
        init_pair(COL_RANDOM, COLOR_WHITE, COLOR_MAGENTA);

        keypad(stdscr, TRUE);
        nodelay(stdscr, TRUE);
        srand((unsigned)time(NULL));

        generateItems();
        generateGate();
    }
    ~Game() {}

    /* ───────────────────── 메인 루프 ───────────────────── */
    void refreshScreen()
    {
        int growthItemTimer = 0, poisonItemTimer = 0,
            gateSpawnTimer  = 0, timeItemTimer   = 0;

        bool gameStartedFlag = false;

        while (1)
        {
            if (gameStartedFlag)
            {
                growthItemTimer++;
                poisonItemTimer++;
                gateSpawnTimer++;
                timeItemTimer++;
                shieldSpawnTimer++;          // 🟠
                gameTimerSeconds++;
                randomSpawnTimer++;
            }

            clear();

            /* 창 생성 (Score 높이 11 로 확장) */
            WINDOW *board   = newwin(gameMap.mapSize.height + 2,
                                     gameMap.mapSize.width  + 2, 0, 0);
            WINDOW *score   = newwin(11, 27, 0,  gameMap.mapSize.width + 4);
            WINDOW *mission = newwin(9,  27, 10, gameMap.mapSize.width + 4);

            box(board, 0, 0);
            box(score, 0, 0);
            box(mission, 0, 0);

            /* ── Score Board ── */
            mvwprintw(score, 1, 1, "*******Score Board*******");
            mvwprintw(score, 3, 1, " B: %d/%d",
                    (int)gameMap.snakeHeadObject.snakeBodySegments.size(),
                    maxSnakeLength);
            mvwprintw(score, 4, 1, " +: %d", growthItemCount);
            mvwprintw(score, 5, 1, " -: %d", poisonItemCount);
            mvwprintw(score, 6, 1, " G: %d", gatesUsedCount);
            mvwprintw(score, 7, 1, " S: %d", shieldCount);          // 🟠

            /* ▼▼ 이 두 줄을 꼭 붙여 주세요 ▼▼ */
            if (shieldTick > 0)
                mvwprintw(score, 8, 1, " INV:%02d", shieldTick);    // 🟠

            mvwprintw(score, 9, 1, " time: %d",
                    gameTimerSeconds / (1000 / gameSpeedDelay));

            /* ── Mission Board ── */
            mvwprintw(mission, 1, 1, "******Mission Board******");
            mvwprintw(mission, 3, 1, " B: 7 / %d (%c) ",
                      (int)gameMap.snakeHeadObject.snakeBodySegments.size(),
                      missionSnakeLengthStatus);
            mvwprintw(mission, 4, 1, " +: 5 / %d (%c) ",
                      growthItemCount, missionGrowthItemStatus);
            mvwprintw(mission, 5, 1, " -: 2 / %d (%c) ",
                      poisonItemCount, missionPoisonItemStatus);
            mvwprintw(mission, 6, 1, " G: 1 / %d (%c) ",
                      gatesUsedCount, missionGateUseStatus);

            /* ── 보드 오브젝트 그리기 ── */
            for (auto &w : gameMap.immuneWalls) {
                wattron(board, COLOR_PAIR(2));
                mvwaddch(board, w.coord.row, w.coord.col, '+');
                wattroff(board, COLOR_PAIR(2));
            }
            for (auto &w : gameMap.regularWalls) {
                wattron(board, COLOR_PAIR(2));
                mvwaddch(board, w.coord.row, w.coord.col, ' ');
                wattroff(board, COLOR_PAIR(2));
            }
            for (auto &b : gameMap.snakeHeadObject.snakeBodySegments) {
                wattron(board, COLOR_PAIR(4));
                mvwaddch(board, b.coord.row, b.coord.col, ' ');
                wattroff(board, COLOR_PAIR(4));
            }
            for (auto &g : gameMap.gameGates) {
                wattron(board, COLOR_PAIR(7));
                mvwaddch(board, g.coord.row, g.coord.col, ' ');
                wattroff(board, COLOR_PAIR(7));
            }
            /* Snake Head */
            wattron(board, COLOR_PAIR(3));
            mvwaddch(board, gameMap.snakeHeadObject.coord.row,
                             gameMap.snakeHeadObject.coord.col, ' ');
            wattroff(board, COLOR_PAIR(3));
            /* Items */
            wattron(board, COLOR_PAIR(5));
            mvwaddch(board, gameMap.growthItemObject.coord.row,
                             gameMap.growthItemObject.coord.col, ' ');
            wattroff(board, COLOR_PAIR(5));

            wattron(board, COLOR_PAIR(6));
            mvwaddch(board, gameMap.poisonItemObject.coord.row,
                             gameMap.poisonItemObject.coord.col, ' ');
            wattroff(board, COLOR_PAIR(6));

            wattron(board, COLOR_PAIR(8));
            mvwaddch(board, gameMap.timeItemObject.coord.row,
                             gameMap.timeItemObject.coord.col, ' ');
            wattroff(board, COLOR_PAIR(8));

            /* 🟠 Shield Item 출력 */
            if (gameMap.shieldItemObject.coord.row != -1) {          // ★ sentinel 체크
                wattron(board, COLOR_PAIR(COL_SHIELD));
                mvwaddch(board,
                         gameMap.shieldItemObject.coord.row,
                         gameMap.shieldItemObject.coord.col, ' ');
                wattroff(board, COLOR_PAIR(COL_SHIELD));
            }

            if (gameMap.randomItemObject.coord.row != -1) {
                wattron(board, COLOR_PAIR(COL_RANDOM));
                mvwaddch(board,
                         gameMap.randomItemObject.coord.row,
                         gameMap.randomItemObject.coord.col, ' ');
                wattroff(board, COLOR_PAIR(COL_RANDOM));
            }

            refresh();
            wrefresh(board);
            wrefresh(score);
            wrefresh(mission);

            /* ── 입력 처리 ── */
            int key = getch();
            int previousDirection = gameMap.snakeHeadObject.currentDirection;

            switch (key)
            {
            case KEY_UP:    gameMap.snakeHeadObject.currentDirection = 1; gameStartedFlag = true; break;
            case KEY_DOWN:  gameMap.snakeHeadObject.currentDirection = 4; gameStartedFlag = true; break;
            case KEY_RIGHT: gameMap.snakeHeadObject.currentDirection = 3; gameStartedFlag = true; break;
            case KEY_LEFT:  gameMap.snakeHeadObject.currentDirection = 2; gameStartedFlag = true; break;
            }

            /* 아이템·Gate 스폰 주기 */
            if (growthItemTimer >= 50) { growthItemTimer = 0; generateGItem(); }
            if (poisonItemTimer >= 50) { poisonItemTimer = 0; generatePItem(); }
            if (timeItemTimer   >= 30) { timeItemTimer   = 0; generateTItem(); }

            if (shieldSpawnTimer >= SHIELD_SPAWN_CYCLE) {        // 🟠
                shieldSpawnTimer = 0;
                gameMap.spawnShieldItem();
            }

            if (randomSpawnTimer >= 70) {            // 주기 원하는 값
                randomSpawnTimer = 0;
                gameMap.spawnRandomItem();
            }

            if (speedBoostTimer > 0) { if (--speedBoostTimer == 0) speedMultiplier = 1; }

            if (gateSpawnTimer >= 80 &&
                !(gameMap.gameGates[0].isActive || gameMap.gameGates[1].isActive))
            {
                gateSpawnTimer = 0;
                generateGate();
            }
            if (allMissionsCompleted)
            {
                while (1)
                {
                    wclear(score);
                    box(score, 0, 0);
                    mvwprintw(score, 3, 2, "Press \'P\' to next stage.");
                    mvwprintw(score, 4, 2, "Press \'E\' to exit.");
                    wrefresh(score);
                    char key = getch();
                    if (key == 'e')
                        endwin();
                    if (key == 'p')
                    {
                        allMissionsCompleted = false;
                        gameMap = Map(21, 41, rand() % 4 + 2);
                        gateActiveDuration = 0;
                        growthItemCount = 0;
                        poisonItemCount = 0;
                        gatesUsedCount = 0;
                        maxSnakeLength = 3;
                        gameTimerSeconds = 0;
                        gameStartedFlag = false;
                        speedMultiplier = 1;

                        missionSnakeLengthStatus = ' ';
                        missionGrowthItemStatus = ' ';
                        missionPoisonItemStatus = ' ';
                        missionGateUseStatus = ' ';
                        generateItems();
                        generateGate();
                        gameSpeedDelay = ((rand() % 25) + 5) * 10;
                        break;
                    }
                    usleep(1000 * 100);
                }
            }
            if (!update(growthItemTimer, poisonItemTimer, timeItemTimer, previousDirection)) // Pass renamed local timers and previous direction
            {
                while (1)
                {
                    gameMap.snakeHeadObject.move(); // Changed gameMap.head.move() to gameMap.snakeHeadObject.move()
                    wclear(score);
                    box(score, 0, 0);
                    const char *_gameOverReason = gameOverReason.c_str(); // Use renamed gameOverReason
                    mvwprintw(score, 3, 2, _gameOverReason);
                    mvwprintw(score, 4, 2, "Press \'P\' to continue.");
                    mvwprintw(score, 5, 2, "Press \'E\' to exit");
                    wrefresh(score);
                    char key = getch();
                    if (key == 'e')
                    {
                        endwin();
                        return;
                    }
                    if (key == 'p')
                    {
                        gameMap.snakeHeadObject = SnakeHead(gameMap.mapSize.height / 2, gameMap.mapSize.width / 2); // Changed gameMap.head and gameMap.size.h/w
                        gameMap.snakeHeadObject.snakeBodySegments.clear(); // Changed gameMap.head.body.clear()
                        gameMap.snakeHeadObject.snakeBodySegments.push_back(SnakeBody(gameMap.mapSize.height / 2 + 1, gameMap.mapSize.width / 2)); // Changed gameMap.head.body.push_back() and gameMap.size.h/w
                        gameMap.snakeHeadObject.snakeBodySegments.push_back(SnakeBody(gameMap.mapSize.height / 2 + 2, gameMap.mapSize.width / 2)); // Changed gameMap.head.body.push_back() and gameMap.size.h/w
                        gameMap.snakeHeadObject.snakeBodySegments.push_back(SnakeBody(gameMap.mapSize.height / 2 + 3, gameMap.mapSize.width / 2)); // Changed gameMap.head.body.push_back() and gameMap.size.h/w
                        allMissionsCompleted = false;
                        gateActiveDuration = 0;
                        growthItemCount = 0;
                        poisonItemCount = 0;
                        gatesUsedCount = 0;
                        maxSnakeLength = 0;
                        gameTimerSeconds = 0;
                        gameStartedFlag = false;
                        speedMultiplier = 1;

                        missionSnakeLengthStatus = ' ';
                        missionGrowthItemStatus = ' ';
                        missionPoisonItemStatus = ' ';
                        missionGateUseStatus = ' ';
                        generateItems();
                        generateGate();
                        break;
                    }
                    usleep(1000 * 100);
                }
            }

            usleep(1000 * (int)((float)gameSpeedDelay / speedMultiplier));
        }
    }

    bool isValid(int previousDirection = 0) // Use renamed parameter
    {
        if (gameMap.snakeHeadObject.coord.row < 1 ||
            gameMap.snakeHeadObject.coord.row > gameMap.mapSize.height ||
            gameMap.snakeHeadObject.coord.col < 1 ||
            gameMap.snakeHeadObject.coord.col > gameMap.mapSize.width)
        {
            gameOverReason = "Out of bounds.";
            return false;
        }

        if (shieldTick > 0) return true;

        if (previousDirection == 5)
        {
            gameOverReason = "Tried moving in the opposite direction."; // Use renamed gameOverReason
            return false;
        }
        for (auto it = gameMap.regularWalls.begin(); it != gameMap.regularWalls.end(); it++) // Changed gameMap.wall to gameMap.regularWalls
        {
            if (it->coord == gameMap.snakeHeadObject.coord) // Changed gameMap.head.coord to gameMap.snakeHeadObject.coord
            {
                gameOverReason = "Collided with the wall.";
                return false;
            }
        }
        for (auto it = gameMap.snakeHeadObject.snakeBodySegments.begin(); it != gameMap.snakeHeadObject.snakeBodySegments.end(); it++) // Changed gameMap.head.body.begin() to gameMap.snakeHeadObject.snakeBodySegments.begin()
        {
            if (it->coord == gameMap.snakeHeadObject.coord) // Changed gameMap.head.coord to gameMap.snakeHeadObject.coord
            {
                gameOverReason = "Collided with the body.";
                return false;
            }
        }
        if (gameMap.snakeHeadObject.snakeBodySegments.size() < 3) // Changed gameMap.head.body.size() to gameMap.snakeHeadObject.snakeBodySegments.size()
        {
            gameOverReason = "Length is less than 3.";
            return false;
        }
        return true;
    }

    void generateRandCoord(int &row, int &col, bool shouldIncludeWall = false) // Renamed x, y to row, col
    {
        while (1)
        {
            row = rand() % (gameMap.mapSize.height - 1) + 2; // Changed gameMap.size.h to gameMap.mapSize.height
            col = rand() % (gameMap.mapSize.width - 1) + 2; // Changed gameMap.size.w to gameMap.mapSize.width
            Coord tmp;
            tmp.row = row; // Changed tmp.x to tmp.row
            tmp.col = col; // Changed tmp.y to tmp.col
            bool same = false;
            if (!shouldIncludeWall)
            {
                for (auto it = gameMap.regularWalls.begin(); it != gameMap.regularWalls.end(); it++) // Changed gameMap.wall to gameMap.regularWalls
                {
                    if (it->coord == tmp)
                        same = true;
                }
            }
            for (auto it = gameMap.snakeHeadObject.snakeBodySegments.begin(); it != gameMap.snakeHeadObject.snakeBodySegments.end(); it++) // Changed gameMap.head.body.begin() to gameMap.snakeHeadObject.snakeBodySegments.begin()
            {
                if (it->coord == tmp)
                    same = true;
            }
            for (auto it = gameMap.gameGates.begin(); it != gameMap.gameGates.end(); it++) // Changed gameMap.gate to gameMap.gameGates
            {
                if (it->coord == tmp)
                    same = true;
            }
            if (gameMap.snakeHeadObject.coord == tmp) // Changed gameMap.head.coord to gameMap.snakeHeadObject.coord
                same = true;
            if (gameMap.growthItemObject.coord == tmp) // Changed gameMap.gItem.coord to gameMap.growthItemObject.coord
                same = true;
            if (gameMap.poisonItemObject.coord == tmp) // Changed gameMap.pItem.coord to gameMap.poisonItemObject.coord
                same = true;

            if (!same)
                break;
        }
    }

    void generateGate()
    {
        int wallIndex1, wallIndex2; // Renamed from idx1, idx2
        while (1)
        {
            wallIndex1 = rand() % gameMap.regularWalls.size(); // Changed gameMap.wall.size() to gameMap.regularWalls.size()
            wallIndex2 = rand() % gameMap.regularWalls.size(); // Changed gameMap.wall.size() to gameMap.regularWalls.size()
            if (gameMap.regularWalls[wallIndex1].coord.row == 1 || gameMap.regularWalls[wallIndex1].coord.row == gameMap.mapSize.height || gameMap.regularWalls[wallIndex1].coord.col == 1 || gameMap.regularWalls[wallIndex1].coord.col == gameMap.mapSize.width) // Changed gameMap.wall, coord.x/y, gameMap.size.h/w
            {
                int possibleDirections = 4; // Renamed from possible
                for (int i = 0; i < 4; i++)
                {
                    Coord tmp = gameMap.regularWalls[wallIndex1].coord; // Changed gameMap.wall to gameMap.regularWalls
                    switch (i)
                    {
                    case 0:
                        tmp.row--; // Changed tmp.x to tmp.row
                        break;
                    case 1:
                        tmp.row++; // Changed tmp.x to tmp.row
                        break;
                    case 2:
                        tmp.col--; // Changed tmp.y to tmp.col
                        break;
                    case 3:
                        tmp.col++; // Changed tmp.y to tmp.col
                        break;
                    }
                    for (auto it = gameMap.regularWalls.begin(); it != gameMap.regularWalls.end(); it++) // Changed gameMap.wall to gameMap.regularWalls
                    {
                        if (tmp == it->coord)
                            possibleDirections--;
                    }
                }
                if (possibleDirections <= 1)
                    continue;
            }
            if (gameMap.regularWalls[wallIndex2].coord.row == 1 || gameMap.regularWalls[wallIndex2].coord.row == gameMap.mapSize.height || gameMap.regularWalls[wallIndex2].coord.col == 1 || gameMap.regularWalls[wallIndex2].coord.col == gameMap.mapSize.width) // Changed gameMap.wall, coord.x/y, gameMap.size.h/w
            {
                int possibleDirections = 4;
                for (int i = 0; i < 4; i++)
                {
                    Coord tmp = gameMap.regularWalls[wallIndex2].coord; // Changed gameMap.wall to gameMap.regularWalls
                    switch (i)
                    {
                    case 0:
                        tmp.row--; // Changed tmp.x to tmp.row
                        break;
                    case 1:
                        tmp.row++; // Changed tmp.x to tmp.row
                        break;
                    case 2:
                        tmp.col--; // Changed tmp.y to tmp.col
                        break;
                    case 3:
                        tmp.col++; // Changed tmp.y to tmp.col
                        break;
                    }
                    for (auto it = gameMap.regularWalls.begin(); it != gameMap.regularWalls.end(); it++) // Changed gameMap.wall to gameMap.regularWalls
                    {
                        if (tmp == it->coord)
                            possibleDirections--;
                    }
                }
                if (possibleDirections <= 1)
                    continue;
            }
            if (wallIndex1 != wallIndex2)
                break;
        }
        gameMap.gameGates[0] = Gate(gameMap.regularWalls[wallIndex1]); // Changed gameMap.gate to gameMap.gameGates, gameMap.wall to gameMap.regularWalls
        gameMap.gameGates[1] = Gate(gameMap.regularWalls[wallIndex2]); // Changed gameMap.gate to gameMap.gameGates, gameMap.wall to gameMap.regularWalls
    }

    void generateItems()
    {
        generateGItem();
        generatePItem();
        generateTItem();
    }

    void generateTItem()
    {
        int row, col; // Renamed x, y to row, col
        generateRandCoord(row, col);
        gameMap.timeItemObject = TimeItem(row, col); // Using gameMap.timeItemObject
    }

    void generateGItem()
    {
        int row, col; // Renamed x, y to row, col
        generateRandCoord(row, col);
        gameMap.growthItemObject = GrowthItem(row, col); // Using gameMap.growthItemObject
    }

    void generatePItem()
    {
        int row, col; // Renamed x, y to row, col
        generateRandCoord(row, col);
        gameMap.poisonItemObject = PoisonItem(row, col); // Using gameMap.poisonItemObject
    }

    bool update(int &growthItemTimer, int &poisonItemTimer, int &timeItemTimer, int previousDirection = 0) // Renamed parameters
    {
        if (gateActiveDuration == 0)
        {
            gameMap.gameGates[0].isActive = false; // Changed gameMap.gate to gameMap.gameGates
            gameMap.gameGates[1].isActive = false; // Changed gameMap.gate to gameMap.gameGates
        }
        else
            gateActiveDuration--;
        if (gameMap.snakeHeadObject.currentDirection != -1) // Changed gameMap.head.direction to gameMap.snakeHeadObject.currentDirection
        {
            gameMap.snakeHeadObject.snakeBodySegments.insert(gameMap.snakeHeadObject.snakeBodySegments.begin(), SnakeBody(gameMap.snakeHeadObject)); // Changed gameMap.head.body.insert() and SnakeBody(gameMap.head)
            gameMap.snakeHeadObject.snakeBodySegments.pop_back(); // Changed gameMap.head.body.pop_back()
        }
        gameMap.snakeHeadObject.move(); // Changed gameMap.head.move()
        for (int i = 0; i < gameMap.gameGates.size(); i++) // Changed gameMap.gate.size() to gameMap.gameGates.size()
        {
            auto it = gameMap.gameGates.begin() + i; // Changed gameMap.gate.begin() to gameMap.gameGates.begin()
            if (it->coord == gameMap.snakeHeadObject.coord) // Changed gameMap.head.coord to gameMap.snakeHeadObject.coord
            {
                it->isActive = true;
                gateActiveDuration = gameMap.snakeHeadObject.snakeBodySegments.size(); // Changed gameMap.head.body.size() to gameMap.snakeHeadObject.snakeBodySegments.size()
                auto other = (i == 0 ? gameMap.gameGates.begin() + 1 : gameMap.gameGates.begin()); // Changed gameMap.gate.begin() to gameMap.gameGates.begin()
                if (other->exitDirection == 6) // Using exitDirection
                {
                    bool canGo = true;
                    Coord toGo = other->coord;
                    switch (gameMap.snakeHeadObject.currentDirection) // Changed gameMap.head.direction to gameMap.snakeHeadObject.currentDirection
                    {
                    case 1:
                        toGo.row--; // Changed toGo.x to toGo.row
                        break;
                    case 2:
                        toGo.col--; // Changed toGo.y to toGo.col
                        break;
                    case 3:
                        toGo.col++; // Changed toGo.y to toGo.col
                        break;
                    case 4:
                        toGo.row--; // Changed toGo.x to toGo.row
                        break;
                    }
                    for (auto w = gameMap.regularWalls.begin(); w != gameMap.regularWalls.end(); w++) // Changed gameMap.wall to gameMap.regularWalls
                    {
                        if (w->coord == toGo)
                        {
                            canGo = false;
                            break;
                        }
                    }
                    int possibleDirections = 4; // Renamed from possible
                    int dir = -1;
                    for (int i = 0; i < 4; i++)
                    {
                        Coord tmp = other->coord;
                        switch (i)
                        {
                        case 0:
                            tmp.row--; // Changed tmp.x to tmp.row
                            break;
                        case 1:
                            tmp.row++; // Changed tmp.x to tmp.row
                            break;
                        case 2:
                            tmp.col--; // Changed tmp.y to tmp.col
                            break;
                        case 3:
                            tmp.col++; // Changed tmp.y to tmp.col
                            break;
                        }
                        int t = possibleDirections;
                        for (auto it = gameMap.regularWalls.begin(); it != gameMap.regularWalls.end(); it++) // Changed gameMap.wall to gameMap.regularWalls
                        {
                            if (tmp == it->coord)
                                possibleDirections--;
                        }
                        if (possibleDirections == t)
                        {
                            switch (i)
                            {
                            case 0:
                                dir = 1;
                                break;
                            case 1:
                                dir = 2;
                                break;
                            case 2:
                                dir = 3;
                                break;
                            case 3:
                                dir = 4;
                                break;
                            }
                        }
                    }
                    if (!canGo && possibleDirections != 1)
                    {
                        switch (gameMap.snakeHeadObject.currentDirection) // Changed gameMap.head.direction to gameMap.snakeHeadObject.currentDirection
                        {
                        case 1:
                            gameMap.snakeHeadObject.currentDirection = 3; // Changed gameMap.head.direction to gameMap.snakeHeadObject.currentDirection
                            break;
                        case 2:
                            gameMap.snakeHeadObject.currentDirection = 1; // Changed gameMap.head.direction to gameMap.snakeHeadObject.currentDirection
                            break;
                        case 3:
                            gameMap.snakeHeadObject.currentDirection = 4; // Changed gameMap.head.direction to gameMap.snakeHeadObject.currentDirection
                            break;
                        case 4:
                            gameMap.snakeHeadObject.currentDirection = 2; // Changed gameMap.head.direction to gameMap.snakeHeadObject.currentDirection
                            break;
                        }
                    }
                    if (!canGo && possibleDirections == 1)
                    {
                        gameMap.snakeHeadObject.currentDirection = dir; // Changed gameMap.head.direction to gameMap.snakeHeadObject.currentDirection
                    }
                }
                else
                {
                    gameMap.snakeHeadObject.currentDirection = other->exitDirection; // Changed gameMap.head.direction to gameMap.snakeHeadObject.currentDirection, using exitDirection
                }
                gameMap.snakeHeadObject.coord = other->coord; // Changed gameMap.head.coord to gameMap.snakeHeadObject.coord
                gameMap.snakeHeadObject.move(); // Changed gameMap.head.move()
                gatesUsedCount++;
            }
        }

        if (gameMap.snakeHeadObject.coord == gameMap.growthItemObject.coord) // Changed gameMap.head.coord and gameMap.gItem.coord
        {
            growthItemCount++;
            generateGItem();
            growthItemTimer = 0;
            auto last = gameMap.snakeHeadObject.snakeBodySegments.end() - 1, sec = gameMap.snakeHeadObject.snakeBodySegments.end() - 2; // Changed gameMap.head.body
            gameMap.snakeHeadObject.snakeBodySegments.push_back(SnakeBody(last->coord.row - (sec->coord.row - last->coord.row), last->coord.col - (sec->coord.col - last->coord.col))); // Changed gameMap.head.body.push_back() and coord.x/y
        }
        if (gameMap.snakeHeadObject.coord == gameMap.poisonItemObject.coord) // Changed gameMap.head.coord and gameMap.pItem.coord
        {
            poisonItemCount++;
            generatePItem();
            poisonItemTimer = 0;
            gameMap.snakeHeadObject.snakeBodySegments.pop_back(); // Changed gameMap.head.body.pop_back()
        }
        if (gameMap.snakeHeadObject.coord == gameMap.timeItemObject.coord) // Changed gameMap.head.coord and gameMap.tItem.coord
        {
            generateTItem();
            timeItemTimer = 0;
            speedMultiplier = 1.5;
            speedBoostTimer = 40;
        }
        if (gameMap.snakeHeadObject.coord == gameMap.shieldItemObject.coord) {
            shieldTick   = SHIELD_DURATION;   // 40 tick 무적 상태 시작
            shieldCount++;
            gameMap.shieldItemObject = ShieldItem(-1, -1);      // 맵에서 제거 (좌표 0,0)
        }

        if (gameMap.snakeHeadObject.coord == gameMap.randomItemObject.coord)
        {
            int rnd = rand()%4;            // 0:G 1:P 2:T 3:S
            switch(rnd){
                case 0:  /* 성장 효과 */
                    growthItemCount++;
                    generateGItem();
                    {
                        auto &body=gameMap.snakeHeadObject.snakeBodySegments;
                        auto last=body.end()-1, sec=body.end()-2;
                        body.push_back(SnakeBody(
                            last->coord.row-(sec->coord.row-last->coord.row),
                            last->coord.col-(sec->coord.col-last->coord.col)));
                    }
                    break;
                case 1:  /* 독 효과 */
                    poisonItemCount++;
                    generatePItem();
                    gameMap.snakeHeadObject.snakeBodySegments.pop_back();
                    break;
                case 2:  /* 시간 효과 */
                    generateTItem();
                    speedMultiplier=1.5;
                    speedBoostTimer=40;
                    break;
                case 3:  /* 보호막 효과 */
                    shieldTick   = SHIELD_DURATION;
                    shieldCount++;
                    break;
            }
            gameMap.randomItemObject = RandomItem();   // 좌표 -1,-1 로 리셋
        }

        // mission
        if (gameMap.snakeHeadObject.snakeBodySegments.size() >= 7) // Changed gameMap.head.body.size()
            missionSnakeLengthStatus = 'v';
        else
            missionSnakeLengthStatus = ' ';
        if (growthItemCount >= 5)
            missionGrowthItemStatus = 'v';
        else
            missionGrowthItemStatus = ' ';
        if (poisonItemCount >= 2)
            missionPoisonItemStatus = 'v';
        else
            missionPoisonItemStatus = ' ';
        if (gatesUsedCount >= 1)
            missionGateUseStatus = 'v';
        else
            missionGateUseStatus = ' ';

        // allMissionsCompleted
        if (gameMap.snakeHeadObject.snakeBodySegments.size() >= 7 && growthItemCount >= 5 && poisonItemCount >= 2 && gatesUsedCount >= 1) // Changed gameMap.head.body.size()
        {
            allMissionsCompleted = true;
        }
        if (gameMap.snakeHeadObject.snakeBodySegments.size() > maxSnakeLength) // Changed gameMap.head.body.size()
            maxSnakeLength = gameMap.snakeHeadObject.snakeBodySegments.size(); // Changed gameMap.head.body.size()
        if (shieldTick > 0) --shieldTick;
        
        return isValid(previousDirection);
    }
};

#endif