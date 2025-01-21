#pragma once

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <iostream>
#include <memory>
#include "ResourceManager.h"
#include "Player.hpp"
#include "Box.hpp"
#include "Fade.hpp"
#include "BoxMap.hpp"
#include "Candle/RadialLight.hpp"

#define GRAVITY_MAGNITUDE 18.f
#define JUMP_FORCE 20.f
#define SCALE 30.f
#define BOX_WIDTH 32.f
#define CIRCLE_RADIUS 16.f
#define ROTATE_SPEED 2.f
#define EXPLOSION_FORCE 15.f
#define WIN_FORCE 10.f
#define PICKUP_FORCE 1.f

// identifiers
#define RED 1
#define WHITE 2
#define CYAN 3
#define GREEN 4
#define YELLOW 5
#define PLAYER 6

class Player;
class Box;
class Fade;
class BoxMap;

class Game : public b2ContactListener {
	public:
		Game();
		~Game();

		void run();
		void createBox(const sf::Vector2i &mousePos, const sf::Color &color);
		void removeBox(const sf::Vector2i &mousePos);

		// ---- getters ----
		b2World 				&getWorld();
		sf::Vector2u 			&getWindowSize();
		std::vector<Box*> 		&getBoxes();
		sf::View				&getView();

		// ---- listeners ----
		void BeginContact(b2Contact* contact) override; // override the base class functions
		void EndContact(b2Contact* contact) override;	// override the base class functions

	private:
		void updateEvents();
		void updatePlay();
		void updateEditor();
		void draw_boxes();
		void draw_player();

		// ---- editor ----
		void draw_grid();
		void draw_box_at_cursor(const sf::Vector2i &mousePos);

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

		std::vector<Box*> 	_boxes;
		std::unique_ptr<BoxMap> _boxMap;
		int					_currLevel = 1;
		Player*				_player;
		b2Vec2				_playerSpawnPos;
		sf::Clock			_waitTilRespawnClock;
		
		b2Body *_touchRedBox = nullptr;
		b2Body *_touchGreenBox = nullptr;
		b2Body *_touchYellowoBox = nullptr;
	
		bool _letsRespawn = false;
		sf::Clock _jumpCoolDownClock;
		int _canJump = 0; // how many bodies touch the ball at a time: able to jump if > 0

		// ---- window things ----
		sf::RenderWindow	_window;
		sf::Vector2u		_windowSize;
		sf::View			_view;

		// ---- fade effect level transition ----
		Fade*	_fade = nullptr;

		// ---- box color to be placed in editor ----
		int						_boxColorIndex = 0;
		std::vector<sf::Color>	_boxColors = {sf::Color::White, 
											  sf::Color::Green,
											  sf::Color::Red,
											  sf::Color::Cyan,
											  sf::Color::Yellow};
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

		bool _jump = false;
};