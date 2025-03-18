#pragma once

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include "Window.hpp"
#include "Renderer.hpp"
#include "ResourceManager.hpp"
#include "SoundManager.hpp"
#include "Player.hpp"
#include "HUD.hpp"
#include "Fade.hpp"
#include "Grid.hpp"
#include "BoxMap.hpp"

class Box;
class BoxParticles;

class Game {
	public:
		enum gameMode { Play, Editor };
		enum scrollWheelInput { ScrollUp, ScrollDown, None };
	public:
		Game();
		~Game();

		void 								run();
		void 								createBox(const sf::Vector2i &mousePos, const sf::Color &color);
		void 								removeBox(const sf::Vector2i &mousePos);

		void								setSrollWheelInput(scrollWheelInput scrollInput);
		void								setMode(gameMode mode);
		void								setBoxColorIndex(int value);
		void								setCurrLevel(unsigned int value);
		void								setLevelCoins(unsigned int value);
		void								setLevelScore(unsigned int value);
		void								setTotalScore(unsigned int value);

		Window								&getWindow();
		b2World 							&getWorld();
		std::vector<std::shared_ptr<Box>>	&getBoxes();
		gameMode 							getMode() const;
		int									getBoxColorIndex() const;
		unsigned int						getCurrLevel() const;
		unsigned int						getLevelCoins() const;
		unsigned int						getLevelScore() const;
		unsigned int						getTotalScore() const;
		float								getLerpAlpha() const;
		const Player						&getPlayer() const;
		BoxMap								&getBoxMap();
		Fade								&getFade();
		HUD									&getHud();
		SoundManager						&getSoundManager();

	private:
		void doPhysicsStep();
		void processInput();
		void updatePlay();
		void updateEditor();
		void updateBoxes();
		void addParticlesToRenderQueue();
		void addBoxesToRenderQueue();

		// ---- editor ----
		void addCursorBoxToRenderQueue(const sf::Vector2i &mousePos);
	private:
		Window				m_Window;
		gameMode			m_Mode = Play;
		b2World				m_World;
		sf::Time			m_DeltaTime;
		Player				m_Player;
		BoxMap				m_BoxMap;
		Fade 				m_Fade;
		HUD 				m_Hud;
		SoundManager		m_SoundManager;
		Renderer			m_Renderer;

		std::vector<std::shared_ptr<Box>>	m_Boxes;

		unsigned int	m_CurrLevel = 1;
		unsigned int	m_LevelCoins = 0;
		unsigned int	m_LevelScore = 0;
		unsigned int	m_TotalScore = 0;

		// ---- physics interpolation ----
		float				m_Accumulator = 0;
		float				m_LerpAlpha;

		// ---- particles ----
		std::vector<std::shared_ptr<BoxParticles>> m_BoxParticles;

		// ---- editor ----
		Grid						m_Grid;
		sf::RectangleShape			m_CursorBox;
		int							m_BoxColorIndex = 0;
		std::array<sf::Color, 6>	m_BoxColors = {sf::Color::White, 
													sf::Color::Green,
											  		sf::Color::Red,
											  		sf::Color::Cyan,
											 		sf::Color::Yellow,
											  		sf::Color(25, 25, 25)}; // <-- dark gray
		scrollWheelInput	m_ScrollWheelInput = None;
};
