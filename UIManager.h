#pragma once
#include <SFML/Graphics.hpp>
#include "GameTypes.h"

// 점수 텍스트 등 간단한 UI 요소를 관리하는 클래스.
// - 폰트 로딩
// - Score 텍스트 업데이트/렌더링
class UIManager {
public:
    UIManager();
    void updateScore(int score);
    void render(sf::RenderWindow& window);
private:
    sf::Font font;
    sf::Text scoreText;
    bool hasFont;
};