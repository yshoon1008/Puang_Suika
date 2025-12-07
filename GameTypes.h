#pragma once

namespace Config {
    const float PPM = 30.0f;        // 1미터 = 30픽셀
    const int WINDOW_WIDTH = 600;
    const int WINDOW_HEIGHT = 800;
    const float GRAVITY = 20.0f;      
}

// 푸앙이 진화 단계 (1~11단계)
enum class PuangTier {
    Puang00, Puang01, Puang02, Puang03, Puang04,
    Puang05, Puang06, Puang07, Puang08, Puang09, Puang10
};

enum class Scene {
    Menu,       // 시작 화면
    Playing,    // 게임 중
    Gallery,    // 캐릭터 도감
    GameOver    // 게임 오버    
};

enum class GameState {
    Ready, Playing, GameOver
};