#include "Game.h"
#include <iostream>
#include <algorithm> // std::min, std::max 사용

Game::Game() {
    window.create(sf::VideoMode({Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT}), "CAU Puang Suika");
    window.setFramerateLimit(60);

    // 1. 시스템 초기화
    physicsWorld = new PhysicsWorld();
    playfield = new Playfield(physicsWorld->getWorld());
    puangManager = new PuangManager(physicsWorld->getWorld());
    uiManager = new UIManager();
    gameOverTimer = 0.0f;
    dropTimer = 0.0f;

    // 2. 폰트 로드
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
    }

    // 3. 배경음악 로드 및 재생
    bgm = new sf::Music();
    if (!bgm->openFromFile("assets/bgm.mp3")) {
        std::cerr << "Failed to load bgm!" << std::endl;
    } else {
        bgm->setLooping(true);  // 무한 반복 설정
        bgm->setVolume(30);     // 볼륨 조절 (0~100)
        bgm->play();            // 재생 시작
    }

    // 4. 효과음 로드
    if (!dropBuffer.loadFromFile("assets/drop.mp3")) {
        std::cerr << "Failed to load drop.mp3!" << std::endl;
        // 로드 실패 시에도 안전하게 생성 (빈 버퍼라도 넣어줌)
        dropSound = new sf::Sound(dropBuffer); 
    } else {
        // 버퍼를 넣어서 동적 할당
        dropSound = new sf::Sound(dropBuffer);
        dropSound->setVolume(80); 
    }

    // =========================================================
    // [UI 초기화 1] 시작 화면 (메뉴)
    // =========================================================
    currentScene = Scene::Menu;

    // 타이틀
    titleText = new sf::Text(font); 
    titleText->setString("CAU Puang Suika");
    titleText->setCharacterSize(50);
    titleText->setFillColor(sf::Color(0, 102, 204)); 
    titleText->setStyle(sf::Text::Bold);
    
     // SFML 3.0의 getLocalBounds()는 size.x / size.y 사용
    sf::FloatRect titleRect = titleText->getLocalBounds();
    titleText->setOrigin({titleRect.size.x / 2.0f, titleRect.size.y / 2.0f});
    titleText->setPosition({Config::WINDOW_WIDTH / 2.0f, 150.0f});

    // 시작 버튼 박스
    startButton.setSize({200.0f, 60.0f});
    startButton.setFillColor(sf::Color(50, 200, 50));
    startButton.setOrigin({100.0f, 30.0f});
    startButton.setPosition({Config::WINDOW_WIDTH / 2.0f, 350.0f});

    // 시작 버튼 텍스트
    startText = new sf::Text(font); 
    startText->setString("Start Game");
    startText->setCharacterSize(24);
    startText->setFillColor(sf::Color::White);
    
    sf::FloatRect startRect = startText->getLocalBounds();
    startText->setOrigin({startRect.size.x / 2.0f, startRect.size.y / 2.0f});
    startText->setPosition({Config::WINDOW_WIDTH / 2.0f, 350.0f});

    // 도감 버튼 박스
    galleryButton.setSize({200.0f, 60.0f});
    galleryButton.setFillColor(sf::Color(255, 165, 0));
    galleryButton.setOrigin({100.0f, 30.0f});
    galleryButton.setPosition({Config::WINDOW_WIDTH / 2.0f, 450.0f});

    // 도감 버튼 텍스트
    galleryText = new sf::Text(font); 
    galleryText->setString("Gallery");
    galleryText->setCharacterSize(24);
    galleryText->setFillColor(sf::Color::White);
    
    sf::FloatRect galRect = galleryText->getLocalBounds();
    galleryText->setOrigin({galRect.size.x / 2.0f, galRect.size.y / 2.0f});
    galleryText->setPosition({Config::WINDOW_WIDTH / 2.0f, 450.0f});

    // =========================================================
    // [UI 초기화 2] 도감 화면
    // =========================================================
    backButton.setSize({80.0f, 40.0f});
    backButton.setFillColor(sf::Color(100, 100, 100));
    backButton.setPosition({20.0f, 20.0f});

    backText = new sf::Text(font); 
    backText->setString("Back");
    backText->setCharacterSize(18);
    backText->setFillColor(sf::Color::White);
    backText->setPosition({35.0f, 28.0f});

    // =========================================================
    // [UI 초기화 3] 게임 화면
    // =========================================================
    guideLine.setSize({2.0f, (float)Config::WINDOW_HEIGHT});
    guideLine.setFillColor(sf::Color(200, 200, 200));

    // 우측 상단 Next 영역 배경
    uiBackground.setSize({120.0f, 150.0f});
    uiBackground.setFillColor(sf::Color(255, 255, 255, 200)); 
    uiBackground.setOutlineThickness(2.0f);
    uiBackground.setOutlineColor(sf::Color::Black);
    uiBackground.setPosition({450.0f, 10.0f});

    // Next 라벨
    nextLabel = new sf::Text(font); 
    nextLabel->setString("Next");
    nextLabel->setCharacterSize(24);
    nextLabel->setFillColor(sf::Color::Black);
    nextLabel->setPosition({465.0f, 20.0f});

    nextPreviewShape = new sf::CircleShape();
    currentPreviewShape = new sf::CircleShape();

    // 5. 게임 로직 초기화 (랜덤 시드, current/next 티어 설정)
    srand((unsigned int)time(NULL));

    generateNext();          // 우선 nextTier 하나 뽑고
    currentTier = nextTier;  // 그걸 현재 푸앙으로 설정
    updateCurrentPreview();  // 상단 현재 푸앙 미리보기 갱신
    generateNext();          // 다시 nextTier 뽑기
    updateNextPreview();     // 오른쪽 NEXT 미리보기 갱신    
}

