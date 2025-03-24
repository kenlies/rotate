#include "../includes/Fade.hpp"
#include "../includes/Game.hpp"

Fade::Fade(Game* game) : m_Game(game) {
    m_Shape.setFillColor(sf::Color::Black);
	m_Shape.setSize({static_cast<float>(m_Game->getWindow().getWindowSize().x * 4),
						static_cast<float>(m_Game->getWindow().getWindowSize().y * 4)});
	m_Shape.setOrigin(m_Shape.getSize().y / 2, m_Shape.getSize().x / 2);
	m_Shape.setPosition(m_Game->getWindow().getWindowSize().x / 2,
							m_Game->getWindow().getWindowSize().y / 2);
}

Fade::~Fade() {

}

void Fade::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(m_Shape, states);
}

void Fade::setActive() {
	m_Active = true;
	m_FadeCounter = 255.f;
}

void Fade::decrementFadeCounter(float deltaTime) {
	m_FadeCounter -= 200.f * deltaTime;
	m_Shape.setFillColor(sf::Color(0, 0, 0, static_cast<int>(m_FadeCounter)));
	if (m_FadeCounter <= 0.f) {
		m_Active = false;
	}
}

const sf::Clock& Fade::getFadeClock() const {
	return m_FadeClock;
}

float Fade::getFadeCounter() const {
	return m_FadeCounter;
}

bool Fade::getActive() const {
	return m_Active;
}