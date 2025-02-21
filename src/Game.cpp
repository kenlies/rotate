#include "../includes/Game.hpp"

Game::Game() : 
    m_Window({}, "rotate", sf::Style::Fullscreen, sf::ContextSettings(0, 0, 8)),
    m_WindowSize(m_Window.getSize()),
    m_World(b2Vec2(0.f, Constants::GRAVITY_MAGNITUDE)),
    m_View(sf::Vector2f(m_WindowSize.x / 2, m_WindowSize.y / 2), sf::Vector2f())
{
    m_Window.setMouseCursorVisible(false);
    m_View.setSize(sf::Vector2f(m_WindowSize.x, m_WindowSize.y));

    // ---- create sound buffers ----
    std::shared_ptr<sf::SoundBuffer> bufSoundOne = std::make_shared<sf::SoundBuffer>();
    std::shared_ptr<sf::SoundBuffer> bufSoundTwo = std::make_shared<sf::SoundBuffer>();
    std::shared_ptr<sf::SoundBuffer> bufSoundThree = std::make_shared<sf::SoundBuffer>();
    std::shared_ptr<sf::SoundBuffer> bufExplosionSoundOne = std::make_shared<sf::SoundBuffer>();
    if (bufSoundOne->loadFromFile(ResourceManager::getAssetFilePath("coin_collect1.ogg")) &&
        bufSoundTwo->loadFromFile(ResourceManager::getAssetFilePath("coin_collect2.ogg")) &&
        bufSoundThree->loadFromFile(ResourceManager::getAssetFilePath("coin_collect3.ogg")) &&
        bufExplosionSoundOne->loadFromFile(ResourceManager::getAssetFilePath("coin_explosion.ogg"))) {
        m_CoinSoundBuffers.push_back(bufSoundOne);
        m_CoinSoundBuffers.push_back(bufSoundTwo);
        m_CoinSoundBuffers.push_back(bufSoundThree);
        m_CoinExplosionSoundBuffers.push_back(bufExplosionSoundOne);
    } else {
        std::cout << "Error: Could not load coin sound buffers" << std::endl;
        exit(EXIT_FAILURE);
    }

    // ---- crate player ball ----
    m_Player = std::make_unique<Player>(this);
    m_JumpCoolDownClock.restart();

    // ---- create boxmap ----
    m_BoxMap = std::make_unique<BoxMap>(this);

    // ---- set the listenr for object contacts ----
    m_World.SetContactListener(this);

    // ---- load the first map ----
    m_BoxMap->loadMap(ResourceManager::getLevelFilePath("level") + std::to_string(m_CurrLevel));
    m_Player->getBody()->SetTransform(m_PlayerSpawnPos, 0);

    // ---- create hud ----
    m_Hud = std::make_unique<Hud>(this);
}

Game::~Game() {

}

