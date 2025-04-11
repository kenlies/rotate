#include "../includes/Game.hpp"
#include "../includes/Box.hpp"
#include "../includes/BoxParticles.hpp"
#include "../includes/constants.h"
#include "../includes/utils.h"
#include <iostream>

Game::Game() : 
    m_World(b2Vec2(0.f, Constants::GRAVITY_MAGNITUDE)),
    m_Window(this),
    m_Player(this),
    m_BoxMap(this),
    m_Hud(this),
    m_Fade(this),
    m_Grid(this),
    m_Renderer(m_Window.getWindow())
{
    // ---- create sound buffers ----
    m_SoundManager.loadSound("pickup1", "coin_collect1.ogg");
    m_SoundManager.loadSound("pickup2", "coin_collect2.ogg");
    m_SoundManager.loadSound("pickup3", "coin_collect3.ogg");
    m_SoundManager.loadSound("explosion", "coin_explosion.ogg");

    // ---- set the listenr for object contacts ----
    m_World.SetContactListener(&m_Player);

    // ---- load the first map ----
    m_BoxMap.loadMap(ResourceManager::getLevelFilePath("level") + std::to_string(m_CurrLevel));

    // ---- set player pos to spawn ----
    m_Player.getBody()->SetTransform(m_Player.getSpawnPos(), 0);
}

Game::~Game() {

}

// some input processing happens already in the pollEvents() function in the Window class
// due to that for some inputs I only want to register one, for instance the "M" key unlike on
// movement where I want to register for as long as the key is pressed
void Game::processInput() {
    if (m_BoxColorIndex < 0) {
        m_BoxColorIndex = m_BoxColors.size() - 1;
    } else if (m_BoxColorIndex >= m_BoxColors.size()) {
        m_BoxColorIndex = 0;
    }
    if (m_Window.isKeyPressed(sf::Keyboard::Q) || m_Window.isKeyPressed(sf::Keyboard::Escape)) {
        m_Window.getWindow().close();
    }
    m_Window.processViewMovement(m_DeltaTime.asSeconds());
    if (m_Window.getChangeMode()) {
        m_Mode == Play ? m_Mode = Editor : m_Mode = Play; // toggle between editor and play
        m_Window.setChangeMode(false);
    }
    m_Player.processInput(m_DeltaTime.asSeconds());
}

void Game::run() {
    sf::Clock frameClock;

    while (m_Window.isOpen()) {
        m_DeltaTime = frameClock.restart();

        // ---- poll events ----
        m_Window.pollEvents();
        // ---- process input ----
        processInput();
        // ---- update ----
        switch(m_Mode) {
            case Play:
                updatePlay();
                break;
            case Editor:
                updateEditor();
                break;
        }

        // ---- render -----
        m_Renderer.renderFrame();
    }
}

void Game::doPhysicsStep() {
    // fixed time step
    // max frame time to avoid spiral of death (on slow devices)
    float frameTime = std::min(m_DeltaTime.asSeconds(), 0.25f);

    m_Accumulator += frameTime;
    while (m_Accumulator >= Constants::TIME_STEP) {
        b2Body* playerBody = m_Player.getBody();
        m_Player.setInterpolationData(playerBody->GetPosition(), playerBody->GetAngle());
        for (auto& box : m_Boxes) {
            b2Body* boxBody = box->getBody();
            box->setInterpolationData(boxBody->GetPosition(), boxBody->GetAngle());
        }
        m_World.Step(Constants::TIME_STEP, 8, 2);
        m_Accumulator -= Constants::TIME_STEP;
    }
    m_LerpAlpha = m_Accumulator / Constants::TIME_STEP;
}

