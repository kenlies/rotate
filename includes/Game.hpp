#pragma once

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
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
		void updateBoxes();
		void addParticlesToRenderQueue();
		void addBoxesToRenderQueue();

		// ---- editor ----
		void addCursorBoxToRenderQueue(const sf::Vector2i &mousePos);
		
		// ---- helper/other ----
		b2Vec2 createForce(float force) const;

	private:
		enum scrollWheelInput {
			ScrollUp,
			ScrollDown,
			None
		};

		// ---- window things ----
		sf::RenderWindow	m_Window;
		sf::Vector2u		m_WindowSize;
		sf::View			m_View;

		// ---- game things ----
		gameMode			m_Mode = Play;
		b2World				m_World;
		sf::Time			m_DeltaTime;
		Fade 				m_Fade;
		HUD 				m_Hud;
		BoxMap				m_BoxMap;
		SoundManager		m_SoundManager;
		Renderer			m_Renderer;

		std::vector<std::shared_ptr<Box>>	m_Boxes;

		int					m_CurrLevel = 1;
		int					m_LevelCoins = 0;
		int					m_LevelScore = 0;
		int					m_TotalScore = 0;

		// ---- physics interpolation ----
		float				m_Accumulator = 0;
		float				m_LerpAlpha;

		// ---- player ----
		Player					m_Player;
		bool 					m_LetsRespawn = false;
		b2Vec2					m_PlayerSpawnPos;
		sf::Clock				m_WaitTilRespawnClock;

		bool 					m_Jump = false;
		sf::Clock				m_JumpCoolDownClock;
		int						m_CanJump = 0; // how many bodies touch the ball at a time: able to jump if > 0
		
		b2Body 					*m_TouchRedBox = nullptr;
		b2Body 					*m_TouchGreenBox = nullptr;
		b2Body 					*m_TouchYellowBox = nullptr;

		// ---- particles ----
		std::vector<std::shared_ptr<BoxParticles>> m_BoxParticles;

		// ---- editor ----
		Grid						m_Grid;
		sf::RectangleShape			m_CursorBox;
		int	m_BoxColorIndex = 0;
		std::array<sf::Color, 6>	m_BoxColors = {sf::Color::White, 
													sf::Color::Green,
											  		sf::Color::Red,
											  		sf::Color::Cyan,
											 		sf::Color::Yellow,
											  		sf::Color(25, 25, 25)}; // <-- dark gray
		scrollWheelInput	m_ScrollWheelInput = None;

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
