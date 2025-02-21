#include "../includes/Fade.hpp"

Fade::Fade(Game *game) : m_Game(game) {
    m_Shape.setFillColor(sf::Color::Black);
	m_Shape.setSize({static_cast<float>(m_Game->getWindowSize().x * 4), static_cast<float>(m_Game->getWindowSize().y * 4)});
	m_Shape.setOrigin(m_Shape.getSize().y / 2, m_Shape.getSize().x / 2);
	m_Shape.setPosition(m_Game->getWindowSize().x / 2, m_Game->getWindowSize().y / 2);
}

Fade::~Fade() {

}

void Fade::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(m_Shape, states);
}

// ---- increment/decrement ----

void Fade::decrementFadeCounter(float deltaTime) {
	m_FadeCounter -= 200.f * deltaTime;
	m_Shape.setFillColor(sf::Color(0, 0, 0, static_cast<int>(m_FadeCounter)));
}

// ---- getters ----
const sf::Clock &Fade::getFadeClock() const {
	return m_FadeClock;
}

float Fade::getFadeCounter() const {
	return m_FadeCounter;
}