void Game::updatePlay() {
    float radians = m_Window.getView().getRotation() * Constants::DEG_TO_RAD;

    // ---- update the physics world ----
    doPhysicsStep();

    // ---- do the fadeout effect ----
    if (m_Fade.getActive()) {
        m_Fade.decrementFadeCounter(m_DeltaTime.asSeconds());
    }

    // set the adjusted gravity in the Box2D world 
    m_World.SetGravity(Utils::createForce(Constants::GRAVITY_MAGNITUDE, radians));

    m_Hud.update(m_DeltaTime.asSeconds());
    m_Player.update(radians);
    updateBoxes();
    
    // ---- add to render queue ----
    addParticlesToRenderQueue();
    addBoxesToRenderQueue();
    m_Renderer.addToRenderQueue(m_Player);
    if (m_Fade.getActive()) { m_Renderer.addToRenderQueue(m_Fade); }
    m_Renderer.addToRenderQueue(m_Hud);
}

void Game::updateEditor() {
    sf::Vector2i mousePos = sf::Mouse::getPosition(m_Window.getWindow());

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        createBox(mousePos, m_BoxColors[m_BoxColorIndex]);
    } else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        removeBox(mousePos);
    }

    m_Grid.update();
    m_Hud.update(m_DeltaTime.asSeconds());
    updateBoxes();

    // ---- add to render queue ----
    addBoxesToRenderQueue();
    m_Renderer.addToRenderQueue(m_Grid);
    addCursorBoxToRenderQueue(mousePos); // draw the box to be placed
    m_Renderer.addToRenderQueue(m_Hud);
}

void Game::addParticlesToRenderQueue() {
    for (auto it = m_BoxParticles.begin(); it != m_BoxParticles.end();) {
        if ((*it)->getCurrLife().asSeconds() > 3) {
            it = m_BoxParticles.erase(it);
        } else {
            (*it)->update(m_DeltaTime);
            m_Renderer.addToRenderQueue(*(*it));
            ++it;
        }
    }
}

void Game::updateBoxes() {
    for (auto it = m_Boxes.begin(); it != m_Boxes.end();) {
        (*it)->update(m_DeltaTime.asSeconds());
        if ((*it)->getShape().getFillColor() == sf::Color::Yellow && m_Mode == Play &&
            (*it)->getBody()->GetType() == b2_dynamicBody && !(*it)->isInView(m_Window.getView())) {
            m_BoxParticles.emplace_back(std::make_unique<BoxParticles>(150, (*it)->getShape().getPosition()));
            m_World.DestroyBody((*it)->getBody());
            it = m_Boxes.erase(it);
            m_LevelScore++;
            m_Hud.refreshScore(m_LevelScore);
            m_SoundManager.playSound("explosion", 60.f);
            continue;
        }
        ++it;
    }
}

void Game::addBoxesToRenderQueue() {
    // ---- draw boxes ----
    // this draws the oldest boxes first, rather that newest which could be an issue
    for (auto it = m_Boxes.begin(); it != m_Boxes.end(); it++) {
        m_Renderer.addToRenderQueue(*(*it));
    }
}

void Game::addCursorBoxToRenderQueue(const sf::Vector2i& mousePos) {
    m_CursorBox.setFillColor(m_BoxColors[m_BoxColorIndex]);
    if (m_BoxColors[m_BoxColorIndex] == sf::Color::Yellow) {
        m_CursorBox.setOrigin(Constants::BOX_WIDTH / 4, Constants::BOX_WIDTH / 4);
        m_CursorBox.setSize({Constants::BOX_WIDTH / 2, Constants::BOX_WIDTH / 2});
    } else {
        m_CursorBox.setOrigin(Constants::BOX_WIDTH / 2, Constants::BOX_WIDTH / 2);
        m_CursorBox.setSize({Constants::BOX_WIDTH, Constants::BOX_WIDTH});
    }
    // need to use mapPixelToCoords to account for distrortion on location after using m_View.move()
    m_CursorBox.setPosition(m_Window.getWindow().mapPixelToCoords(mousePos));
    m_Renderer.addToRenderQueue(m_CursorBox);
}

