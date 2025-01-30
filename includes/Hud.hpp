#pragma once

#include "Game.hpp"

class Game;

class Hud : public sf::Drawable, public sf::Transformable {
	public:
		Hud(Game *game);
		~Hud();

		void updateScore(unsigned short score);
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	private:
		Game *_game;
		sf::Font _scoreFont;
		sf::Text _scoreText;
		sf::Font _scoreAvailableFont;
		sf::Text _scoreAvailableText;
};
