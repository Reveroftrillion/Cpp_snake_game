#include <vector>
#include "game.h"
#include <ncurses.h>

using namespace std;

int main(){
    int inputCharacter, menuOptionSelected = 1; // Renamed ch, bt
    Game gameInstance = Game(); // Renamed m
    while(1){
        initscr();
        clear();
        raw();
        keypad(stdscr, TRUE);
        noecho();
        curs_set(0);
        mvprintw(12, 29, "SNAKE GAME");
        mvprintw(15, 34, "Play : Press 'p'");
        mvprintw(16, 34, "Exit : Press 'e'");
        while(1){
            inputCharacter = getch(); // Using inputCharacter
            if(inputCharacter == 'p'){
                menuOptionSelected = 1; // Using menuOptionSelected
                break;
            }
            else if(inputCharacter == 'e'){
                menuOptionSelected = 2; // Using menuOptionSelected
                break;
            }
        }
        if(menuOptionSelected == 1){ // Using menuOptionSelected
            gameInstance = Game(); // Using gameInstance
            gameInstance.refreshScreen(); // Using gameInstance
        }
        if(menuOptionSelected == 2){ // Using menuOptionSelected
            endwin();
            return 0;
        }
    }
    return 0;
}