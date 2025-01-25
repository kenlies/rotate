#include "../includes/Fade.hpp"

Fade::Fade(Game *game) : _game(game) {
    sf::RectangleShape *rectangle = new sf::RectangleShape();
    rectangle->setFillColor(sf::Color::Black);
	rectangle->setSize({static_cast<float>(_game->getWindowSize().x * 4), static_cast<float>(_game->getWindowSize().y * 4)});
	rectangle->setOrigin(rectangle->getSize().y / 2, rectangle->getSize().x / 2);
	rectangle->setPosition(_game->getWindowSize().x / 2, _game->getWindowSize().y / 2);
    _shape = rectangle;
}

Fade::~Fade() {
	delete _shape;
}

void Fade::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	_shape->setFillColor(sf::Color(0, 0, 0, _fadeCounter));
	target.draw(*_shape, states);
}

// ---- increment/decrement ----

void Fade::decrementFadeCounter() {
	_fadeCounter -= 4;
}

// ---- getters ----

sf::RectangleShape* Fade::getShape() const {
	return _shape;
}

sf::Clock Fade::getFadeClock() const {
	return _fadeClock;
}

int Fade::getFadeCounter() const {
	return _fadeCounter;
}