void Game::updateEvents() {
    m_ScrollWheelInput = None;
    for (auto event = sf::Event(); m_Window.pollEvent(event);) {
        switch (event.type) {
            // window closed
            case sf::Event::Closed:
                m_Window.close();
                break;

            // key pressed
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape || event.key.code == sf::Keyboard::Q) {
                    m_Window.close();
                }
                else if (event.key.code == sf::Keyboard::Space) {
                    m_Jump = true;
                }
                else if (event.key.code == sf::Keyboard::A) {
                    m_Mode == Editor ? m_MoveViewLeft = true : m_RotateLeft = true;
                }
                else if (event.key.code == sf::Keyboard::D) {
                    m_Mode == Editor ? m_MoveViewRight = true : m_RotateRight = true;
                }
                else if ((event.key.code == sf::Keyboard::W) && m_Mode == Editor) {
                    m_MoveViewUp = true;
                }
                else if ((event.key.code == sf::Keyboard::S) && m_Mode == Editor) {
                    m_MoveViewDown = true;
                }
                else if (event.key.code == sf::Keyboard::M) {
                    m_Mode == Play ? m_Mode = Editor : m_Mode = Play; // toggle between editor and play
                }
                else if (event.key.code == sf::Keyboard::Up && m_Mode == Editor) {
                    // change colors of box in editor mode
                    m_BoxColorIndex++;
                    if (m_BoxColorIndex >= m_BoxColors.size()) {
                        m_BoxColorIndex = 0;
                    }
                }
                else if (event.key.code == sf::Keyboard::Down && m_Mode == Editor) {
                    // change colors of box in editor mode
                    m_BoxColorIndex--;
                    if (m_BoxColorIndex < 0) {
                        m_BoxColorIndex = m_BoxColors.size() - 1;
                    }
                }
                else if (event.key.code == sf::Keyboard::Comma && m_Mode == Editor) {
                    std::cout << "Saving map\n";
                    m_BoxMap->saveMap(ResourceManager::getLevelFilePath("tmp"));
                }
                else if (event.key.code == sf::Keyboard::Period && m_Mode == Editor) {
                    std::cout << "Loading map\n";
                    m_BoxMap->loadMap(ResourceManager::getLevelFilePath("tmp"));
                    m_Player->getBody()->SetTransform(m_PlayerSpawnPos, 0);
                }
                break;
            case sf::Event::KeyReleased:
                if (event.key.code == sf::Keyboard::Space) {
                    m_Jump = false;
                } else if (event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::Left) {
                    m_Mode == Editor ? m_MoveViewLeft = false : m_RotateLeft = false;
                }
                else if (event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::Right) {
                    m_Mode == Editor ? m_MoveViewRight = false : m_RotateRight = false;
                }
                else if ((event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up) && m_Mode == Editor) {
                    m_MoveViewUp = false;
                }
                else if ((event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down) && m_Mode == Editor) {
                    m_MoveViewDown = false;
                }
                break;
            case sf::Event::MouseWheelScrolled:
                if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                    if (event.mouseWheelScroll.delta > 0) {
                        m_ScrollWheelInput = ScrollUp;
                    } else {
                        m_ScrollWheelInput = ScrollDown;
                    }
                }
                break; // added this check if actaully works

            default:
                break;
        }
    }
}

void Game::run() {
    sf::Clock frameClock;

    while (m_Window.isOpen()) {
        // ---- handle events ----
        updateEvents();

        m_Hud->updateFPS();
        m_DeltaTime = frameClock.restart();

        switch(m_Mode) {
            case Play:
                updatePlay();
                break;
            case Editor:
                updateEditor();
                break;
        }
    }
}

void Game::doPhysicsStep() {
    // fixed time step
    // max frame time to avoid spiral of death (on slow devices)
    float frameTime = std::min(m_DeltaTime.asSeconds(), 0.25f);

    m_Accumulator += frameTime;
    while (m_Accumulator >= Constants::TIME_STEP) {
        b2Body *playerBody = m_Player->getBody();
        m_Player->setInterpolationData(playerBody->GetPosition(), playerBody->GetAngle());
        for (auto& box : m_Boxes) {
            b2Body *boxBody = box->getBody();
            box->setInterpolationData(boxBody->GetPosition(), boxBody->GetAngle());
        }
        m_World.Step(Constants::TIME_STEP, 8, 2);
        m_Accumulator -= Constants::TIME_STEP;
    }
    m_LerpAlpha = m_Accumulator / Constants::TIME_STEP;
}

