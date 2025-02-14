#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <box2d/box2d.h>
#include <iostream>
#include <memory>
#include <random>
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
		Game();
		~Game();

		void run();
		void createBox(const sf::Vector2i &mousePos, const sf::Color &color);
		void removeBox(const sf::Vector2i &mousePos);

		// ---- getters ----
		b2World 							&getWorld();
		sf::Vector2u 						&getWindowSize();
		std::vector<std::shared_ptr<Box>>	&getBoxes();
		sf::View							&getView();
		int									getLevelCoins() const;

		// ---- listeners ----
		void BeginContact(b2Contact* contact) override; // override the base class functions
		void EndContact(b2Contact* contact) override;	// override the base class functions

	private:
		void updateEvents();
		void updatePlay();
		void updateEditor();
		void draw_particles();
		void draw_boxes();

		// ---- editor ----
		void draw_grid();
		void draw_box_at_cursor(const sf::Vector2i &mousePos);
		
		// ---- helper/other ----
		b2Vec2 createForce(float force) const;
		void playSounds(const std::vector<std::shared_ptr<sf::SoundBuffer>> &soundBuffers,
							std::deque<sf::Sound> &sounds, float volume);

		// ---- enums ----
		enum gameMode {
			Play,
			Editor
		};

		enum scrollWheelInput {
			ScrollUp,
			ScrollDown,
			None
		};

		gameMode			_mode = Play;
		b2World				_world;

		sf::Time			_deltaTime;

		std::vector<std::shared_ptr<Box>> 				_boxes;
		std::unique_ptr<BoxMap>							_boxMap;
		std::vector<std::shared_ptr<sf::SoundBuffer>>	_coinSoundBuffers;
		std::deque<sf::Sound>							_coinSounds;
		std::vector<std::shared_ptr<sf::SoundBuffer>>	_coinExplosionSoundBuffers;
		std::deque<sf::Sound>							_coinExplosionSounds;
		int												_currLevel = 1;
		int												_levelCoins = 0;
		int												_levelScore = 0;
		int												_totalScore = 0;

		// ---- player ----
		std::unique_ptr<Player>	_player;
		bool 					_letsRespawn = false;
		b2Vec2					_playerSpawnPos;
		sf::Clock				_waitTilRespawnClock;

		bool 					_jump = false;
		sf::Clock				_jumpCoolDownClock;
		int						_canJump = 0; // how many bodies touch the ball at a time: able to jump if > 0
		
		b2Body 					*_touchRedBox = nullptr;
		b2Body 					*_touchGreenBox = nullptr;
		b2Body 					*_touchYellowoBox = nullptr;


		// ---- window things ----
		sf::RenderWindow	_window;
		sf::Vector2u		_windowSize;
		sf::View			_view;

		// ---- fade effect level transition ----
		std::unique_ptr<Fade> _fade;
		
		// ---- Hud -----
		std::unique_ptr<Hud> _hud;

		// ---- particles ----
		std::vector<std::shared_ptr<BoxParticles>> _boxParticles;

		// ---- box color to be placed in editor ----
		int						_boxColorIndex = 0;
		std::vector<sf::Color>	_boxColors = {sf::Color::White, 
											  sf::Color::Green,
											  sf::Color::Red,
											  sf::Color::Cyan,
											  sf::Color::Yellow,
											  sf::Color(25, 25, 25)}; // <-- dark gray
		scrollWheelInput		_scrollWheelInput = None;

		// ---- text things ----
		sf::Font			_modeFont;
		sf::Text			_modeText;

		// ---- rotate the world ----
		bool _rotateRight = false;
		bool _rotateLeft = false;
		float _rotVel = 0; // rotation speed where x: left y: right
		
		// ---- move level editor view ----
		bool _moveViewLeft = false;
		bool _moveViewRight = false;
		bool _moveViewUp = false;
		bool _moveViewDown = false;
};
