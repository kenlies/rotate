#include "../includes/Game.hpp"

Game::Game() : 
    _window({}, "rotate", sf::Style::Fullscreen, sf::ContextSettings(0, 0, 8)),
    _windowSize(_window.getSize()),
    _world(b2Vec2(0.f, Constants::GRAVITY_MAGNITUDE)),
    _view(sf::Vector2f(_windowSize.x / 2, _windowSize.y / 2), sf::Vector2f())
{
    _window.setMouseCursorVisible(false);
    _view.setSize(sf::Vector2f(_windowSize.x, _windowSize.y));

    // ---- create sound buffers ----
    std::shared_ptr<sf::SoundBuffer> bufSoundOne = std::make_shared<sf::SoundBuffer>();
    std::shared_ptr<sf::SoundBuffer> bufSoundTwo = std::make_shared<sf::SoundBuffer>();
    std::shared_ptr<sf::SoundBuffer> bufSoundThree = std::make_shared<sf::SoundBuffer>();
    std::shared_ptr<sf::SoundBuffer> bufExplosionSoundOne = std::make_shared<sf::SoundBuffer>();
    if (bufSoundOne->loadFromFile(ResourceManager::getAssetFilePath("coin_collect1.ogg")) &&
        bufSoundTwo->loadFromFile(ResourceManager::getAssetFilePath("coin_collect2.ogg")) &&
        bufSoundThree->loadFromFile(ResourceManager::getAssetFilePath("coin_collect3.ogg")) &&
        bufExplosionSoundOne->loadFromFile(ResourceManager::getAssetFilePath("coin_explosion.ogg"))) {
        _coinSoundBuffers.push_back(bufSoundOne);
        _coinSoundBuffers.push_back(bufSoundTwo);
        _coinSoundBuffers.push_back(bufSoundThree);
        _coinExplosionSoundBuffers.push_back(bufExplosionSoundOne);
    } else {
        std::cout << "Error: Could not load coin sound buffers" << std::endl;
        exit(EXIT_FAILURE);
    }

    // ---- crate player ball ----
    _player = std::make_unique<Player>(this);
    _jumpCoolDownClock.restart();

    // ---- create boxmap ----
    _boxMap = std::make_unique<BoxMap>(this);

    // ---- set the listenr for object contacts ----
    _world.SetContactListener(this);

    // ---- load the first map ----
    _boxMap->loadMap(ResourceManager::getLevelFilePath("level") + std::to_string(_currLevel));
    _player->getBody()->SetTransform(_playerSpawnPos, 0);

    // ---- create hud ----
    _hud = std::make_unique<Hud>(this);
}

Game::~Game() {

}

