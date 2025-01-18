#include "../includes/Game.hpp"

Game::Game() : 
    _window({}, "Untitled", sf::Style::Fullscreen, sf::ContextSettings(0, 0, 8)),
    _windowSize(_window.getSize()),
    _mode(Play),
    _world(b2Vec2(0.f, GRAVITY_MAGNITUDE)),
    _boxColorIndex(0),
    _scrollWheelInput(None),
    _view(sf::Vector2f(_windowSize.x / 2, _windowSize.y / 2), sf::Vector2f()),
    _rotateRight(false),
    _rotateLeft(false),
    _canJump(0),
    _rotVel(0),
    _currLevel(1)
{
    _window.setFramerateLimit(60);
    _window.setMouseCursorVisible(false);
    _view.setSize(sf::Vector2f(_windowSize.x, _windowSize.y));

    // ---- load fonts and setup texts ----
    if (!_modeFont.loadFromFile(ResourceManager::getAssetFilePath("BebasNeue-Regular.ttf"))) {
        std::cerr << "Error: Could not load font 'BebasNeue-Regular.ttf' from assets." << std::endl;
        exit(EXIT_FAILURE);
    }
    _modeText.setPosition({5, static_cast<float>(_windowSize.y / 64)}); // set position to top left
    _modeText.setString("Editor");
    _modeText.setFillColor(sf::Color::Red);
    _modeText.setFont(_modeFont);
    sf::FloatRect textBounds = _modeText.getLocalBounds();
    _modeText.setOrigin(0, textBounds.height / 2.f);

    // ---- set box colors for editor ----
    _boxColors.emplace_back(sf::Color::White);
    _boxColors.emplace_back(sf::Color::Green);
    _boxColors.emplace_back(sf::Color::Red);
    _boxColors.emplace_back(sf::Color::Cyan);
    _boxColors.emplace_back(sf::Color::Yellow);

    // ---- crate player ball ----
    _player = new Player(this);
    _jumpCoolDownClock.restart();

    // ---- create boxmap ----
    _boxMap = new BoxMap(this);

    // ---- set the listenr for object contacts ----
    _world.SetContactListener(this);

    // ---- load the first map ----
    _boxMap->loadMap(ResourceManager::getLevelFilePath("level") + std::to_string(_currLevel));
    _player->getBody()->SetTransform(_playerSpawnPos, 0);
}

Game::~Game() {
    delete _player;
    delete _boxMap;
    for (Box* box : _boxes) {
        delete box; 
    }
    _boxes.clear();
}