void Game::updatePlay() {
    static bool cameraOnPlayer = true;

    // ---- update the physics world ----
    doPhysicsStep();

    // ---- spawn player at spawn position ----
    if (m_LetsRespawn) {
        if (m_WaitTilRespawnClock.getElapsedTime().asSeconds() > 2) {
            m_Player->getBody()->SetTransform(m_PlayerSpawnPos, 0);
            m_Player->getBody()->SetLinearVelocity(b2Vec2(0, 0));
            m_Player->getBody()->SetAngularVelocity(0);
            m_LetsRespawn = false;
            cameraOnPlayer = true;
            m_Fade = std::make_unique<Fade>(this);
            m_LevelCoins = 0;
            m_LevelScore = 0;
            m_BoxMap->loadMap(ResourceManager::getLevelFilePath("level") + std::to_string(m_CurrLevel));
            m_Hud->updateScore(m_LevelScore);
        }
    }

    // ---- do the fadeout effect ----
    if (m_Fade) {
        m_Fade->decrementFadeCounter(m_DeltaTime.asSeconds());
        if (m_Fade->getFadeCounter() <= 0.f) {
            m_Fade = nullptr;
        }
    }

    // ---- collect yellow box ----
    if (m_TouchYellowBox) {
        if (m_TouchYellowBox->GetType() != b2_dynamicBody) {
            playSounds(m_CoinSoundBuffers, m_CoinSounds, 20.f);
        }
        m_TouchYellowBox->SetType(b2_dynamicBody);
        m_TouchYellowBox->ApplyLinearImpulseToCenter(createForce(-Constants::PICKUP_FORCE), true);
        m_TouchYellowBox = nullptr;
    }

    // ---- level clear ----
    if (m_TouchGreenBox) {
        cameraOnPlayer = false;
        m_LetsRespawn = true;
        m_CurrLevel++;
        m_TotalScore += m_LevelScore;
        if (m_CurrLevel >= 4) {
            m_CurrLevel = 1;
            m_TotalScore = 0;
        }
        m_TouchGreenBox->SetType(b2_dynamicBody);
        m_TouchGreenBox->ApplyLinearImpulseToCenter(createForce(-Constants::WIN_FORCE), true);
        m_TouchGreenBox->ApplyTorque(m_Player->getBody()->GetAngle(), true);
        m_TouchGreenBox = nullptr;
        std::cout << "LEVEL CLEAR\n";
    }

    // ---- death by red box ----
    if (m_TouchRedBox) {
        cameraOnPlayer = false;
        m_LetsRespawn = true;
        b2Vec2 explosionForce = createForce(-Constants::EXPLOSION_FORCE);
        b2ContactEdge* contacts = m_TouchRedBox->GetContactList();

        // apply the world breaking effect: break things!!!
        while (contacts) {
            b2Contact* contact = contacts->contact;

            if (contact->IsTouching()) {
                b2Fixture* fixtureA = contact->GetFixtureA();
                b2Fixture* fixtureB = contact->GetFixtureB();

                int* fixtureA_Color = static_cast<int*>(fixtureA->GetBody()->GetUserData());
                int* fixtureB_Color = static_cast<int*>(fixtureB->GetBody()->GetUserData());

                if ((fixtureA_Color && *fixtureA_Color == Constants::RED && fixtureB_Color && *fixtureB_Color == Constants::WHITE) ||
                    fixtureA_Color && *fixtureA_Color == Constants::WHITE && fixtureB_Color && *fixtureB_Color == Constants::RED) {
                        
                    b2Body* whiteBox = (*fixtureA_Color == Constants::WHITE) ? fixtureA->GetBody() : fixtureB->GetBody();
                    whiteBox->SetType(b2_dynamicBody);
                    whiteBox->ApplyLinearImpulseToCenter(explosionForce, true);
                    whiteBox->ApplyTorque(m_Player->getBody()->GetAngle(), true);
                    whiteBox->SetSleepingAllowed(false);
                }
            }
            contacts = contacts->next;
        }
        m_TouchRedBox->ApplyLinearImpulseToCenter(explosionForce, true);
        m_TouchRedBox->ApplyTorque(m_Player->getBody()->GetAngle(), true);
        m_TouchRedBox = nullptr;
        std::cout << "DEATH BY RED BOX\n";
    }

    // ---- death by void ----
    if ((std::abs(m_Player->getBody()->GetLinearVelocity().x) > 40.f
        || std::abs(m_Player->getBody()->GetLinearVelocity().y) > 40.f)
        && !m_LetsRespawn) {
        cameraOnPlayer = false;
        m_LetsRespawn = true;
        m_WaitTilRespawnClock.restart();
        std::cout << "DEATH BY VOID\n"; 
    }

    // ---- rotate the view ----
    if (m_RotateLeft && !m_LetsRespawn) {
        m_RotVel += Constants::ROTATE_SPEED;
    }
    if (m_RotateRight && !m_LetsRespawn) {
        m_RotVel -= Constants::ROTATE_SPEED;
    }
    m_View.rotate(m_RotVel * m_DeltaTime.asSeconds()); // frame rate independent
    m_RotVel *= 0.92; // smoothing rotation

    // ---- jump ----
    if (m_Jump && m_CanJump > 0 && !m_LetsRespawn && m_JumpCoolDownClock.getElapsedTime().asSeconds() > 1.5f) {
        m_Player->getBody()->ApplyLinearImpulseToCenter(createForce(Constants::JUMP_FORCE), false);
        m_JumpCoolDownClock.restart();
    }

    // set the adjusted gravity in the Box2D world
    m_World.SetGravity(createForce(Constants::GRAVITY_MAGNITUDE));

    // make camera follow player with lerp
    if (cameraOnPlayer) {
        sf::Vector2f playerPos = m_Player->getShape()->getPosition();
        sf::Vector2f currentCenter = m_View.getCenter();
        float k = 4.0f;
        float lerpFactor = 1 - std::exp(-k * m_DeltaTime.asSeconds());

        sf::Vector2f interpolatedPos = currentCenter + lerpFactor * (playerPos - currentCenter);
        m_View.setCenter(interpolatedPos);
    }

    m_Hud->updateScoreLightIntensity(m_DeltaTime.asSeconds());

    // ---- draw everything ----
    m_Window.clear();

    drawParticles();
    drawBoxes();
    m_Window.draw(*m_Player);
    if (m_Fade) { m_Window.draw(*m_Fade); }
    m_Window.draw(*m_Hud);

    m_Window.display();
}

