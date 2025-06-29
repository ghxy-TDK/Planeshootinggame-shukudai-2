#pragma once
#include <graphics.h>
#include <conio.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include <mmsystem.h>
#include <iostream>
#include <string>

// ��Ϸ��������
#define WINDOW_WIDTH 1080
#define WINDOW_HEIGHT 1080
#define MAX_STAR 150
#define MAX_BULLETS 100
#define MAX_ENEMIES 15
#define MAX_EXPLOSIONS 30

// ��Ϸ״̬ö��
enum GameState { MENU, PLAYING, GAME_OVER, PAUSED };

// ��ʾ�����е�����
struct Star {
    double x;
    int y, color;
    double step;
};

// ��ʾ��ײ���ľ���
struct Rect {
    int x, y, width, height;
    bool intersects(const Rect& other) const {
        return x < other.x + other.width && x + width > other.x &&
            y < other.y + other.height && y + height > other.y;
    }
};