void Game::run() {
    sf::Clock frameClock;

    while (_window.isOpen()) {
        // ---- handle event ----
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

void Game::updatePlay() {
    static bool cameraOnPlayer = true;

    // ---- update the physics world ----
    _world.Step(_deltaTime.asSeconds(), 8, 3); // read somewhere the Box2d should use fixed timestep, so maybe change

    // ---- spawn player at spawn position ----
    if (_letsRespawn) {
        if (_waitTilRespawnClock.getElapsedTime().asSeconds() > 2) {
            _player->getBody()->SetTransform(_playerSpawnPos, 0);
            _player->getBody()->SetLinearVelocity(b2Vec2(0, 0));
            _player->getBody()->SetAngularVelocity(0);
            _letsRespawn = false;
            cameraOnPlayer = true;
            _fade = new Fade(this);
            _fade->getFadeClock().restart();
            _boxMap->loadMap(ResourceManager::getLevelFilePath("level") + std::to_string(_currLevel));
        }
    }

    // ---- do the fadeout effect ----
    if (_fade) {
        if (_fade->getFadeClock().getElapsedTime().asMilliseconds() > 1) {
            _fade->decrementFadeCounter();
            _fade->getFadeClock().restart();
            if (_fade->getFadeCounter() <= 0) {
                delete _fade;
                _fade = nullptr;
            }
        }
    }

    // ---- collect yellow box ----
    if (_touchYellowoBox) {

        float rotation = _view.getRotation();
        float radians = rotation * (b2_pi / 180.0f);
        b2Vec2 force;
        force.x = PICKUP_FORCE * sin(radians);
        force.y = -PICKUP_FORCE * cos(radians);

        _touchYellowoBox->SetType(b2_dynamicBody);
        _touchYellowoBox->ApplyLinearImpulseToCenter(force, true);
        _touchYellowoBox = nullptr;
    }

    // ---- level clear ----
    if (_touchGreenBox) {
        cameraOnPlayer = false;
        _letsRespawn = true;
        _currLevel++;
        if (_currLevel >= 4) {
            _currLevel = 1;
        }
        float rotation = _view.getRotation();
        float radians = rotation * (b2_pi / 180.0f);
        b2Vec2 force;
        force.x = WIN_FORCE * sin(radians);
        force.y = -WIN_FORCE * cos(radians);

        _touchGreenBox->SetType(b2_dynamicBody);
        _touchGreenBox->ApplyLinearImpulseToCenter(force, true);
        _touchGreenBox->ApplyTorque(_player->getBody()->GetAngle(), true);
        _touchGreenBox = nullptr;
        std::cout << "LEVEL CLEAR\n";
    }

    // ---- death by red box ----
    if (_touchRedBox) {
        cameraOnPlayer = false;
        _letsRespawn = true;
        float rotation = _view.getRotation();
        float radians = rotation * (b2_pi / 180.0f);
        b2Vec2 force;
        force.x = EXPLOSION_FORCE * sin(radians);
        force.y = -EXPLOSION_FORCE * cos(radians);

        b2ContactEdge* contacts = _touchRedBox->GetContactList();

        // apply the world breaking effect: break things!!!
        while (contacts) {
            b2Contact* contact = contacts->contact;

            if (contact->IsTouching()) {
                b2Fixture* fixtureA = contact->GetFixtureA();
                b2Fixture* fixtureB = contact->GetFixtureB();

                int* fixtureA_Color = static_cast<int*>(fixtureA->GetBody()->GetUserData());
                int* fixtureB_Color = static_cast<int*>(fixtureB->GetBody()->GetUserData());

                if ((fixtureA_Color && *fixtureA_Color == RED && fixtureB_Color && *fixtureB_Color == WHITE) ||
                    fixtureA_Color && *fixtureA_Color == WHITE && fixtureB_Color && *fixtureB_Color == RED) {
                        
                    b2Body* whiteBox = (*fixtureA_Color == WHITE) ? fixtureA->GetBody() : fixtureB->GetBody();
                    whiteBox->SetType(b2_dynamicBody);
                    whiteBox->ApplyLinearImpulseToCenter(force, true);
                    whiteBox->ApplyTorque(_player->getBody()->GetAngle(), true);
                    whiteBox->SetSleepingAllowed(false);
                }
            }
            contacts = contacts->next;
        }
        _touchRedBox->ApplyLinearImpulseToCenter(force, true);
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
        _rotVel += ROTATE_SPEED;
    }
    if (_rotateRight && !_letsRespawn) {
        _rotVel -= ROTATE_SPEED;
    }
    _view.rotate(_rotVel * _deltaTime.asSeconds()); // frame rate independent
    _rotVel *= 0.92; // smoothing rotation

    float rotation = _view.getRotation();
    // convert rotation angle to radians for trigonometric calculations
    float radians = rotation * (b2_pi / 180.0f);

    // calculate gravity direction in world space to always point to window bottom
    b2Vec2 gravity;
    gravity.x = -GRAVITY_MAGNITUDE * sin(radians);
    gravity.y = GRAVITY_MAGNITUDE * cos(radians);

    // ---- jump ----
    if (_jump && _canJump > 0 && !_letsRespawn && _jumpCoolDownClock.getElapsedTime().asSeconds() > 1.5f) {
        b2Vec2 force;
        force.x = -JUMP_FORCE * sin(radians);
        force.y = JUMP_FORCE * cos(radians); 
        _player->getBody()->ApplyLinearImpulseToCenter(force, false);
        _jumpCoolDownClock.restart();
    }

    // set the adjusted gravity in the Box2D world
    _world.SetGravity(gravity);

    // make camera follow player
    if (cameraOnPlayer) {
        _view.setCenter(_player->getShape()->getPosition());
    }

    // ---- draw everything ----
    _window.clear();
    _window.setView(_view);
    draw_boxes();
    draw_player();
    if (_fade) {
        _fade->draw(_window, sf::RenderStates::Default);
    }
    _window.display();
}

void Game::updateEditor() {

    if (_moveViewLeft) {
        _view.move({-5, 0});
    }
    if (_moveViewRight) {
        _view.move({5, 0});
    }
    if (_moveViewUp) {
        _view.move({0, -5});
    }
    if (_moveViewDown) {
        _view.move({0, 5});
    }
    _window.setView(_view);

    sf::Vector2i mousePos = sf::Mouse::getPosition(_window);

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        createBox(mousePos, _boxColors[_boxColorIndex]);
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        removeBox(mousePos);
    }

    // adjust editor text poistion for view movement
    _modeText.setPosition({_view.getCenter().x - _view.getSize().x / 2 + 10, _view.getCenter().y - _view.getSize().y / 2 + 10}); // add padding

    // ---- draw everything ----
    _window.clear();
    draw_boxes();
    draw_grid();
    draw_box_at_cursor(mousePos); // draw the box to be placed
    _window.draw(_modeText);

    _window.display();
}

void Game::draw_player() {
    _player->getShape()->setPosition(SCALE * _player->getBody()->GetPosition().x, SCALE * _player->getBody()->GetPosition().y);
    _player->getShape()->setRotation(_player->getBody()->GetAngle() * 180 / b2_pi);
    _window.draw(*(_player->getShape()));
}

void Game::draw_boxes() {
    // ---- draw boxes ----
    // this draws the oldest boxes first, rather that newest which could be an issue
    for (auto it : _boxes) {
        if (it->getShape()->getFillColor() == sf::Color::Cyan && _mode == Play) {
            continue;
        }
        it->getShape()->setPosition(SCALE * it->getBody()->GetPosition().x, SCALE * it->getBody()->GetPosition().y);
        if (it->getShape()->getFillColor() == sf::Color::Yellow && _mode == Play) {
            it->getShape()->rotate(50.f * _deltaTime.asSeconds());
        } else {
            it->getShape()->setRotation(it->getBody()->GetAngle() * 180 / b2_pi);
        }
        _window.draw(*(it->getShape()));

        // ---- draw lighting ----
        if (_mode == Play) {
            it->getLight()->setPosition(SCALE * it->getBody()->GetPosition().x, SCALE * it->getBody()->GetPosition().y);
            _window.draw(*(it->getLight()));
        }
    }
}

void Game::draw_grid() {
    sf::Vector2f topLeft = _window.mapPixelToCoords(sf::Vector2i(0, 0));
    sf::Vector2f bottomRight = _window.mapPixelToCoords(sf::Vector2i(_windowSize.x, _windowSize.y));

    // store the lines to be drawed in a vertex array
    sf::VertexArray gridLines(sf::Lines);

    // calculate the vertical lines to be drawn
    for (int x = static_cast<int>(topLeft.x / BOX_WIDTH) * BOX_WIDTH; x <= bottomRight.x; x += BOX_WIDTH) {
        gridLines.append(sf::Vertex(sf::Vector2f(x, topLeft.y), sf::Color::Red));
        gridLines.append(sf::Vertex(sf::Vector2f(x, bottomRight.y), sf::Color::Red));
    }

    // calculate the horizontal lines to be drawn
    for (int y = static_cast<int>(topLeft.y / BOX_WIDTH) * BOX_WIDTH; y <= bottomRight.y; y += BOX_WIDTH) {
        gridLines.append(sf::Vertex(sf::Vector2f(topLeft.x, y), sf::Color::Red));
        gridLines.append(sf::Vertex(sf::Vector2f(bottomRight.x, y), sf::Color::Red));
    }

    // Draw all lines in one call
    _window.draw(gridLines);
}

void Game::draw_box_at_cursor(const sf::Vector2i &mousePos) {
    sf::RectangleShape rectangle;

    rectangle.setFillColor(_boxColors[_boxColorIndex]);
    if (_boxColors[_boxColorIndex] == sf::Color::Yellow) {
        rectangle.setOrigin(BOX_WIDTH / 4, BOX_WIDTH / 4);
        rectangle.setSize({BOX_WIDTH / 2, BOX_WIDTH / 2});
    } else {
        rectangle.setOrigin(BOX_WIDTH / 2, BOX_WIDTH / 2);
        rectangle.setSize({BOX_WIDTH, BOX_WIDTH});
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
    float mouseX = std::floor(mousePosConverted.x / 32.0f);
    float mouseY = std::floor(mousePosConverted.y / 32.0f);
    b2Vec2 checkPos((mouseX * 32 + 16) / SCALE, (mouseY * 32 + 16) / SCALE);

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
        if (color == sf::Color::Cyan)
            _playerSpawnPos = checkPos;

        Box *box = new Box(this, checkPos, color);
        _boxes.push_back(box);
    }
}

void Game::removeBox(const sf::Vector2i &mousePos) {
    sf::Vector2f mousePosConverted = _window.mapPixelToCoords(mousePos, _view);
    float mouseX = std::floor(mousePosConverted.x / 32.0f);
    float mouseY = std::floor(mousePosConverted.y / 32.0f);

    b2Vec2 checkPos((mouseX * 32 + 16) / SCALE, (mouseY * 32 + 16) / SCALE); // tiled position (grid)

    for (int i = 0; i < _boxes.size(); i++) {
        if (_boxes[i]->getBody()->GetPosition() == checkPos) {
            _world.DestroyBody(_boxes[i]->getBody());
            delete _boxes[i];
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
    if (fixtureA_UserData && *fixtureA_UserData == PLAYER) {
        box = contact->GetFixtureB()->GetBody();
        color = fixtureB_UserData;
    } else if (fixtureB_UserData && *fixtureB_UserData == PLAYER) {
        box = contact->GetFixtureA()->GetBody();
        color = fixtureA_UserData;
    }

    // don't don anything if in contact with spawn point
    if (color && *color == CYAN) {
        return;
    }
    else if (color && *color == YELLOW) {
        _touchYellowoBox = box;
        return;
    } else if (color && *color == RED && !_letsRespawn) {
        _waitTilRespawnClock.restart();
        _touchRedBox = box;
    } else if (color && *color == GREEN && !_letsRespawn) {
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
    if (fixtureA_UserData && *fixtureA_UserData == PLAYER) {
        box = contact->GetFixtureB()->GetBody();
        color = fixtureB_UserData;
    } else if (fixtureB_UserData && *fixtureB_UserData == PLAYER) {
        box = contact->GetFixtureB()->GetBody();
        color = fixtureA_UserData;
    }

    if (color && *color == CYAN || color && *color == YELLOW) {
        return ;
    }

    _canJump--;
}

// --- getters ----
// these can't be const because some none const methods
// are called on the return of these
b2World &Game::getWorld() {
    return _world;
}

sf::Vector2u &Game::getWindowSize() {
    return _windowSize;
}

std::vector<Box*> &Game::getBoxes() {
    return _boxes;
}

sf::View &Game::getView() {
    return _view;
}