void Game::updateEditor() {

    if (m_MoveViewLeft) {
        m_View.move({-Constants::EDITOR_MOVE_SPEED * m_DeltaTime.asSeconds(), 0});
    }
    if (m_MoveViewRight) {
        m_View.move({Constants::EDITOR_MOVE_SPEED * m_DeltaTime.asSeconds(), 0});
    }
    if (m_MoveViewUp) {
        m_View.move({0, -Constants::EDITOR_MOVE_SPEED * m_DeltaTime.asSeconds()});
    }
    if (m_MoveViewDown) {
        m_View.move({0, Constants::EDITOR_MOVE_SPEED * m_DeltaTime.asSeconds()});
    }
    m_Window.setView(m_View);

    sf::Vector2i mousePos = sf::Mouse::getPosition(m_Window);

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        createBox(mousePos, m_BoxColors[m_BoxColorIndex]);
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        removeBox(mousePos);
    }

    // ---- draw everything ----
    m_Window.clear();
    drawBoxes();
    drawGrid();
    drawBoxAtCursor(mousePos); // draw the box to be placed
    m_Window.draw(*m_Hud);

    m_Window.display();
}

void Game::drawParticles() {
    for (auto it = m_BoxParticles.begin(); it != m_BoxParticles.end();) {
        (*it)->update(m_DeltaTime);
        m_Window.draw(*(*it));
        if ((*it)->getCurrLife().asSeconds() > 3) {
            it = m_BoxParticles.erase(it);
            std::cout << "removed particles from vector" << "\n";
            continue;
        }
        ++it;
    }
}

void Game::drawBoxes() {
    // ---- draw boxes ----
    // this draws the oldest boxes first, rather that newest which could be an issue
    for (auto it = m_Boxes.begin(); it != m_Boxes.end(); ) {
        if ((*it)->getShape()->getFillColor() == sf::Color::Cyan && m_Mode == Play) {
            ++it;
            continue;
        }
        if ((*it)->getShape()->getFillColor() == sf::Color::Yellow && m_Mode == Play) {
            (*it)->getShape()->rotate(50.f * m_DeltaTime.asSeconds());
            if ((*it)->getBody()->GetType() == b2_dynamicBody && !(*it)->isInView(m_View)) {
                // emit particles!!
                std::cout << "emit particles" << "\n";
                m_BoxParticles.emplace_back(std::make_unique<BoxParticles>(150, (*it)->getShape()->getPosition()));
                m_World.DestroyBody((*it)->getBody());
                it = m_Boxes.erase(it);
                m_LevelScore++;
                m_Hud->updateScore(m_LevelScore);
                playSounds(m_CoinExplosionSoundBuffers, m_CoinExplosionSounds, 15);
                continue;
            }
        } 
        m_Window.draw(*(*it));

        // ---- draw lighting ----
        if (m_Mode == Play && (*it)->getShape()->getFillColor() != sf::Color(25, 25, 25)) {  // don't draw lighting on dark gray boxes
            (*it)->updateLightPosition();
            m_Window.draw(((*it)->getLight()));
        }
        ++it;
    }
}

