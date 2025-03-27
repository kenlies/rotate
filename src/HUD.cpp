#include "../includes/HUD.hpp"
#include "../includes/Game.hpp"
#include "../includes/constants.h"
#include <iostream>

HUD::HUD(Game* game) : m_Game(game) {
    sf::Vector2u& windowSize = m_Game->getWindow().getWindowSize();

    if (!m_ModeFont.loadFromFile(ResourceManager::getAssetFilePath("BebasNeue-Regular.ttf"))) {
        std::cerr << "Error: Could not load font 'BebasNeue-Regular.ttf' from assets." << std::endl;
    }
    m_ModeText.setPosition({5, static_cast<float>(windowSize.y / 64)}); // set position to top left
    m_ModeText.setString("Editor");
    m_ModeText.setFillColor(sf::Color::Red);
    m_ModeText.setFont(m_ModeFont);
    sf::FloatRect textBounds = m_ModeText.getLocalBounds();
    m_ModeText.setOrigin(0, textBounds.height / 2.f);

    m_FPSText.setFont(m_ModeFont);
    m_FPSText.setFillColor(sf::Color::White);
    m_FPSText.setString("fps: 999");
    m_FPSText.setScale({0.5f, 0.5f});
    m_FPSText.setPosition({static_cast<float>(windowSize.x - 60), static_cast<float>(windowSize.y - 30)});

    if (!m_ScoreFont.loadFromFile(ResourceManager::getAssetFilePath("Even Stevens.ttf"))) {
        std::cerr << "Error: Could not load font 'Even Stevens' from assets." << std::endl;
    }
    m_ScoreText.setFont(m_ScoreFont);
    m_ScoreText.setString("0");
    m_ScoreText.setCharacterSize(24);
    m_ScoreText.setOrigin(m_ScoreText.getGlobalBounds().width / 2, m_ScoreText.getGlobalBounds().height / 2);
    m_ScoreText.scale({1.f, 1.5f});
    m_ScoreText.setFillColor(sf::Color::Yellow);
    m_ScoreText.setPosition(windowSize.x / 2, windowSize.y - windowSize.y / 8);
    m_ScoreSlash.setFont(m_ScoreFont);
    m_ScoreSlash.setString("/");
    m_ScoreSlash.setCharacterSize(18);
    m_ScoreSlash.scale({1.f, 1.8f});
    m_ScoreSlash.setFillColor(sf::Color::Yellow);
    m_ScoreSlash.setOrigin(m_ScoreSlash.getGlobalBounds().width / 2, m_ScoreSlash.getGlobalBounds().height / 2);
    m_ScoreSlash.setPosition(m_ScoreText.getPosition().x + 23, m_ScoreText.getPosition().y + 32);
    m_ScoreAvailableText.setFont(m_ScoreFont);
    m_ScoreAvailableText.setString(std::to_string(m_Game->getLevelCoins()));
    m_ScoreAvailableText.setCharacterSize(16);
    m_ScoreAvailableText.scale({1.f, 1.5f});
    m_ScoreAvailableText.setFillColor(sf::Color::Yellow);
    m_ScoreAvailableText.setOrigin(m_ScoreAvailableText.getGlobalBounds().width / 2, m_ScoreAvailableText.getGlobalBounds().height / 2);
    m_ScoreAvailableText.setPosition(m_ScoreText.getPosition().x + 32, m_ScoreText.getPosition().y + 40);

    m_ScoreLight.setColor(sf::Color::White);
    m_ScoreLight.setRange(90);
    m_ScoreLight.setPosition(m_ScoreText.getPosition().x, m_ScoreText.getPosition().y);
    m_ScoreLight.setIntensity(0);
}

HUD::~HUD() {

}

void HUD::update(float deltaTime) {
    updateScoreLightIntensity(deltaTime);
    updateFPS();
}

void HUD::updateFPS() {
	if (m_FPSClock.getElapsedTime().asSeconds() >= 1.f) {
        m_FPS = m_Frame;
		m_Frame = 0;
        m_FPSText.setString("fps: " + std::to_string(m_FPS));
		m_FPSClock.restart();
	}
	++m_Frame;
}

void HUD::refreshScore(unsigned int score) {
    sf::Vector2u& windowSize = m_Game->getWindow().getWindowSize();

    m_ScoreText.setString(std::to_string(score));
    m_ScoreAvailableText.setString(std::to_string(m_Game->getLevelCoins()));
    if (std::stoi(std::string((m_ScoreText.getString()))) == 0) {
        m_ScoreText.setPosition(windowSize.x / 2, windowSize.y - windowSize.y / 8);
        m_ScoreLight.setPosition(m_ScoreText.getPosition().x, m_ScoreText.getPosition().y);
    } else if (std::stoi(std::string((m_ScoreText.getString()))) == 10) {
        m_ScoreText.setPosition(m_ScoreText.getPosition().x + 7 - m_ScoreText.getString().getSize() * 10, m_ScoreText.getPosition().y);
        m_ScoreLight.setPosition(m_ScoreText.getPosition().x, m_ScoreText.getPosition().y);
    } else if (std::stoi(std::string((m_ScoreText.getString()))) == 20) {
        m_ScoreText.setPosition(m_ScoreText.getPosition().x + 3 - m_ScoreText.getString().getSize() * 10, m_ScoreText.getPosition().y);
        m_ScoreLight.setPosition(m_ScoreText.getPosition().x + 10, m_ScoreText.getPosition().y);
    }
    if (score != 0) {
        m_LightIntensity = 0.4f;
    }
}

void HUD::updateScoreLightIntensity(float deltaTime) {
    if (m_LightIntensity > 0.0f) {
        m_LightIntensity = std::max(m_LightIntensity - deltaTime, 0.0f);
        m_ScoreLight.setIntensity(m_LightIntensity);
    }
}

void HUD::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    sf::RenderWindow& window = m_Game->getWindow().getWindow();

    window.setView(window.getDefaultView());  // set default view before drawing

    if (m_Game->getMode() == Game::Play) {
        target.draw(m_ScoreLight, states);
        target.draw(m_ScoreText, states);
        target.draw(m_ScoreSlash, states);
        target.draw(m_ScoreAvailableText, states);
    } else {
        target.draw(m_ModeText, states);
    }
    target.draw(m_FPSText, states);

    window.setView(m_Game->getWindow().getView());  // restore the actual game view
}
