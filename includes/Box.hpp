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

		// ---- setters ----
		void	setInterpolationData(b2Vec2 prevPos, float prevAngle);

		// ---- getters ----
		b2Body*										getBody() const;
		const std::unique_ptr<sf::RectangleShape>	&getShape() const;
		const std::unique_ptr<candle::RadialLight>	&getLight() const;

	public:
		struct InterpolationData {
			b2Vec2 _prevPos;
			float _prevAngle;
		};

	private:
		InterpolationData						_lerpData;
		Game*									_game;
		b2Body* 								_body;
		std::unique_ptr<sf::RectangleShape>		_shape;
		std::unique_ptr<candle::RadialLight>	_light;
};
