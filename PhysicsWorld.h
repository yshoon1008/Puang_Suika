#pragma once
#include <box2d/box2d.h>

// Box2D 세계(b2World)를 감싸는 래퍼 클래스.
// - 중력, Step 파라미터(velocity / position iterations)를 한 곳에서 관리한다.
// - Game에서는 PhysicsWorld만 알고, 직접 b2World를 만들지 않도록 역할을 분리했다.
class PhysicsWorld {
public:
    PhysicsWorld();
    ~PhysicsWorld();

    // 물리 시뮬레이션 한 프레임 진행
    // dt : 지난 프레임 시간(초 단위). 현재는 고정값(1/60)으로 호출한다.
    void update(float dt);

    // Box2D 세계 포인터 반환
    // - Playfield, PuangManager 등에서 바디 생성 시 사용한다.
    b2World* getWorld() { return world; }

private:
    b2World* world;
    const int velocityIterations = 8;   // 속도(velocity) 반복 횟수
    const int positionIterations = 3;   // 위치(position) 반복 횟수
};