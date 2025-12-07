#pragma once
#include <vector>
#include <map>
#include <string>
#include <box2d/box2d.h>
#include "Puang.h"
#include "GameTypes.h"

// 전방 선언
class PuangManager; 

// Box2D 충돌 콜백 리스너.
// - BeginContact에서 같은 tier 푸앙 두 개를 발견하면 mergeQueue에 쌍으로 넣어준다.
// - 실제 병합/점수 계산은 PuangManager::processMerges에서 처리한다.
class PuangContactListener : public b2ContactListener {
public:
    std::vector<std::pair<Puang*, Puang*>> mergeQueue;
    void BeginContact(b2Contact* contact) override;
};

// 푸앙 전체(벡터)를 관리하고, 병합/점수/텍스처를 담당하는 매니저 클래스.
// - spawnPuang : 새 푸앙 생성
// - update     : 물리 결과 반영 및 삭제 처리
// - render     : 모든 푸앙 렌더링
// - processMerges : 충돌 큐를 기반으로 병합 로직 수행
class PuangManager {
public:
    PuangManager(b2World* world);
    ~PuangManager();

    void update();
    void render(sf::RenderWindow& window);
    
    // 텍스처 관리가 내부에서 이루어지므로 호출부는 좌표와 티어만 주면 됨
    Puang* spawnPuang(PuangTier tier, float x, float y);
    
    int getScore() const { return score; }

    // 다음 푸앙 미리보기(Next UI)를 위해 텍스처를 반환하는 함수
    const sf::Texture& getTexture(PuangTier tier);

    bool checkOverflow(float limitY);

private:
    b2World* world;
    std::vector<Puang*> puangs;
    PuangContactListener contactListener;
    int score;
    float mergeDelayTimer;

    // 텍스처 저장소 (Tier -> Texture 매핑)
    std::map<PuangTier, sf::Texture> textures;
    
    // 텍스처 로딩 함수
    void loadTextures();

    void processMerges();
};