void Game::updateEvents() {
    _scrollWheelInput = None;
    for (auto event = sf::Event(); _window.pollEvent(event);) {
        switch (event.type) {
            // window closed
            case sf::Event::Closed:
                _window.close();
                break;

            // key pressed
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape || event.key.code == sf::Keyboard::Q) {
                    _window.close();
                }
                else if (event.key.code == sf::Keyboard::Space) {
                    _jump = true;
                }
                else if (event.key.code == sf::Keyboard::A) {
                    _mode == Editor ? _moveViewLeft = true : _rotateLeft = true;
                }
                else if (event.key.code == sf::Keyboard::D) {
                    _mode == Editor ? _moveViewRight = true : _rotateRight = true;
                }
                else if ((event.key.code == sf::Keyboard::W) && _mode == Editor) {
                    _moveViewUp = true;
                }
                else if ((event.key.code == sf::Keyboard::S) && _mode == Editor) {
                    _moveViewDown = true;
                }
                else if (event.key.code == sf::Keyboard::M) {
                    _mode == Play ? _mode = Editor : _mode = Play; // toggle between editor and play
                }
                else if (event.key.code == sf::Keyboard::Up && _mode == Editor) {
                    // change colors of box in editor mode
                    _boxColorIndex++;
                    if (_boxColorIndex >= _boxColors.size()) {
                        _boxColorIndex = 0;
                    }
                }
                else if (event.key.code == sf::Keyboard::Down && _mode == Editor) {
                    // change colors of box in editor mode
                    _boxColorIndex--;
                    if (_boxColorIndex < 0) {
                        _boxColorIndex = _boxColors.size() - 1;
                    }
                }
                else if (event.key.code == sf::Keyboard::Comma && _mode == Editor) {
                    std::cout << "Saving map\n";
                    _boxMap->saveMap(ResourceManager::getLevelFilePath("tmp"));
                }
                else if (event.key.code == sf::Keyboard::Period && _mode == Editor) {
                    std::cout << "Loading map\n";
                    _boxMap->loadMap(ResourceManager::getLevelFilePath("tmp"));
                    _player->getBody()->SetTransform(_playerSpawnPos, 0);
                }
                break;
            case sf::Event::KeyReleased:
                if (event.key.code == sf::Keyboard::Space) {
                    _jump = false;
                } else if (event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::Left) {
                    _mode == Editor ? _moveViewLeft = false : _rotateLeft = false;
                }
                else if (event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::Right) {
                    _mode == Editor ? _moveViewRight = false : _rotateRight = false;
                }
                else if ((event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up) && _mode == Editor) {
                    _moveViewUp = false;
                }
                else if ((event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down) && _mode == Editor) {
                    _moveViewDown = false;
                }
                break;
            case sf::Event::MouseWheelScrolled:
                if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                    if (event.mouseWheelScroll.delta > 0) {
                        _scrollWheelInput = ScrollUp;
                    } else {
                        _scrollWheelInput = ScrollDown;
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

    while (_window.isOpen()) {
        // ---- handle events ----
        updateEvents();

        _hud->updateFPS();
        _deltaTime = frameClock.restart();

        switch(_mode) {
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
    float frameTime = std::min(_deltaTime.asSeconds(), 0.25f);

    _accumulator += frameTime;
    while (_accumulator >= Constants::TIME_STEP) {
        b2Body *playerBody = _player->getBody();
        _player->setInterpolationData(playerBody->GetPosition(), playerBody->GetAngle());
        for (auto& box : _boxes) {
            b2Body *boxBody = box->getBody();
            box->setInterpolationData(boxBody->GetPosition(), boxBody->GetAngle());
        }
        _world.Step(Constants::TIME_STEP, 8, 2);
        _accumulator -= Constants::TIME_STEP;
    }
    _lerpAlpha = _accumulator / Constants::TIME_STEP;
}

void Game::updatePlay() {
    static bool cameraOnPlayer = true;

    // ---- update the physics world ----
    doPhysicsStep();

    // ---- spawn player at spawn position ----
    if (_letsRespawn) {
        if (_waitTilRespawnClock.getElapsedTime().asSeconds() > 2) {
            _player->getBody()->SetTransform(_playerSpawnPos, 0);
            _player->getBody()->SetLinearVelocity(b2Vec2(0, 0));
            _player->getBody()->SetAngularVelocity(0);
            _letsRespawn = false;
            cameraOnPlayer = true;
            _fade = std::make_unique<Fade>(this);
            _levelCoins = 0;
            _levelScore = 0;
            _boxMap->loadMap(ResourceManager::getLevelFilePath("level") + std::to_string(_currLevel));
            _hud->updateScore(_levelScore);
        }
    }

    // ---- do the fadeout effect ----
    if (_fade) {
        _fade->decrementFadeCounter(_deltaTime.asSeconds());
        if (_fade->getFadeCounter() <= 0.f) {
            _fade = nullptr;
        }
    }

    // ---- collect yellow box ----
    if (_touchYellowoBox) {
        if (_touchYellowoBox->GetType() != b2_dynamicBody) {
            playSounds(_coinSoundBuffers, _coinSounds, 20.f);
        }
        _touchYellowoBox->SetType(b2_dynamicBody);
        _touchYellowoBox->ApplyLinearImpulseToCenter(createForce(-Constants::PICKUP_FORCE), true);
        _touchYellowoBox = nullptr;
    }

    // ---- level clear ----
    if (_touchGreenBox) {
        cameraOnPlayer = false;
        _letsRespawn = true;
        _currLevel++;
        _totalScore += _levelScore;
        if (_currLevel >= 4) {
            _currLevel = 1;
            _totalScore = 0;
        }
        _touchGreenBox->SetType(b2_dynamicBody);
        _touchGreenBox->ApplyLinearImpulseToCenter(createForce(-Constants::WIN_FORCE), true);
        _touchGreenBox->ApplyTorque(_player->getBody()->GetAngle(), true);
        _touchGreenBox = nullptr;
        std::cout << "LEVEL CLEAR\n";
    }

    // ---- death by red box ----
    if (_touchRedBox) {
        cameraOnPlayer = false;
        _letsRespawn = true;
        b2Vec2 explosionForce = createForce(-Constants::EXPLOSION_FORCE);
        b2ContactEdge* contacts = _touchRedBox->GetContactList();

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
                    whiteBox->ApplyTorque(_player->getBody()->GetAngle(), true);
                    whiteBox->SetSleepingAllowed(false);
                }
            }
            contacts = contacts->next;
        }
        _touchRedBox->ApplyLinearImpulseToCenter(explosionForce, true);
        _touchRedBox->ApplyTorque(_player->getBody()->GetAngle(), true);
        _touchRedBox = nullptr;
        std::cout << "DEATH BY RED BOX\n";
    }

    // ---- death by void ----
    if ((std::abs(_player->getBody()->GetLinearVelocity().x) > 40.f
        || std::abs(_player->getBody()->GetLinearVelocity().y) > 40.f)
        && !_letsRespawn) {
        cameraOnPlayer = false;
        _letsRespawn = true;
        _waitTilRespawnClock.restart();
        std::cout << "DEATH BY VOID\n"; 
    }

    // ---- rotate the view ----
    if (_rotateLeft && !_letsRespawn) {
        _rotVel += Constants::ROTATE_SPEED;
    }
    if (_rotateRight && !_letsRespawn) {
        _rotVel -= Constants::ROTATE_SPEED;
    }
    _view.rotate(_rotVel * _deltaTime.asSeconds()); // frame rate independent
    _rotVel *= 0.92; // smoothing rotation

    // ---- jump ----
    if (_jump && _canJump > 0 && !_letsRespawn && _jumpCoolDownClock.getElapsedTime().asSeconds() > 1.5f) {
        _player->getBody()->ApplyLinearImpulseToCenter(createForce(Constants::JUMP_FORCE), false);
        _jumpCoolDownClock.restart();
    }

    // set the adjusted gravity in the Box2D world
    _world.SetGravity(createForce(Constants::GRAVITY_MAGNITUDE));

    // make camera follow player
    if (cameraOnPlayer) {
        _view.setCenter(_player->getShape()->getPosition());
    }

    // ---- draw everything ----
    _window.clear();

    drawParticles();
    drawBoxes();
    _window.draw(*_player);
    if (_fade) { _window.draw(*_fade); }
    _window.draw(*_hud);

    _window.display();
}

void Game::updateEditor() {

    if (_moveViewLeft) {
        _view.move({-Constants::EDITOR_MOVE_SPEED * _deltaTime.asSeconds(), 0});
    }
    if (_moveViewRight) {
        _view.move({Constants::EDITOR_MOVE_SPEED * _deltaTime.asSeconds(), 0});
    }
    if (_moveViewUp) {
        _view.move({0, -Constants::EDITOR_MOVE_SPEED * _deltaTime.asSeconds()});
    }
    if (_moveViewDown) {
        _view.move({0, Constants::EDITOR_MOVE_SPEED * _deltaTime.asSeconds()});
    }
    _window.setView(_view);

    sf::Vector2i mousePos = sf::Mouse::getPosition(_window);

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        createBox(mousePos, _boxColors[_boxColorIndex]);
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        removeBox(mousePos);
    }

    // ---- draw everything ----
    _window.clear();
    drawBoxes();
    drawGrid();
    drawBoxAtCursor(mousePos); // draw the box to be placed
    _window.draw(*_hud);

    _window.display();
}

