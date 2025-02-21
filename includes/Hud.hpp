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
		Game			*m_Game;

		unsigned int	m_Frame;
		unsigned int	m_Fps;
		sf::Clock 		m_FPSClock;
		sf::Text		m_FPSText;

		sf::Font		m_ModeFont;
		sf::Text		m_ModeText;
	
		sf::Font		m_ScoreFont;
		sf::Text		m_ScoreText;
		sf::Text		m_ScoreAvailableText;
		sf::Text		m_ScoreSlash;

		std::unique_ptr<candle::RadialLight> m_ScoreLight;
};