Game::~Game() {
    // 포인터 메모리 해제
    delete titleText;
    delete startText;
    delete galleryText;
    delete backText;
    delete bgm;
    delete dropSound;

    delete currentPreviewShape;
    delete nextPreviewShape;
    delete nextLabel;
    delete uiManager;
    delete puangManager;
    delete playfield;
    delete physicsWorld;
}

// 메인 게임 루프
void Game::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

// ---------------------------------------------------------
// [로직] 다음 과일 랜덤 생성
// ---------------------------------------------------------
void Game::generateNext() {
    int r = rand() % 3;
    nextTier = static_cast<PuangTier>(r);
}

// ---------------------------------------------------------
// [로직] Next 미리보기 업데이트
// ---------------------------------------------------------
void Game::updateNextPreview() {
    const sf::Texture& tex = puangManager->getTexture(nextTier);
    
    float realRadius = Puang::radiusFor(nextTier);
    float displayRadius = realRadius; 

    nextPreviewShape->setRadius(displayRadius);
    nextPreviewShape->setOrigin({displayRadius, displayRadius});
    nextPreviewShape->setTexture(&tex);
    nextPreviewShape->setPosition({510.0f, 90.0f});

    sf::Vector2u texSize = tex.getSize();
    int size = std::min(texSize.x, texSize.y);
    int offsetX = (texSize.x - size) / 2;
    int offsetY = (texSize.y - size) / 2;
    nextPreviewShape->setTextureRect(sf::IntRect({offsetX, offsetY}, {size, size}));
}

// ---------------------------------------------------------
// [로직] Current 미리보기 업데이트
// ---------------------------------------------------------
void Game::updateCurrentPreview() {
    const sf::Texture& tex = puangManager->getTexture(currentTier);
    float realRadius = Puang::radiusFor(currentTier);

    currentPreviewShape->setRadius(realRadius);
    currentPreviewShape->setOrigin({realRadius, realRadius});
    currentPreviewShape->setTexture(&tex);
    
    sf::Vector2u texSize = tex.getSize();
    int size = std::min(texSize.x, texSize.y);
    int offsetX = (texSize.x - size) / 2;
    int offsetY = (texSize.y - size) / 2;
    currentPreviewShape->setTextureRect(sf::IntRect({offsetX, offsetY}, {size, size}));
    
    currentPreviewShape->setFillColor(sf::Color(255, 255, 255, 200)); 
}

// ---------------------------------------------------------
// [로직] 마우스 좌표 제한
// ---------------------------------------------------------
float Game::getClampedMouseX() {
    float mouseX = static_cast<float>(sf::Mouse::getPosition(window).x);
    float wallThickness = 20.0f;
    float currentRadius = Puang::radiusFor(currentTier);
    float safetyBuffer = 2.0f;

    float minX = wallThickness + currentRadius + safetyBuffer;
    float maxX = Config::WINDOW_WIDTH - wallThickness - currentRadius - safetyBuffer;

    if (mouseX < minX) mouseX = minX;
    if (mouseX > maxX) mouseX = maxX;
    return mouseX;
}

