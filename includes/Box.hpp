#pragma once

#include "Game.hpp"
#include "Candle/RadialLight.hpp"

class Game;

class Box {
	public:
		Box(Game *game, b2Vec2 &checkPos, const sf::Color &color);
		~Box();

		// ---- getters ----
		b2Body*				getBody() const;
		sf::RectangleShape*	getShape() const;
		candle::RadialLight* getLight() const;
		
	private:
		Game*				_game;
		b2Body* 			_body;
		sf::RectangleShape*	_shape;
		candle::RadialLight* _light;
};
