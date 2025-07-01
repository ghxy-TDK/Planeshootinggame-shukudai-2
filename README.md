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
├── include/                  # 所有头文件
│   ├── Game/
│   │   ├── Bullet.h
│   │   ├── Enemy.h
│   │   ├── Explosion.h
│   │   ├── GameObject.h
│   │   ├── Player.h
│   │   └── PowerUp.h
│   ├── Managers/
│   │   ├── GameManager.h
│   │   └── SoundManager.h
│   └── Utils/
│       ├── common.h
│       └── Constants.h      # 建议添加的常量定义文件
├── src/                     # 所有源代码
│   ├── Game/
│   │   ├── Bullet.cpp
│   │   ├── Enemy.cpp
│   │   ├── Explosion.cpp
│   │   ├── GameObject.cpp
│   │   ├── Player.cpp
│   │   └── PowerUp.cpp
│   ├── Managers/
│   │   ├── GameManager.cpp
│   │   └── SoundManager.cpp
│   └── Utils/
│       └── common.cpp
├── res/                     # 游戏资源
│   ├── Textures/
|   |   ├── BOSS.png
|   |   ├── enemy001.png
|   |   ├── enemy002.png
│   │   └── FEUI.png         # 图片资源
│   └── Sounds/              # 音效资源
├── vcproj/                  # VS项目配置
│   ├── planeshootinggame.vcxproj
│   ├── planeshootinggame.filters
│   └── planeshootinggame.user
└── scripts/                 # 构建/运行脚本
    ├── build.sh             # 构建脚本
    └── run.sh               # 运行脚本
