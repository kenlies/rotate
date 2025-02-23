#pragma once

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

class Game;

class Player : public sf::Drawable {
	public:
		Player(Game *game);
		~Player();

		virtual void	draw(sf::RenderTarget& target, sf::RenderStates states) const override;
		void			updatePosition();

		// ---- setters ----
		void	setInterpolationData(b2Vec2 prevPos, float prevAngle);

		// ---- getters ----
		b2Body*					getBody() const;
		const	sf::CircleShape	&getShape() const;
	public:
		struct InterpolationData {
			b2Vec2 _prevPos;
			float _prevAngle;
		};

	private:
		InterpolationData		m_LerpData;
		Game*					m_Game;
		b2Body* 				m_Body;
		sf::CircleShape			m_Shape;
};
