# Planeshootinggame-shukudai-2
# ✈️ Plane Shooting Game - EasyX飞机大战

**使用EasyX图形库开发的经典街机风格飞机射击游戏**，包含完整的Visual Studio项目文件和游戏资源。

## 🚀 项目特点
- 纯C++实现，高性能游戏循环
- EasyX图形库渲染游戏画面
- 完整的游戏资源管理（图片/音效）
- Visual Studio 2022项目即开即用

## 📁 项目结构
```plaintext
planeshootinggame/
├── include/                  # 公共头文件目录
│   ├── GameObjects/          # 游戏对象相关头文件
│   │   ├── Bullet.h
│   │   ├── Enemy.h
│   │   ├── Explosion.h
│   │   ├── GameObject.h      # 基类
│   │   ├── Player.h
│   │   └── PowerUp.h
│   ├── Managers/             # 管理类头文件
│   │   ├── GameManager.h
│   │   └── SoundManager.h
│   └── Utils/                # 工具类头文件
│       └── common.h
│
├── src/                      # 源代码目录
│   ├── GameObjects/          # 游戏对象实现
│   │   ├── Bullet.cpp
│   │   ├── Enemy.cpp
│   │   ├── Explosion.cpp
│   │   ├── GameObject.cpp
│   │   ├── Player.cpp
│   │   └── PowerUp.cpp
│   │
│   ├── Managers/             # 管理类实现
│   │   ├── GameManager/      # GameManager子模块
│   │   │   ├── GameManager.cpp
│   │   │   ├── GameManager_Collision.cpp
│   │   │   ├── GameManager_Draw.cpp
│   │   │   ├── GameManager_Input.cpp
│   │   │   ├── GameManager_Skill.cpp
│   │   │   ├── GameManager_Spawn.cpp
│   │   │   └── GameManager_Update.cpp
│   │   └── SoundManager.cpp
│   │
│   ├── Utils/                # 工具类实现
│   │   └── common.cpp
│   │
│   └── planeshootinggame.cpp # 主程序入口
│
├── res/                      # 游戏资源
│   ├── Textures/             # 纹理资源
│   │   ├── BOSS.png
│   │   ├── enemy001.png
│   │   ├── enemy002.png
│   │   └── FEUI.png
│   └── Sounds/               # 音效资源
│   │   └── backgroundmusic2.wma
│
└── vcproj/                   # VS项目配置
    ├── planeshootinggame.vcxproj
    ├── planeshootinggame.filters
    └── planeshootinggame.user