void Game::drawParticles() {
    for (auto it = _boxParticles.begin(); it != _boxParticles.end();) {
        (*it)->update(_deltaTime);
        _window.draw(*(*it));
        if ((*it)->getCurrLife().asSeconds() > 3) {
            it = _boxParticles.erase(it);
            std::cout << "removed particles from vector" << "\n";
            continue;
        }
        ++it;
    }
}

void Game::drawBoxes() {
    // ---- draw boxes ----
    // this draws the oldest boxes first, rather that newest which could be an issue
    for (auto it = _boxes.begin(); it != _boxes.end(); ) {
        if ((*it)->getShape()->getFillColor() == sf::Color::Cyan && _mode == Play) {
            ++it;
            continue;
        }
        if ((*it)->getShape()->getFillColor() == sf::Color::Yellow && _mode == Play) {
            (*it)->getShape()->rotate(50.f * _deltaTime.asSeconds());
            if ((*it)->getBody()->GetType() == b2_dynamicBody && !(*it)->isInView(_view)) {
                // emit particles!!
                std::cout << "emit particles" << "\n";
                _boxParticles.emplace_back(std::make_unique<BoxParticles>(150, (*it)->getShape()->getPosition()));
                _world.DestroyBody((*it)->getBody());
                it = _boxes.erase(it);
                _levelScore++;
                _hud->updateScore(_levelScore);
                playSounds(_coinExplosionSoundBuffers, _coinExplosionSounds, 15);
                continue;
            }
        } 
        _window.draw(*(*it));

        // ---- draw lighting ----
        if (_mode == Play && (*it)->getShape()->getFillColor() != sf::Color(25, 25, 25)) {  // don't draw lighting on dark gray boxes
            (*it)->getLight()->setPosition(Constants::SCALE * (*it)->getBody()->GetPosition().x, Constants::SCALE * (*it)->getBody()->GetPosition().y);
            _window.draw(*((*it)->getLight()));
        }
        ++it;
    }
}

