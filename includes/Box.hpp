#pragma once

#include "Game.hpp"
#include "Candle/RadialLight.hpp"

class Game;

class Box : public sf::Drawable {
	public:
		Box(Game *game, b2Vec2 &checkPos, const sf::Color &color);
		~Box();

		virtual void	draw(sf::RenderTarget& target, sf::RenderStates states) const override;
		bool 			isInView(const sf::View &view) const;
		void			updateLightPosition();

		// ---- setters ----
		void	setInterpolationData(b2Vec2 prevPos, float prevAngle);

		// ---- getters ----
		b2Body*										getBody() const;
		const std::unique_ptr<sf::RectangleShape>	&getShape() const;
		const candle::RadialLight					&getLight() const;
	public:
		struct InterpolationData {
			b2Vec2 _prevPos;
			float _prevAngle;
		};

	private:
		InterpolationData						m_LerpData;
		Game*									m_Game;
		b2Body* 								m_Body;
		std::unique_ptr<sf::RectangleShape>		m_Shape;
		candle::RadialLight						m_Light;
};
