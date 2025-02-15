#pragma once

#include "Game.hpp"

class Game;

class Player : public sf::Drawable {
	public:
		Player(Game *game);
		~Player();

		virtual void	draw(sf::RenderTarget& target, sf::RenderStates states) const override;

		struct InterpolationData {
			b2Vec2 _prevPos;
			float _prevAngle;
		};

		// ---- setters ----
		void	setInterpolationData(b2Vec2 prevPos, float prevAngle);

		// ---- getters ----
		b2Body*									getBody() const;
		const std::unique_ptr<sf::CircleShape>	&getShape() const;
		
	private:
		InterpolationData					_lerpData;
		Game*								_game;
		b2Body* 							_body;
		std::unique_ptr<sf::CircleShape>	_shape;
};