// ---------------------------------------------------------
// [이벤트] 입력 처리
// ---------------------------------------------------------
void Game::processEvents() {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) window.close();

        if (const auto* mouseBtn = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseBtn->button == sf::Mouse::Button::Left) {
                sf::Vector2f mousePos(static_cast<float>(mouseBtn->position.x), static_cast<float>(mouseBtn->position.y));

                // 1. 메뉴 화면
                if (currentScene == Scene::Menu) {
                    if (startButton.getGlobalBounds().contains(mousePos)) {
                        currentScene = Scene::Playing;
                    }
                    else if (galleryButton.getGlobalBounds().contains(mousePos)) {
                        currentScene = Scene::Gallery;
                    }
                }
                // 2. 도감 화면
                else if (currentScene == Scene::Gallery) {
                    if (backButton.getGlobalBounds().contains(mousePos)) {
                        currentScene = Scene::Menu;
                    }
                }
                // 3. 게임 화면
                else if (currentScene == Scene::Playing) {
                    if (dropTimer <= 0.0f) {
                        float safeX = getClampedMouseX();
                        puangManager->spawnPuang(currentTier, safeX, 200.0f); 
                        
                        if (dropSound) dropSound->play();       // 효과음 재생
                        
                        // 현재/다음 티어 갱신
                        currentTier = nextTier;
                        updateCurrentPreview(); 
                        generateNext();
                        updateNextPreview();
                        dropTimer = 0.5f;    
                    }
                }
                // 4. 게임 오버 상태일 때 클릭하면 재시작
                else if (currentScene == Scene::GameOver) {
                resetGame(); // 초기화
                currentScene = Scene::Menu; // 메뉴로 이동 (또는 바로 Playing)
                }
            }
        }
    }
}

// ---------------------------------------------------------
// [업데이트]
// ---------------------------------------------------------
void Game::update() {
    if (currentScene == Scene::Playing) {
        physicsWorld->update(1.0f / 60.0f);     // Box2D 물리 시뮬레이션
        
        // 푸앙 상태 업데이트 및 병합 처리, 점수 계산
        puangManager->update();
        uiManager->updateScore(puangManager->getScore());

        float limitY = playfield->getLimitLineY();
        
        if (dropTimer > 0.0f) {
            dropTimer -= 1.0f / 60.0f;
        }

        // 제한선을 넘었는가?
        if (puangManager->checkOverflow(limitY)) {
            gameOverTimer += 1.0f / 60.0f; // 시간 누적
            
            // 1초 이상 넘어가 있으면 게임 오버
            if (gameOverTimer > 1.0f) {
                currentScene = Scene::GameOver;
            }
        } else {
            gameOverTimer = 0.0f; // 안 넘었으면 타이머 리셋
        }

        float safeX = getClampedMouseX();
        guideLine.setPosition({safeX, 0});
        currentPreviewShape->setPosition({safeX, 200.0f}); 
    }
}

// ---------------------------------------------------------
// [렌더링]
// ---------------------------------------------------------
void Game::render() {
    window.clear(sf::Color(210, 240, 255)); 

    if (currentScene == Scene::Menu) {
        renderMenu();
    } 
    else if (currentScene == Scene::Gallery) {
        renderGallery();
    }
    else if (currentScene == Scene::GameOver) {
        renderGameOver();
    }
    else {
        renderGame();
    }

    window.display();
}

// ---------------------------------------------------------
// 메뉴 화면 렌더링
// ---------------------------------------------------------
void Game::renderMenu() {
    // 포인터 역참조하여 그리기
    window.draw(*titleText);
    window.draw(startButton);
    window.draw(*startText);
    window.draw(galleryButton);
    window.draw(*galleryText);
}

// ---------------------------------------------------------
// 도감(Gallery) 화면 렌더링
// ---------------------------------------------------------
void Game::renderGallery() {
    window.draw(backButton);
    window.draw(*backText);

    sf::Text nameLabel(font);
    nameLabel.setCharacterSize(14);
    nameLabel.setFillColor(sf::Color::Black);
    
    sf::CircleShape icon;
    float startX = 80.0f;
    float startY = 150.0f;

    for (int i = 0; i <= (int)PuangTier::Puang10; i++) {
        PuangTier tier = static_cast<PuangTier>(i);
        const sf::Texture& tex = puangManager->getTexture(tier);

        float x = startX + (i % 4) * 140.0f;
        float y = startY + (i / 4) * 180.0f;

        float r = 40.0f;
        icon.setRadius(r);
        icon.setOrigin({r, r});
        icon.setPosition({x, y});
        icon.setTexture(&tex);

        sf::Vector2u s = tex.getSize();
        int minSize = std::min(s.x, s.y);
        icon.setTextureRect(sf::IntRect({(int)(s.x-minSize)/2, (int)(s.y-minSize)/2}, {minSize, minSize}));
        
        window.draw(icon);

        nameLabel.setString(getPuangName(tier));
        // size.x, size.y 사용
        sf::FloatRect b = nameLabel.getLocalBounds();
        nameLabel.setOrigin({b.size.x/2.f, b.size.y/2.f});
        nameLabel.setPosition({x, y + 55.0f});
        window.draw(nameLabel);
    }
}

