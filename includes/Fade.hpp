#pragma once

#include <SFML/Graphics.hpp>

class Game;

class Fade : public sf::Drawable {
	public:
		Fade(Game *game);
		~Fade();

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
		void decrementFadeCounter(float deltaTime);

		void	setActive();

		// ---- getters ----
		const sf::Clock								&getFadeClock() const;
		float										getFadeCounter() const;
		bool										getActive() const;
		
	private:
		Game*								m_Game;
		sf::RectangleShape					m_Shape;
		sf::Clock							m_FadeClock;
		float								m_FadeCounter;
		bool								m_Active = false;
};
