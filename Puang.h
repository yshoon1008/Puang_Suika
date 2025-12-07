#pragma once
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include "GameTypes.h"

// 하나의 푸앙(동그라미 캐릭터)을 나타내는 클래스.
// - Box2D 동적 바디(b2Body) + SFML CircleShape를 함께 관리한다.
// - tier에 따라 반지름과 텍스처가 달라진다.
class Puang {
public:
    // world : 바디를 생성할 Box2D 월드
    // tier  : 푸앙 단계
    // x, y  : 생성 위치(픽셀 기준)
    // texture : 해당 tier에 맞는 푸앙 텍스처
    Puang(b2World* world, PuangTier tier, float x, float y, const sf::Texture& texture);
    ~Puang();

    void update();                          // Box2D 물리 결과를 SFML shape에 반영 (위치/각도 동기화)
    void render(sf::RenderWindow& window);  // 화면에 푸앙을 그린다.

    PuangTier getTier() const { return tier; }
    b2Body* getBody() const { return body; }
    
    bool isDead = false;
    // 현재 반지름 반환 (게임오버 판정용)
    float getRadius() const { return shape.getRadius(); }
    // 티어별 반지름의 단일 정의
    static float radiusFor(PuangTier t);

private:
    b2Body* body;
    sf::CircleShape shape;
    PuangTier tier;      
};