void Game::drawGrid() {
    sf::Vector2f topLeft = m_Window.mapPixelToCoords(sf::Vector2i(0, 0));
    sf::Vector2f bottomRight = m_Window.mapPixelToCoords(sf::Vector2i(m_WindowSize.x, m_WindowSize.y));

    // store the lines to be drawed in a vertex array
    sf::VertexArray gridLines(sf::Lines);

    // calculate the vertical lines to be drawn
    for (int x = static_cast<int>(topLeft.x / Constants::BOX_WIDTH) * Constants::BOX_WIDTH; x <= bottomRight.x; x += Constants::BOX_WIDTH) {
        gridLines.append(sf::Vertex(sf::Vector2f(x, topLeft.y), sf::Color::Red));
        gridLines.append(sf::Vertex(sf::Vector2f(x, bottomRight.y), sf::Color::Red));
    }

    // calculate the horizontal lines to be drawn
    for (int y = static_cast<int>(topLeft.y / Constants::BOX_WIDTH) * Constants::BOX_WIDTH; y <= bottomRight.y; y += Constants::BOX_WIDTH) {
        gridLines.append(sf::Vertex(sf::Vector2f(topLeft.x, y), sf::Color::Red));
        gridLines.append(sf::Vertex(sf::Vector2f(bottomRight.x, y), sf::Color::Red));
    }

    // Draw all lines in one call
    m_Window.draw(gridLines);
}

void Game::drawBoxAtCursor(const sf::Vector2i &mousePos) {
    sf::RectangleShape rectangle;

    rectangle.setFillColor(m_BoxColors[m_BoxColorIndex]);
    if (m_BoxColors[m_BoxColorIndex] == sf::Color::Yellow) {
        rectangle.setOrigin(Constants::BOX_WIDTH / 4, Constants::BOX_WIDTH / 4);
        rectangle.setSize({Constants::BOX_WIDTH / 2, Constants::BOX_WIDTH / 2});
    } else {
        rectangle.setOrigin(Constants::BOX_WIDTH / 2, Constants::BOX_WIDTH / 2);
        rectangle.setSize({Constants::BOX_WIDTH, Constants::BOX_WIDTH});
    }
    // need to use mapPixelToCoords to account for distrortion on location after using m_View.move()
    rectangle.setPosition(m_Window.mapPixelToCoords(mousePos));

    m_Window.draw(rectangle);
}

void Game::createBox(const sf::Vector2i &mousePos, const sf::Color &color) {   
    bool available = true;
    sf::Vector2f mousePosConverted = m_Window.mapPixelToCoords(mousePos, m_View);

    // divide mouse by 32 because each box is 32 pixels
    // then floow to round down example:
    // if mouseX = 63 then 63 / 32 ≈ 1.9
    // then we take floor(1.9) which gives us 1, so we are in the second tile
    float mouseX = std::floor(mousePosConverted.x / Constants::BOX_WIDTH);
    float mouseY = std::floor(mousePosConverted.y / Constants::BOX_WIDTH);
    b2Vec2 checkPos((mouseX * Constants::BOX_WIDTH + Constants::BOX_WIDTH / 2) / Constants::SCALE, (mouseY * Constants::BOX_WIDTH + Constants::BOX_WIDTH / 2) / Constants::SCALE);

    // check that the spot isn't already occupied or
    // if there is already a spawn box in the world
	for (int i = 0; i < m_Boxes.size(); i++) {
		if (m_Boxes[i]->getBody() && m_Boxes[i]->getBody()->GetPosition() == checkPos
            || (m_Boxes[i]->getShape()->getFillColor() == sf::Color::Cyan && color == sf::Color::Cyan)) {
			available = false;
			break;
		}
	}

    // create the box
    if (available) {
        if (color == sf::Color::Cyan) {
            m_PlayerSpawnPos = checkPos;
        } else if (color == sf::Color::Yellow) {
            m_LevelCoins++;
        }
        m_Boxes.emplace_back(std::make_shared<Box>(this, checkPos, color));
    }
}

void Game::removeBox(const sf::Vector2i &mousePos) {
    sf::Vector2f mousePosConverted = m_Window.mapPixelToCoords(mousePos, m_View);
    float mouseX = std::floor(mousePosConverted.x / Constants::BOX_WIDTH);
    float mouseY = std::floor(mousePosConverted.y / Constants::BOX_WIDTH);

    b2Vec2 checkPos((mouseX * Constants::BOX_WIDTH + Constants::BOX_WIDTH / 2) / Constants::SCALE, (mouseY * Constants::BOX_WIDTH + Constants::BOX_WIDTH / 2) / Constants::SCALE); // tiled position (grid)

    for (int i = 0; i < m_Boxes.size(); i++) {
        if (m_Boxes[i]->getBody()->GetPosition() == checkPos) {
            m_World.DestroyBody(m_Boxes[i]->getBody());
            m_Boxes[i] = m_Boxes.back();
            m_Boxes.pop_back();
            std::cout << "remove block\n";
            break;
        }
    }
}

