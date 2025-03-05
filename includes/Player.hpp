#pragma once

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

class Game;

class Player : public sf::Drawable {
	public:
		Player(Game *game);
		~Player();

		virtual void	draw(sf::RenderTarget& target, sf::RenderStates states) const override;
		void 			update();

		void			setInterpolationData(const b2Vec2 &prevPos, float prevAngle);

		b2Body*					getBody() const;
		const sf::CircleShape	&getShape() const;
	public:
		struct InterpolationData {
			b2Vec2 _prevPos;
			float _prevAngle;
		};
	
	private:
		void updateTransform();
	private:
		InterpolationData		m_LerpData;
		Game*					m_Game;
		b2Body* 				m_Body;
		sf::CircleShape			m_Shape;
};
