#include "Puang.h"

// 하나의 푸앙 객체를 생성한다.
// - Box2D 동적 바디 생성
// - tier에 따라 반지름을 결정하고, SFML CircleShape 설정
// - body userData에 this 포인터를 저장해 ContactListener에서 되찾을 수 있게 한다.
Puang::Puang(b2World* world, PuangTier tier, float x, float y, const sf::Texture& texture) 
    : tier(tier) {
    
    float radius = Puang::radiusFor(tier);
    
    // Box2D 물리 설정
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position.Set(x / Config::PPM, y / Config::PPM);
    body = world->CreateBody(&def);

    b2CircleShape circle;
    circle.m_radius = radius / Config::PPM;

    b2FixtureDef fixture;
    fixture.shape = &circle;
    fixture.density = 1.0f;
    fixture.friction = 0.3f;
    fixture.restitution = 0.2f; // 튕기는 정도
    body->CreateFixture(&fixture);

    // ContactListener에서 이 푸앙 객체를 되찾기 위한 포인터 저장
    body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);

    shape.setRadius(radius);
    shape.setOrigin({radius, radius}); // 중심점을 원의 중앙으로
    shape.setTexture(&texture);        // 텍스처 입히기

    // 1. 중심점(Origin) 설정
    sf::Vector2u texSize = texture.getSize();
    int size = std::min(texSize.x, texSize.y); // 가로/세로 중 작은 쪽 기준
    int offsetX = (texSize.x - size) / 2;
    int offsetY = (texSize.y - size) / 2;
    
    // 텍스처의 중앙 부분만 사용 (Texture Rect 설정)
    shape.setTextureRect(sf::IntRect({offsetX, offsetY}, {size, size}));

    shape.setPosition({x, y});
    shape.setRotation(sf::radians(body->GetAngle()));
}

Puang::~Puang() { 
    // body는 World가 파괴될 때 같이 파괴되거나 Manager가 처리하므로 여기선 둠
}

void Puang::update() {
    b2Vec2 pos = body->GetPosition();
    // shape 위치 동기화
    shape.setPosition({pos.x * Config::PPM, pos.y * Config::PPM});
    
    // 각도 처리
    shape.setRotation(sf::radians(body->GetAngle()));
}

void Puang::render(sf::RenderWindow& window) {
    window.draw(shape);
}

float Puang::radiusFor(PuangTier t) {
    // 화면 너비 600px 기준
    // 최종 : 반지름 180px (지름 360px = 60%)
    switch (t) {
        case PuangTier::Puang00:    return 20.0f;  
        case PuangTier::Puang01:    return 30.0f;  
        case PuangTier::Puang02:    return 40.0f;  
        case PuangTier::Puang03:    return 51.0f;  
        case PuangTier::Puang04:    return 63.0f;  
        case PuangTier::Puang05:    return 76.0f;  
        case PuangTier::Puang06:    return 90.0f; 
        case PuangTier::Puang07:    return 105.0f; 
        case PuangTier::Puang08:    return 121.0f; 
        case PuangTier::Puang09:    return 138.0f; 
        case PuangTier::Puang10:    return 180.0f; 
        default:                    return 20.0f;
    }
}
