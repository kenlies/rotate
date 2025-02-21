#pragma once

#include "Game.hpp"

class Game;

class Fade : public sf::Drawable {
	public:
		Fade(Game *game);
		~Fade();

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
		void decrementFadeCounter(float deltaTime);

		// ---- getters ----
		const std::unique_ptr<sf::RectangleShape>	&getShape() const;
		const sf::Clock								&getFadeClock() const;
		float										getFadeCounter() const;
		
	private:
		Game*								m_Game;
		std::unique_ptr<sf::RectangleShape>	m_Shape;
		sf::Clock							m_FadeClock;
		float								m_FadeCounter = 255.f;
};