// ---------------------------------------------------------
// 게임 화면(Playing) 렌더링
// ---------------------------------------------------------
void Game::renderGame() {
    playfield->render(window);   
    window.draw(guideLine);      
    puangManager->render(window);
    
    window.draw(uiBackground);
    window.draw(*nextLabel);
    window.draw(*nextPreviewShape);
    window.draw(*currentPreviewShape); 
    
    uiManager->render(window);
}

// ---------------------------------------------------------
// 게임 오버 화면 렌더링
//  - 기존 게임 화면 위에 반투명 레이어 + GAME OVER + 점수 + 재시작 안내
// ---------------------------------------------------------
void Game::renderGameOver() {
    // 1. 게임 화면을 배경으로 흐릿하게 깔아둠
    renderGame(); 
    
    // 2. 반투명 검은 막
    sf::RectangleShape overlay({(float)Config::WINDOW_WIDTH, (float)Config::WINDOW_HEIGHT});
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);

    // 3. GAME OVER 텍스트
    sf::Text overText(font);
    overText.setString("GAME OVER");
    overText.setCharacterSize(60);
    overText.setFillColor(sf::Color::Red);
    overText.setStyle(sf::Text::Bold);
    
    sf::FloatRect b = overText.getLocalBounds();
    overText.setOrigin({b.size.x/2.f, b.size.y/2.f});
    overText.setPosition({Config::WINDOW_WIDTH/2.0f, 300.0f});
    window.draw(overText);

    // 4. 최종 점수
    sf::Text scoreText(font);
    scoreText.setString("Final Score: " + std::to_string(puangManager->getScore()));
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(sf::Color::White);
    b = scoreText.getLocalBounds();
    scoreText.setOrigin({b.size.x/2.f, b.size.y/2.f});
    scoreText.setPosition({Config::WINDOW_WIDTH/2.0f, 400.0f});
    window.draw(scoreText);

    // 5. 재시작 안내
    sf::Text restartText(font);
    restartText.setString("Click to Restart");
    restartText.setCharacterSize(20);
    restartText.setFillColor(sf::Color::Yellow);
    b = restartText.getLocalBounds();
    restartText.setOrigin({b.size.x/2.f, b.size.y/2.f});
    restartText.setPosition({Config::WINDOW_WIDTH/2.0f, 500.0f});
    
    // 깜빡임 효과
    static float time = 0;
    time += 0.02f;
    if ((int)(time * 2) % 2 == 0) window.draw(restartText);
}

// ---------------------------------------------------------
// 게임 재설정 함수
//  - PhysicsWorld / PuangManager를 새로 만들고 점수 및 미리보기 초기화
// ---------------------------------------------------------
void Game::resetGame() {
    // 기존 매니저 삭제 후 재생성 (가장 깔끔한 초기화)
    delete puangManager;
    delete physicsWorld; // 월드도 초기화해야 바디들이 싹 사라짐
    
    physicsWorld = new PhysicsWorld();
    playfield = new Playfield(physicsWorld->getWorld()); // 벽 다시 생성
    puangManager = new PuangManager(physicsWorld->getWorld());
    
    uiManager->updateScore(0);
    gameOverTimer = 0.0f;
    
    // 미리보기 초기화
    generateNext();
    currentTier = nextTier;
    updateCurrentPreview();
    generateNext();
    updateNextPreview();
}

// ---------------------------------------------------------
// 티어 이름 반환
// ---------------------------------------------------------
std::string Game::getPuangName(PuangTier tier) {
    switch(tier) {
        case PuangTier::Puang00: return "Baby";
        case PuangTier::Puang01: return "Sad";
        case PuangTier::Puang02: return "Umbrella";
        case PuangTier::Puang03: return "Vacation";
        case PuangTier::Puang04: return "Watermelon";
        case PuangTier::Puang05: return "Sunglasses";
        case PuangTier::Puang06: return "Tube";
        case PuangTier::Puang07: return "Jacket";
        case PuangTier::Puang08: return "Graduate";
        case PuangTier::Puang09: return "Doctor";
        case PuangTier::Puang10: return "Blue Dragon";
        default: return "Unknown";
    }
}