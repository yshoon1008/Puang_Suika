#include "Playfield.h"

Playfield::Playfield(b2World* world) {
    float w = (float)Config::WINDOW_WIDTH;
    float h = (float)Config::WINDOW_HEIGHT;
    float thick = 20.0f;

    // 바닥, 좌, 우 벽 생성
    createWall(world, w/2, h, w, thick);       
    createWall(world, thick/2, h/2, thick, h);       
    createWall(world, w - thick/2, h/2, thick, h);       

    // 화면에 보이는 벽(RectangleShape) 셋업
    walls[0].setSize({w, thick}); walls[0].setPosition({0, h - thick/2});
    walls[1].setSize({thick, h}); walls[1].setPosition({0, 0});
    walls[2].setSize({thick, h}); walls[2].setPosition({w - thick, 0});

    for(int i=0; i<3; i++) walls[i].setFillColor(sf::Color(100, 100, 100));

    // 제한선 생성
    limitLine.setSize({w, 2.0f});
    limitLine.setFillColor(sf::Color::Red);
    limitLine.setPosition({0, getLimitLineY()});
}

void Playfield::createWall(b2World* world, float x, float y, float w, float h) {
    b2BodyDef bodyDef;
    bodyDef.position.Set(x / Config::PPM, y / Config::PPM);     // 픽셀 좌표 → 미터 단위로 변환
    b2Body* body = world->CreateBody(&bodyDef);                 // 정적 바디(기본값) 생성
    b2PolygonShape box;                                         // Box2D 박스 형상 (절반 폭/높이 사용)
    box.SetAsBox((w/2) / Config::PPM, (h/2) / Config::PPM);
    body->CreateFixture(&box, 0.0f);                            // 밀도 0 = 정적 바디
}

void Playfield::render(sf::RenderWindow& window) {
    for(int i=0; i<3; i++) window.draw(walls[i]);
    window.draw(limitLine);
}