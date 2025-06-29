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

// 游戏常量定义
#define WINDOW_WIDTH 1080
#define WINDOW_HEIGHT 1080
#define MAX_STAR 150
#define MAX_BULLETS 100
#define MAX_ENEMIES 15
#define MAX_EXPLOSIONS 30

// 游戏状态枚举
enum GameState { MENU, PLAYING, GAME_OVER, PAUSED };

// 表示背景中的星星
struct Star {
    double x;
    int y, color;
    double step;
};

// 表示碰撞检测的矩形
struct Rect {
    int x, y, width, height;
    bool intersects(const Rect& other) const {
        return x < other.x + other.width && x + width > other.x &&
            y < other.y + other.height && y + height > other.y;
    }
};