// ---- listeners ----
void Game::BeginContact(b2Contact* contact) {

    // don't do anything if the contact doesn't involve player or if the player is respawning
    if ((contact->GetFixtureB()->GetBody() != m_Player->getBody() 
        && contact->GetFixtureA()->GetBody() != m_Player->getBody())) {
        return ;
    }

    int* fixtureA_UserData = static_cast<int*>(contact->GetFixtureA()->GetBody()->GetUserData());
    int* fixtureB_UserData = static_cast<int*>(contact->GetFixtureB()->GetBody()->GetUserData());
    int* color;

    b2Body* box = nullptr;
    if (fixtureA_UserData && *fixtureA_UserData == Constants::PLAYER) {
        box = contact->GetFixtureB()->GetBody();
        color = fixtureB_UserData;
    } else if (fixtureB_UserData && *fixtureB_UserData == Constants::PLAYER) {
        box = contact->GetFixtureA()->GetBody();
        color = fixtureA_UserData;
    }

    // don't don anything if in contact with spawn point
    if (color && *color == Constants::CYAN) {
        return;
    }
    else if (color && *color == Constants::YELLOW) {
        m_TouchYellowBox = box;
        return;
    } else if (color && *color == Constants::RED && !m_LetsRespawn) {
        m_WaitTilRespawnClock.restart();
        m_TouchRedBox = box;
    } else if (color && *color == Constants::GREEN && !m_LetsRespawn) {
        m_WaitTilRespawnClock.restart();
        m_TouchGreenBox = box;
    }
    m_CanJump++;
}

void Game::EndContact(b2Contact* contact) {
    // don't do anything if the contact doesn't involve player or if the player is respawning
    if ((contact->GetFixtureB()->GetBody() != m_Player->getBody() 
        && contact->GetFixtureA()->GetBody() != m_Player->getBody())) {
        return ;
    }

    int* fixtureA_UserData = static_cast<int*>(contact->GetFixtureA()->GetBody()->GetUserData());
    int* fixtureB_UserData = static_cast<int*>(contact->GetFixtureB()->GetBody()->GetUserData());
    int* color;

    b2Body* box = nullptr;
    if (fixtureA_UserData && *fixtureA_UserData == Constants::PLAYER) {
        box = contact->GetFixtureB()->GetBody();
        color = fixtureB_UserData;
    } else if (fixtureB_UserData && *fixtureB_UserData == Constants::PLAYER) {
        box = contact->GetFixtureB()->GetBody();
        color = fixtureA_UserData;
    }

    if (color && *color == Constants::CYAN || color && *color == Constants::YELLOW) {
        return ;
    }

    m_CanJump--;
}

// --- helper/other ----
// calculate force direction in world space to acoount for rotated world
b2Vec2 Game::createForce(float forcePower) const {
    float radians = m_View.getRotation() * Constants::DEG_TO_RAD;
    b2Vec2 force;
    force.x = -forcePower * sin(radians);
    force.y = forcePower * cos(radians);
    return force;
}

void Game::playSounds(const std::vector<std::shared_ptr<sf::SoundBuffer>> &soundBuffers, 
                        std::deque<sf::Sound> &sounds, float volume) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, soundBuffers.size() - 1);
    int rnum = distrib(gen);
    sounds.emplace_back();
    sf::Sound& sound = sounds.back();
    sound.setBuffer(*soundBuffers[rnum]);
    sound.setVolume(volume);
    sounds.back().play();
    // remove stopped sounds
    while (sounds.front().getStatus() == sf::Sound::Stopped) {
        sounds.pop_front();
    }
}

// --- getters ----
// these can't be const because some none const methods
// are called on the return of these
b2World &Game::getWorld() {
    return m_World;
}

sf::RenderWindow &Game::getWindow() {
    return m_Window;
}

sf::Vector2u &Game::getWindowSize() {
    return m_WindowSize;
}

std::vector<std::shared_ptr<Box>> &Game::getBoxes() {
    return m_Boxes;
}

sf::View &Game::getView() {
    return m_View;
}

Game::gameMode Game::getMode() const {
    return m_Mode;
}

int Game::getLevelCoins() const {
    return m_LevelCoins;
}

float Game::getLerpAlpha() const {
    return m_LerpAlpha;
}
