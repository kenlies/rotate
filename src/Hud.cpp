#include "../includes/Hud.hpp"

Hud::Hud(Game *game) : _game(game) {
    if (!_scoreFont.loadFromFile(ResourceManager::getAssetFilePath("square-deal.ttf"))) {
        std::cerr << "Error: Could not load font 'square-deal.ttf' from assets." << std::endl;
    }
    _scoreText.setFont(_scoreFont);
    _scoreText.setString("0");
    _scoreText.setCharacterSize(48);
    _scoreText.setFillColor(sf::Color::Yellow);
    _scoreText.setOrigin(_scoreText.getGlobalBounds().width / 2, _scoreText.getGlobalBounds().height / 2);
    _scoreText.setPosition(_game->getWindowSize().x / 2, _game->getWindowSize().y - _game->getWindowSize().y / 8);
    if (!_scoreAvailableFont.loadFromFile(ResourceManager::getAssetFilePath("BebasNeue-Regular.ttf"))) {
        std::cerr << "Error: Could not load font 'BebasNeue-Regular.ttf' from assets." << std::endl;
    }
    _scoreAvailableText.setFont(_scoreAvailableFont);
    _scoreAvailableText.setString("/ " + std::to_string(_game->getLevelCoins()));
    _scoreAvailableText.setCharacterSize(24);
    _scoreAvailableText.setFillColor(sf::Color::Yellow);
    _scoreAvailableText.setOrigin(_scoreAvailableText.getGlobalBounds().width / 2, _scoreAvailableText.getGlobalBounds().height / 2);
    _scoreAvailableText.setPosition(_scoreText.getPosition().x + 25, _scoreText.getPosition().y + 12);
}

Hud::~Hud() {

}

void Hud::updateScore(int score) {
    _scoreText.setString(std::to_string(score));
    _scoreAvailableText.setString("/ " + std::to_string(_game->getLevelCoins()));
    if (std::stoi(std::string((_scoreText.getString()))) >= 20) {
        _scoreAvailableText.setPosition(_scoreText.getPosition().x + 25 + _scoreText.getString().getSize() * 10, _scoreText.getPosition().y + 12);
    } else {
        _scoreAvailableText.setPosition(_scoreText.getPosition().x + 15 + _scoreText.getString().getSize() * 10, _scoreText.getPosition().y + 12);
    }
}

void Hud::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(_scoreText, states);
    target.draw(_scoreAvailableText, states);
}
