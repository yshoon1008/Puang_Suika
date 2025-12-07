#pragma once
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include "GameTypes.h"

// 게임 플레이 영역(바닥, 좌우 벽, 게임 오버 기준선)을 관리하는 클래스.
// - Box2D 정적인 바디(벽) 생성
// - SFML RectangleShape로 벽과 limitLine 렌더링
class Playfield {
public:
    // world : 벽 바디를 생성할 Box2D 월드
    Playfield(b2World* world);

    // 벽과 제한선을 화면에 그린다.
    void render(sf::RenderWindow& window);

    // 게임 오버 기준선 Y좌표 (픽셀 단위)
    float getLimitLineY() const { return 200.0f; }

private:
    // Box2D에 정적인 바디(벽)를 하나 만든다.
    // x, y : 중심 위치 (픽셀 기준)
    // w, h : 폭, 높이 (픽셀 기준)
    void createWall(b2World* world, float x, float y, float w, float h);

    sf::RectangleShape walls[3];    // 바닥/좌/우 벽

    sf::RectangleShape limitLine;   // 게임 오버 기준선 (이 선을 넘으면 GameOver)
};