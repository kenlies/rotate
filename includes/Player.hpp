#pragma once

#include "Game.hpp"

class Game;

class Player : public sf::Drawable {
	public:
		Player(Game *game);
		~Player();

		virtual void	draw(sf::RenderTarget& target, sf::RenderStates states) const override;

		// ---- setters ----
		void	setInterpolationData(b2Vec2 prevPos, float prevAngle);

		// ---- getters ----
		b2Body*									getBody() const;
		const std::unique_ptr<sf::CircleShape>	&getShape() const;
	public:
		struct InterpolationData {
			b2Vec2 _prevPos;
			float _prevAngle;
		};

	private:
		InterpolationData					m_LerpData;
		Game*								m_Game;
		b2Body* 							m_Body;
		std::unique_ptr<sf::CircleShape>	m_Shape;
};
