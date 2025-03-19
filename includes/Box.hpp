#pragma once

#include <Candle/RadialLight.hpp>
#include <box2d/box2d.h>

class Game;

class Box : public sf::Drawable {
	public:
		Box(Game* game, b2Vec2& checkPos, const sf::Color& color);
		~Box();

		virtual void	draw(sf::RenderTarget& target, sf::RenderStates states) const override;
		void			update(float deltaTime);
		bool 			isInView(const sf::View& view) const;

		void			setInterpolationData(const b2Vec2& prevPos, float prevAngle);

		b2Body*						getBody() const;
		const sf::RectangleShape&	getShape() const;
		const candle::RadialLight&	getLight() const;
	public:
		struct InterpolationData {
			b2Vec2 _prevPos;
			float _prevAngle;
		};

	private:
		InterpolationData						m_LerpData;
		Game*									m_Game;
		b2Body* 								m_Body;
		sf::RectangleShape						m_Shape;
		candle::RadialLight						m_Light;
};
