#include"GameManager.h"
int main() {
    SoundManager soundManager;
    soundManager.playBGM();
    srand((unsigned)time(NULL));
    initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
    setbkcolor(RGB(0, 0, 0));
    BeginBatchDraw();
    GameManager game;
    game.run();
    EndBatchDraw();
    closegraph();
    return 0;
}