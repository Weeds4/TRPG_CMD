#include "game.h"
#include "util.h"

int main() {
    init();
    while (1) {
        titleDraw();
        int menuCode = menuDraw();
        if (menuCode == 0) {
            // 게임 시작
            runGame();
        }
        else if (menuCode == 1) {
            // 게임 정보
            infoDraw();
            
        }
        else if (menuCode == 2) {
            return 0;
        }
        system("cls");
    }
    return 0;
}
