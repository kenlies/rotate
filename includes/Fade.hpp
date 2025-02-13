#pragma once

#include "Game.hpp"

class Game;

class Fade : public sf::Drawable, public sf::Transformable {
	public:
		Fade(Game *game);
		~Fade();

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

		// ---- increment/decrement ----
		void decrementFadeCounter();

		// ---- getters ----
		const std::unique_ptr<sf::RectangleShape>	&getShape() const;
		const sf::Clock								&getFadeClock() const;
		int											getFadeCounter() const;
		
	private:
		Game*								_game;
		std::unique_ptr<sf::RectangleShape>	_shape;
		sf::Clock							_fadeClock;
		int									_fadeCounter = 255;
};
