#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "GameTypes.h"
#include "PhysicsWorld.h"
#include "Playfield.h"
#include "PuangManager.h"
#include "UIManager.h"
#include <random>

// 전체 게임 루프를 관리하는 핵심 클래스.
// - 윈도우 생성 및 이벤트 처리
// - 물리 업데이트, 푸앙 관리, UI 렌더링
// - Scene 전환(메뉴 / 게임 / 도감) 및 게임 오버 처리
class Game {
public:
    Game();
    ~Game();
    void run();

private:
    void processEvents();
    void update();
    void render();
    
    // 헬퍼 함수들
    void generateNext();            // 다음 푸앙 랜덤 생성
    void updateNextPreview();       // Next UI 갱신
    void updateCurrentPreview();    // Current(마우스) UI 갱신
    float getClampedMouseX();       // 마우스 X좌표 제한

    void renderMenu();  
    void renderGallery();
    void renderGame();

    void renderGameOver();      // 그리기 함수
    void resetGame();           // 재시작 함수

    std::string getPuangName(PuangTier tier);

    sf::RenderWindow window;
    PhysicsWorld* physicsWorld;
    Playfield* playfield;
    PuangManager* puangManager;
    UIManager* uiManager;
    
    // UI 요소
    sf::RectangleShape guideLine;           // 마우스 가이드 라인
    sf::RectangleShape uiBackground;        // Next가 잘 보이도록 배경판 추가

    // 미리보기 스프라이트 (포인터)
    sf::CircleShape* nextPreviewShape;      // 다음에 나올 녀석 
    sf::CircleShape* currentPreviewShape;   // 지금 손에 들고 있는 녀석 (마우스 따라다님) 
    
    
    sf::Text* nextLabel;                    // "Next" 라벨
    sf::Font font;                          // 공통 폰트

    // 데이터
    PuangTier currentTier;                  // 현재 손에 들고 있는 푸앙 티어
    PuangTier nextTier;                     // 다음에 나올 푸앙 티어
    float dropTimer;                        // 푸앙 낙하 딜레이 타이머
    float gameOverTimer;                    // 게임오버 누적 시간

    Scene currentScene;                     // 현재 장면

    // 메인 메뉴 UI
    sf::Text* titleText;           
    
    sf::RectangleShape startButton;
    sf::Text* startText;
    
    sf::RectangleShape galleryButton;
    sf::Text* galleryText;

    sf::RectangleShape backButton;
    sf::Text* backText;

    sf::Music* bgm;
    sf::SoundBuffer dropBuffer;     // 효과음 데이터
    sf::Sound* dropSound;           // 효과음 재생기
};