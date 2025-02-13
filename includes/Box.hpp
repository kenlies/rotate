#pragma once

#include "Game.hpp"
#include "Candle/RadialLight.hpp"

class Game;

class Box {
	public:
		Box(Game *game, b2Vec2 &checkPos, const sf::Color &color);
		~Box();

		bool isInView(const sf::View &view) const;

		// ---- getters ----
		b2Body*										getBody() const;
		const std::unique_ptr<sf::RectangleShape>	&getShape() const;
		const std::unique_ptr<candle::RadialLight>	&getLight() const;
		
	private:
		Game*									_game;
		b2Body* 								_body;
		std::unique_ptr<sf::RectangleShape>		_shape;
		std::unique_ptr<candle::RadialLight>	_light;
};