void Game::drawGrid() {
    sf::Vector2f topLeft = _window.mapPixelToCoords(sf::Vector2i(0, 0));
    sf::Vector2f bottomRight = _window.mapPixelToCoords(sf::Vector2i(_windowSize.x, _windowSize.y));

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
    _window.draw(gridLines);
}

void Game::drawBoxAtCursor(const sf::Vector2i &mousePos) {
    sf::RectangleShape rectangle;

    rectangle.setFillColor(_boxColors[_boxColorIndex]);
    if (_boxColors[_boxColorIndex] == sf::Color::Yellow) {
        rectangle.setOrigin(Constants::BOX_WIDTH / 4, Constants::BOX_WIDTH / 4);
        rectangle.setSize({Constants::BOX_WIDTH / 2, Constants::BOX_WIDTH / 2});
    } else {
        rectangle.setOrigin(Constants::BOX_WIDTH / 2, Constants::BOX_WIDTH / 2);
        rectangle.setSize({Constants::BOX_WIDTH, Constants::BOX_WIDTH});
    }
    // need to use mapPixelToCoords to account for distrortion on location after using _view.move()
    rectangle.setPosition(_window.mapPixelToCoords(mousePos));

    _window.draw(rectangle);
}

void Game::createBox(const sf::Vector2i &mousePos, const sf::Color &color) {   
    bool available = true;
    sf::Vector2f mousePosConverted = _window.mapPixelToCoords(mousePos, _view);

    // divide mouse by 32 because each box is 32 pixels
    // then floow to round down example:
    // if mouseX = 63 then 63 / 32 ≈ 1.9
    // then we take floor(1.9) which gives us 1, so we are in the second tile
    float mouseX = std::floor(mousePosConverted.x / Constants::BOX_WIDTH);
    float mouseY = std::floor(mousePosConverted.y / Constants::BOX_WIDTH);
    b2Vec2 checkPos((mouseX * Constants::BOX_WIDTH + Constants::BOX_WIDTH / 2) / Constants::SCALE, (mouseY * Constants::BOX_WIDTH + Constants::BOX_WIDTH / 2) / Constants::SCALE);

    // check that the spot isn't already occupied or
    // if there is already a spawn box in the world
	for (int i = 0; i < _boxes.size(); i++) {
		if (_boxes[i]->getBody() && _boxes[i]->getBody()->GetPosition() == checkPos
            || (_boxes[i]->getShape()->getFillColor() == sf::Color::Cyan && color == sf::Color::Cyan)) {
			available = false;
			break;
		}
	}

    // create the box
    if (available) {
        if (color == sf::Color::Cyan) {
            _playerSpawnPos = checkPos;
        } else if (color == sf::Color::Yellow) {
            _levelCoins++;
        }
        _boxes.emplace_back(std::make_shared<Box>(this, checkPos, color));
    }
}

