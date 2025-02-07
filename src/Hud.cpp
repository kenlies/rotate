#include "../includes/Hud.hpp"

Hud::Hud(Game *game) : _game(game) {
    if (!_scoreFont.loadFromFile(ResourceManager::getAssetFilePath("Even Stevens.ttf"))) {
        std::cerr << "Error: Could not load font 'Even Stevens' from assets." << std::endl;
    }
    _scoreText.setFont(_scoreFont);
    _scoreText.setString("0");
    _scoreText.setCharacterSize(24);
    _scoreText.setOrigin(_scoreText.getGlobalBounds().width / 2, _scoreText.getGlobalBounds().height / 2);
    _scoreText.scale({1.f, 1.5f});
    _scoreText.setFillColor(sf::Color::Yellow);
    _scoreText.setPosition(_game->getWindowSize().x / 2, _game->getWindowSize().y - _game->getWindowSize().y / 8);
    _scoreSlash.setFont(_scoreFont);
    _scoreSlash.setString("/");
    _scoreSlash.setCharacterSize(18);
    _scoreSlash.scale({1.f, 1.8f});
    _scoreSlash.setFillColor(sf::Color::Yellow);
    _scoreSlash.setOrigin(_scoreSlash.getGlobalBounds().width / 2, _scoreSlash.getGlobalBounds().height / 2);
    _scoreSlash.setPosition(_scoreText.getPosition().x + 23, _scoreText.getPosition().y + 32);
    _scoreAvailableText.setFont(_scoreFont);
    _scoreAvailableText.setString(std::to_string(_game->getLevelCoins()));
    _scoreAvailableText.setCharacterSize(16);
    _scoreAvailableText.scale({1.f, 1.5f});
    _scoreAvailableText.setFillColor(sf::Color::Yellow);
    _scoreAvailableText.setOrigin(_scoreAvailableText.getGlobalBounds().width / 2, _scoreAvailableText.getGlobalBounds().height / 2);
    _scoreAvailableText.setPosition(_scoreText.getPosition().x + 32, _scoreText.getPosition().y + 40);

    _scoreLight = std::unique_ptr<candle::RadialLight>(new candle::RadialLight);
    _scoreLight->setColor(sf::Color::White);
    _scoreLight->setRange(90);
    _scoreLight->setPosition(_scoreText.getPosition().x, _scoreText.getPosition().y);
    _scoreLight->setIntensity(0);

}

Hud::~Hud() {

}

void Hud::updateScore(unsigned short score) {
    _scoreText.setString(std::to_string(score));
    _scoreAvailableText.setString(std::to_string(_game->getLevelCoins()));
    if (std::stoi(std::string((_scoreText.getString()))) == 0) {
        _scoreText.setPosition(_game->getWindowSize().x / 2, _game->getWindowSize().y - _game->getWindowSize().y / 8);
        _scoreLight->setPosition(_scoreText.getPosition().x, _scoreText.getPosition().y);
    } else if (std::stoi(std::string((_scoreText.getString()))) == 10) {
        _scoreText.setPosition(_scoreText.getPosition().x + 7 - _scoreText.getString().getSize() * 10, _scoreText.getPosition().y);
        _scoreLight->setPosition(_scoreText.getPosition().x, _scoreText.getPosition().y);
    } else if (std::stoi(std::string((_scoreText.getString()))) == 20) {
        _scoreText.setPosition(_scoreText.getPosition().x + 3 - _scoreText.getString().getSize() * 10, _scoreText.getPosition().y);
        _scoreLight->setPosition(_scoreText.getPosition().x + 10, _scoreText.getPosition().y);
    }
    if (score != 0) {
        _scoreLight->setIntensity(0.4);
    }
}

void Hud::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    _scoreLight->setIntensity(_scoreLight->getIntensity() - 0.02f);
    target.draw(*_scoreLight, states);
	target.draw(_scoreText, states);
    target.draw(_scoreSlash, states);
    target.draw(_scoreAvailableText, states);
}
