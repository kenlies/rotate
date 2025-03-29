#pragma once

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

class Game;

class Player : public sf::Drawable, public b2ContactListener {
	public:
		Player(Game* game);
		~Player();

		virtual void			draw(sf::RenderTarget& target, sf::RenderStates states) const override;
		void 					update(float radians);
		void					processInput(float deltaTime);

		void 					BeginContact(b2Contact* contact) override; // override the base class functions
		void 					EndContact(b2Contact* contact) override;	// override the base class functions PRIVATE??

		void					setInterpolationData(const b2Vec2& prevPos, float prevAngle);
		void					setSpawnPos(const b2Vec2& spawnPos);

		b2Body*					getBody() const;
		const sf::CircleShape&	getShape() const;
		bool					getLetsRespawn() const;
		const b2Vec2&			getSpawnPos() const;
	public:
		struct InterpolationData {
			b2Vec2 _prevPos;
			float _prevAngle;
		};
	
	private:
		void 					updateTransform();
	private:
		InterpolationData		m_LerpData;
		Game*					m_Game;
		b2Body* 				m_Body;
		sf::CircleShape			m_Shape;
		b2Vec2					m_SpawnPos;

		bool 					m_LetsRespawn = false;
		sf::Clock				m_WaitTilRespawnClock;

		bool 					m_Jump = false;
		sf::Clock				m_JumpCoolDownClock;
		int						m_CanJump = 0; // how many bodies touch the ball at a time: able to jump if > 0
		
		b2Body*					m_TouchRedBox = nullptr;
		b2Body*					m_TouchGreenBox = nullptr;
		b2Body*					m_TouchYellowBox = nullptr;
};
