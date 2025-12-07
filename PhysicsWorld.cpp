#include "PhysicsWorld.h"
#include "GameTypes.h"

PhysicsWorld::PhysicsWorld() {
    // Box2D 중력 벡터. (x=0, y=Config::GRAVITY)
    b2Vec2 gravity(0.0f, Config::GRAVITY);
    world = new b2World(gravity);
}

PhysicsWorld::~PhysicsWorld() {
    delete world;
}

void PhysicsWorld::update(float dt) {
    // Box2D Step 호출. dt 동안 물리 시뮬레이션 진행.
    world->Step(dt, velocityIterations, positionIterations);
}