void Game::createBox(const sf::Vector2i& mousePos, const sf::Color& color) {   
    bool available = true;
    sf::Vector2f mousePosConverted = m_Window.getWindow().mapPixelToCoords(mousePos, m_Window.getView());

    // divide mouse by 32 because each box is 32 pixels
    // then floow to round down example:
    // if mouseX = 63 then 63 / 32 ≈ 1.9
    // then we take floor(1.9) which gives us 1, so we are in the second tile
    float mouseX = std::floor(mousePosConverted.x / Constants::BOX_WIDTH);
    float mouseY = std::floor(mousePosConverted.y / Constants::BOX_WIDTH);
    b2Vec2 checkPos((mouseX * Constants::BOX_WIDTH + Constants::BOX_WIDTH / 2) / Constants::SCALE, (mouseY * Constants::BOX_WIDTH + Constants::BOX_WIDTH / 2) / Constants::SCALE);

    // check that the spot isn't already occupied or
    // if there is already a spawn box in the world
	for (size_t i = 0; i < m_Boxes.size(); i++) {
		if (m_Boxes[i]->getBody() && m_Boxes[i]->getBody()->GetPosition() == checkPos
            || (m_Boxes[i]->getShape().getFillColor() == sf::Color::Cyan && color == sf::Color::Cyan)) {
			available = false;
			break;
		}
	}

    // create the box
    if (available) {
        if (color == sf::Color::Cyan) {
            m_Player.setSpawnPos(checkPos); // setting spawn position
        } else if (color == sf::Color::Yellow) {
            m_LevelCoins++;
        }
        m_Boxes.emplace_back(std::make_shared<Box>(this, checkPos, color));
    }
}

void Game::removeBox(const sf::Vector2i& mousePos) {
    sf::Vector2f mousePosConverted = m_Window.getWindow().mapPixelToCoords(mousePos, m_Window.getView());
    float mouseX = std::floor(mousePosConverted.x / Constants::BOX_WIDTH);
    float mouseY = std::floor(mousePosConverted.y / Constants::BOX_WIDTH);

    b2Vec2 checkPos((mouseX * Constants::BOX_WIDTH + Constants::BOX_WIDTH / 2) / Constants::SCALE, (mouseY * Constants::BOX_WIDTH + Constants::BOX_WIDTH / 2) / Constants::SCALE); // tiled position (grid)

    for (size_t i = 0; i < m_Boxes.size(); i++) {
        if (m_Boxes[i]->getBody()->GetPosition() == checkPos) {
            m_World.DestroyBody(m_Boxes[i]->getBody());
            m_Boxes[i] = m_Boxes.back();
            m_Boxes.pop_back();
            break;
        }
    }
}

void Game::setSrollWheelInput(scrollWheelInput scrollInput) {
    m_ScrollWheelInput = scrollInput;
}

void Game::setMode(gameMode mode) {
    m_Mode = mode;
}

void Game::setBoxColorIndex(int value) {
    m_BoxColorIndex = value;
}

void Game::setCurrLevel(unsigned int value) {
    m_CurrLevel = value;
}

void Game::setLevelCoins(unsigned int value) {
    m_LevelCoins = value;
}

void Game::setLevelScore(unsigned int value) {
    m_LevelScore = value;
}

void Game::setTotalScore(unsigned int value) {
    m_TotalScore = value;
}

Window& Game::getWindow() {
    return m_Window;
}

b2World& Game::getWorld() {
    return m_World;
}

std::vector<std::shared_ptr<Box>>& Game::getBoxes() {
    return m_Boxes;
}

Game::gameMode Game::getMode() const {
    return m_Mode;
}

int Game::getBoxColorIndex() const {
    return m_BoxColorIndex;
}

unsigned int Game::getCurrLevel() const {
    return m_CurrLevel;
}

unsigned int Game::getLevelCoins() const {
    return m_LevelCoins;
}

unsigned int Game::getLevelScore() const {
    return m_LevelScore;
}

unsigned int Game::getTotalScore() const {
    return m_TotalScore;
}

float Game::getLerpAlpha() const {
    return m_LerpAlpha;
}

const Player& Game::getPlayer() const {
    return m_Player;
}

BoxMap& Game::getBoxMap() {
    return m_BoxMap;
}

Fade& Game::getFade() {
    return m_Fade;
}

HUD& Game::getHud() {
    return m_Hud;
}

SoundManager& Game::getSoundManager() {
    return m_SoundManager;
}