void Game::removeBox(const sf::Vector2i &mousePos) {
    sf::Vector2f mousePosConverted = _window.mapPixelToCoords(mousePos, _view);
    float mouseX = std::floor(mousePosConverted.x / Constants::BOX_WIDTH);
    float mouseY = std::floor(mousePosConverted.y / Constants::BOX_WIDTH);

    b2Vec2 checkPos((mouseX * Constants::BOX_WIDTH + Constants::BOX_WIDTH / 2) / Constants::SCALE, (mouseY * Constants::BOX_WIDTH + Constants::BOX_WIDTH / 2) / Constants::SCALE); // tiled position (grid)

    for (int i = 0; i < _boxes.size(); i++) {
        if (_boxes[i]->getBody()->GetPosition() == checkPos) {
            _world.DestroyBody(_boxes[i]->getBody());
            _boxes[i] = _boxes.back();
            _boxes.pop_back();
            std::cout << "remove block\n";
            break;
        }
    }
}

// ---- listeners ----
void Game::BeginContact(b2Contact* contact) {

    // don't do anything if the contact doesn't involve player or if the player is respawning
    if ((contact->GetFixtureB()->GetBody() != _player->getBody() 
        && contact->GetFixtureA()->GetBody() != _player->getBody())) {
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
        _touchYellowoBox = box;
        return;
    } else if (color && *color == Constants::RED && !_letsRespawn) {
        _waitTilRespawnClock.restart();
        _touchRedBox = box;
    } else if (color && *color == Constants::GREEN && !_letsRespawn) {
        _waitTilRespawnClock.restart();
        _touchGreenBox = box;
    }
    _canJump++;
}

void Game::EndContact(b2Contact* contact) {
    // don't do anything if the contact doesn't involve player or if the player is respawning
    if ((contact->GetFixtureB()->GetBody() != _player->getBody() 
        && contact->GetFixtureA()->GetBody() != _player->getBody())) {
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

    _canJump--;
}

// --- helper/other ----
// calculate force direction in world space to acoount for rotated world
b2Vec2 Game::createForce(float forcePower) const {
    float radians = _view.getRotation() * Constants::DEG_TO_RAD;
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
    return _world;
}

sf::RenderWindow &Game::getWindow() {
    return _window;
}

sf::Vector2u &Game::getWindowSize() {
    return _windowSize;
}

std::vector<std::shared_ptr<Box>> &Game::getBoxes() {
    return _boxes;
}

sf::View &Game::getView() {
    return _view;
}

Game::gameMode Game::getMode() const {
    return _mode;
}

int Game::getLevelCoins() const {
    return _levelCoins;
}

float Game::getLerpAlpha() const {
    return _lerpAlpha;
}
