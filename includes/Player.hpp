#pragma once

#include "Game.hpp"

class Game;

class Player : public sf::Drawable {
	public:
		Player(Game *game);
		~Player();

		virtual void	draw(sf::RenderTarget& target, sf::RenderStates states) const override;

		// ---- getters ----
		b2Body*									getBody() const;
		const std::unique_ptr<sf::CircleShape>	&getShape() const;
		
	private:
		Game*								_game;
		b2Body* 							_body;
		std::unique_ptr<sf::CircleShape>	_shape;
};
