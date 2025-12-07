#include "PuangManager.h"
#include <algorithm> 
#include <iostream>

// ---------------------------------------------------------
// 1. 충돌 리스너 구현
// ---------------------------------------------------------
void PuangContactListener::BeginContact(b2Contact* contact) {
    // 충돌한 두 바디의 UserData(Puang 객체 주소)를 가져옴
    // Box2D 2.4.1 문법: GetUserData().pointer
    auto dataA = contact->GetFixtureA()->GetBody()->GetUserData();
    auto dataB = contact->GetFixtureB()->GetBody()->GetUserData();

    Puang* p1 = reinterpret_cast<Puang*>(dataA.pointer);
    Puang* p2 = reinterpret_cast<Puang*>(dataB.pointer);

    if (p1 && p2 && !p1->isDead && !p2->isDead) {
        if (p1->getTier() == p2->getTier()) {
            // 같은 단계끼리 부딪혔으면 병합 큐에 추가
            mergeQueue.push_back({p1, p2});
        }
    }
}

// ---------------------------------------------------------
// 2. 매니저 구현
// ---------------------------------------------------------
PuangManager::PuangManager(b2World* world) : world(world), score(0) {
    world->SetContactListener(&contactListener);
    loadTextures(); // 게임 시작 시 이미지 로딩
    mergeDelayTimer = 0.0f;
}

PuangManager::~PuangManager() {
    for (auto p : puangs) delete p;
}

// 이미지 일괄 로딩 함수
void PuangManager::loadTextures() {
    for (int i = 0; i <= (int)PuangTier::Puang10; i++) {
        PuangTier tier = static_cast<PuangTier>(i);
        sf::Texture tex;
        
        // 파일명 규칙: assets/puang_0.png, puang_1.png ...
        std::string filename = "assets/puang_" + std::to_string(i) + ".png";
        
        // 이미지 로딩 시도
        if (!tex.loadFromFile(filename)) {
            // 실패 시 콘솔 경고 및 빈 텍스처 생성 (튕김 방지)
            std::cerr << "[Warning] Failed to load: " << filename << std::endl;
            tex.resize({64, 64}); 
        } else {
            // 이미지가 깨지지 않고 부드럽게 보이도록 설정
            tex.setSmooth(true);
        }
        
        // 맵에 저장
        textures[tier] = tex;
    }
}

// 텍스처 getter
const sf::Texture& PuangManager::getTexture(PuangTier tier) {
    return textures[tier];
}

Puang* PuangManager::spawnPuang(PuangTier tier, float x, float y) {
    // 생성된 텍스처를 Puang 생성자에 전달
    Puang* p = new Puang(world, tier, x, y, textures[tier]);
    puangs.push_back(p);
    return p;
}

void PuangManager::update() {
    // 1. 각 푸앙 객체 업데이트 (물리 -> 그래픽 동기화)
    for (auto p : puangs) p->update();
    
    // 2. 병합 처리
    processMerges();

    // 3. 삭제된 객체(isDead) 메모리 정리
    auto it = std::remove_if(puangs.begin(), puangs.end(), [this](Puang* p) {
        if (p->isDead) {
            world->DestroyBody(p->getBody()); // 물리 세계에서 제거
            delete p; // 메모리 해제
            return true;
        }
        return false;
    });
    puangs.erase(it, puangs.end());
}


void PuangManager::render(sf::RenderWindow& window) {
    for (auto p : puangs) p->render(window);
}

// 병합 큐 처리 함수
void PuangManager::processMerges() {
    // 1. 쿨타임 체크 (딜레이 중이면 처리 안 함)
    if (mergeDelayTimer > 0.0f) {
        mergeDelayTimer -= 1.0f / 60.0f; // 60프레임 기준 시간 감소
        return; 
    }

    // 큐가 비어있으면 할 일 없음
    if (contactListener.mergeQueue.empty()) return;

    // 2. 큐를 순회하며 "유효한 합체" 하나만 찾아서 실행
    auto it = contactListener.mergeQueue.begin();
    while (it != contactListener.mergeQueue.end()) {
        Puang* p1 = it->first;
        Puang* p2 = it->second;

        // 이미 죽은(처리된) 객체가 포함된 요청은 큐에서 삭제하고 다음으로 넘어감
        if (p1->isDead || p2->isDead) {
            it = contactListener.mergeQueue.erase(it);
            continue;
        }

        // --- 여기서부터 실제 합체 로직 (단 1건만 처리) ---
        
        b2Vec2 pos1 = p1->getBody()->GetPosition();
        b2Vec2 pos2 = p2->getBody()->GetPosition();
        float midX = (pos1.x + pos2.x) / 2.0f * Config::PPM;
        float midY = (pos1.y + pos2.y) / 2.0f * Config::PPM;

        int nextTierInt = static_cast<int>(p1->getTier()) + 1;
        score += (nextTierInt * 10);
        
        p1->isDead = true;
        p2->isDead = true;

        // 마지막 단계(Puang10)가 아니면 다음 단계 생성
        if (nextTierInt <= static_cast<int>(PuangTier::Puang10)) { 
            spawnPuang(static_cast<PuangTier>(nextTierInt), midX, midY);
            
            // 합체 성공 시 쿨타임 설정 (0.1초 딜레이)
            mergeDelayTimer = 0.10f; 
        }

        // 처리한 요청을 큐에서 삭제하고
        contactListener.mergeQueue.erase(it);
        
        // 한 프레임에 하나만 처리했으므로 함수를 즉시 종료합니다.
        // 남은 큐는 다음 프레임(또는 쿨타임 이후)에 처리됩니다.
        return; 
    }
}

// 오버플로우(제한선 초과) 체크 함수
bool PuangManager::checkOverflow(float limitY) {
    for (auto p : puangs) {
        // 1. 이미 죽은(병합 예정) 객체는 패스
        if (p->isDead) continue;

        // 2. 움직이고 있는(떨어지는 중인) 객체는 봐줌 (속도가 느릴 때만 체크)
        b2Vec2 velocity = p->getBody()->GetLinearVelocity();
        if (velocity.Length() > 0.5f) continue; 

        // 3. 높이 계산 (Box2D 좌표 -> 픽셀 좌표 변환)
        // Y좌표는 아래로 갈수록 커지므로, '위치 - 반지름'이 '제한선'보다 작으면 넘친 것
        float topY = (p->getBody()->GetPosition().y * Config::PPM) - p->getRadius();
        
        if (topY < limitY) {
            return true; // 선 넘음!
        }
    }
    return false;
}