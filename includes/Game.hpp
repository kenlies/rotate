#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <box2d/box2d.h>
#include <iostream>
#include <memory>
#include <random>
#include <algorithm>
#include "constants.h"
#include "ResourceManager.hpp"
#include "Player.hpp"
#include "Box.hpp"
#include "Hud.hpp"
#include "Fade.hpp"
#include "BoxMap.hpp"
#include "BoxParticles.hpp"
#include "Candle/RadialLight.hpp"

class Player;
class Hud;
class Box;
class Fade;
class BoxMap;
class BoxParticles;

class Game : public b2ContactListener {
	public:
		enum gameMode { Play, Editor };
	public:
		Game();
		~Game();

		void run();
		void createBox(const sf::Vector2i &mousePos, const sf::Color &color);
		void removeBox(const sf::Vector2i &mousePos);

		// ---- getters ----
		b2World 							&getWorld();
		sf::RenderWindow					&getWindow();
		sf::Vector2u 						&getWindowSize();
		std::vector<std::shared_ptr<Box>>	&getBoxes();
		sf::View							&getView();
		gameMode 							getMode() const;
		int									getLevelCoins() const;
		float								getLerpAlpha() const;

		// ---- listeners ----
		void BeginContact(b2Contact* contact) override; // override the base class functions
		void EndContact(b2Contact* contact) override;	// override the base class functions

	private:
		void doPhysicsStep();
		void updateEvents();
		void updatePlay();
		void updateEditor();
		void drawParticles();
		void drawBoxes();

		// ---- editor ----
		void drawGrid();
		void drawBoxAtCursor(const sf::Vector2i &mousePos);
		
		// ---- helper/other ----
		b2Vec2 createForce(float force) const;
		void playSounds(const std::vector<std::shared_ptr<sf::SoundBuffer>> &soundBuffers,
							std::deque<sf::Sound> &sounds, float volume);
	private:
		enum scrollWheelInput {
			ScrollUp,
			ScrollDown,
			None
		};

		gameMode			m_Mode = Play;
		b2World				m_World;

		sf::Time			m_DeltaTime;

		// ---- interpolation ----
		float				m_Accumulator = 0;
		float				m_LerpAlpha;

		std::vector<std::shared_ptr<Box>> 				m_Boxes;
		std::unique_ptr<BoxMap>							m_BoxMap;
		std::vector<std::shared_ptr<sf::SoundBuffer>>	m_CoinSoundBuffers;
		std::deque<sf::Sound>							m_CoinSounds;
		std::vector<std::shared_ptr<sf::SoundBuffer>>	m_CoinExplosionSoundBuffers;
		std::deque<sf::Sound>							m_CoinExplosionSounds;
		int												m_CurrLevel = 1;
		int												m_LevelCoins = 0;
		int												m_LevelScore = 0;
		int												m_TotalScore = 0;

		// ---- player ----
		std::unique_ptr<Player>	m_Player;
		bool 					m_LetsRespawn = false;
		b2Vec2					m_PlayerSpawnPos;
		sf::Clock				m_WaitTilRespawnClock;

		bool 					m_Jump = false;
		sf::Clock				m_JumpCoolDownClock;
		int						m_CanJump = 0; // how many bodies touch the ball at a time: able to jump if > 0
		
		b2Body 					*m_TouchRedBox = nullptr;
		b2Body 					*m_TouchGreenBox = nullptr;
		b2Body 					*m_TouchYellowBox = nullptr;


		// ---- window things ----
		sf::RenderWindow	m_Window;
		sf::Vector2u		m_WindowSize;
		sf::View			m_View;

		// ---- fade effect level transition ----
		std::unique_ptr<Fade> m_Fade;
		
		// ---- Hud -----
		std::unique_ptr<Hud> m_Hud;

		// ---- particles ----
		std::vector<std::shared_ptr<BoxParticles>> m_BoxParticles;

		// ---- box color to be placed in editor ----
		int						m_BoxColorIndex = 0;
		std::vector<sf::Color>	m_BoxColors = {sf::Color::White, 
											  sf::Color::Green,
											  sf::Color::Red,
											  sf::Color::Cyan,
											  sf::Color::Yellow,
											  sf::Color(25, 25, 25)}; // <-- dark gray
		scrollWheelInput		m_ScrollWheelInput = None;

		// ---- rotate the world ----
		bool m_RotateRight = false;
		bool m_RotateLeft = false;
		float m_RotVel = 0; // rotation speed where x: left y: right
		
		// ---- move level editor view ----
		bool m_MoveViewLeft = false;
		bool m_MoveViewRight = false;
		bool m_MoveViewUp = false;
		bool m_MoveViewDown = false;
};
