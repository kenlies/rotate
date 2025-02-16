#pragma once

#include "Game.hpp"
#include "Candle/RadialLight.hpp"

class Game;

class Hud : public sf::Drawable {
	public:
		Hud(Game *game);
		~Hud();

		void			updateScore(unsigned short score);
		void 			updateFPS();

		virtual void	draw(sf::RenderTarget& target, sf::RenderStates states) const override;

		// ---- getters ----
		const unsigned int getFPS() const;

	private:
		Game		*_game;

		unsigned int mFrame;
		unsigned int mFps;
		sf::Clock mFPSClock;
		sf::Text	_FPSText;

		sf::Font	_modeFont;
		sf::Text	_modeText;
	
		sf::Font	_scoreFont;
		sf::Text	_scoreText;
		sf::Text	_scoreAvailableText;
		sf::Text	_scoreSlash;

		std::unique_ptr<candle::RadialLight> _scoreLight;
};
