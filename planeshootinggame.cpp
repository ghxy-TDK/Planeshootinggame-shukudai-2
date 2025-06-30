#include"GameManager.h"
int main() {
    SoundManager soundManager;
    soundManager.playBGM();
    srand((unsigned)time(NULL));
    initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
    setbkcolor(RGB(0, 0, 0));
    GameManager game;
    game.run();

    closegraph();
    return 0;
}