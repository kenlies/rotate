#include "../includes/Fade.hpp"

Fade::Fade(Game *game) : _game(game) {
	_shape = std::make_unique<sf::RectangleShape>();
    _shape->setFillColor(sf::Color::Black);
	_shape->setSize({static_cast<float>(_game->getWindowSize().x * 4), static_cast<float>(_game->getWindowSize().y * 4)});
	_shape->setOrigin(_shape->getSize().y / 2, _shape->getSize().x / 2);
	_shape->setPosition(_game->getWindowSize().x / 2, _game->getWindowSize().y / 2);
}

Fade::~Fade() {

}

void Fade::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	_shape->setFillColor(sf::Color(0, 0, 0, _fadeCounter));
	target.draw(*_shape, states);
}

// ---- increment/decrement ----

void Fade::decrementFadeCounter() {
	_fadeCounter -= 3;
}

// ---- getters ----

const std::unique_ptr<sf::RectangleShape> &Fade::getShape() const {
	return _shape;
}

const sf::Clock &Fade::getFadeClock() const {
	return _fadeClock;
}

int Fade::getFadeCounter() const {
	return _fadeCounter;
}
