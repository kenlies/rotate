#pragma once

#include <SFML/Graphics.hpp>
#include <Candle/RadialLight.hpp>

class Game;

class HUD : public sf::Drawable {
	public:
		HUD(Game *game);
		~HUD();

		void			updateScore(unsigned short score);
		void 			updateFPS();
		void			updateScoreLightIntensity(float deltaTime);

		virtual void	draw(sf::RenderTarget& target, sf::RenderStates states) const override;

		// ---- getters ----
		const unsigned int getFPS() const;
		const candle::RadialLight &getScoreLight() const;

	private:
		Game			*m_Game;

		unsigned int	m_Frame = 0;
		unsigned int	m_FPS = 0;
		sf::Clock 		m_FPSClock;
		sf::Text		m_FPSText;

		sf::Font		m_ModeFont;
		sf::Text		m_ModeText;
	
		sf::Font		m_ScoreFont;
		sf::Text		m_ScoreText;
		sf::Text		m_ScoreAvailableText;
		sf::Text		m_ScoreSlash;

		float			m_LightIntensity = 0.f;
		candle::RadialLight	m_ScoreLight;
};
