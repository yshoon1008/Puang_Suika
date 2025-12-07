#include "UIManager.h"
#include <iostream>

// 생성자에서는 폰트 로딩과 scoreText 기본 설정을 수행한다.
UIManager::UIManager() : scoreText(font) {
    
    // 폰트 로딩 (경로 주의)
    // 윈도우 기본 폰트 경로를 사용합니다.
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        hasFont = false;
    } else {
        hasFont = true;
    }

    // 점수 텍스트 초기화
    scoreText.setFont(font); 
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::Black);
    scoreText.setPosition({50.0f, 10.0f}); // SFML 3.0 벡터 초기화
    
    updateScore(0);
}

// 점수 업데이트 함수
void UIManager::updateScore(int score) {
    if (hasFont) {
        scoreText.setString("Score: " + std::to_string(score));
    }
}

// 렌더링 함수
void UIManager::render(sf::RenderWindow& window) {
    if (hasFont) {
        window.draw(scoreText);
    }
}