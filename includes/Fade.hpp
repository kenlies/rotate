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
		sf::RectangleShape*	getShape() const;
		sf::Clock			getFadeClock() const;
		int					getFadeCounter() const;
		int					getPhase() const;
		
	private:
		Game*				_game;
		sf::RectangleShape*	_shape;
		sf::Clock			_fadeClock;
		int					_fadeCounter = 255;
};
