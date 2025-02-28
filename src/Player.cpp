#include "../includes/Player.hpp"
#include "../includes/Game.hpp"
#include "../includes/constants.h"

Player::Player(Game *game) : m_Game(game) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    // default start pos if there is no spawn point
    bodyDef.position = b2Vec2((m_Game->getWindowSize().x / 2) / Constants::SCALE, (m_Game->getWindowSize().y / 2) / Constants::SCALE);
    bodyDef.allowSleep = false;
    b2Body* body = m_Game->getWorld().CreateBody(&bodyDef);

    b2CircleShape circleShape;
    circleShape.m_p.Set(0, 0); 
    circleShape.m_radius = (Constants::CIRCLE_RADIUS - 1) / Constants::SCALE;
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circleShape;
    fixtureDef.density = 1;
    fixtureDef.friction = 0.7;
    fixtureDef.restitution = 0.42; // bounciness
    body->CreateFixture(&fixtureDef);

	int* identifier;
	identifier = new int(Constants::PLAYER);
	body->SetUserData(identifier);

    m_Shape.setRadius(Constants::CIRCLE_RADIUS);
    m_Shape.setFillColor(sf::Color::Cyan);
    m_Shape.setOrigin({Constants::CIRCLE_RADIUS, Constants::CIRCLE_RADIUS});

	m_Body = body;
}

Player::~Player() {
    delete static_cast<int *>(m_Body->GetUserData());
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(m_Shape, states);
}

void Player::update() {
    updateTransform();
}

void Player::updateTransform() {
    const float alpha = m_Game->getLerpAlpha();
    const b2Vec2 interpolatedPos = (1.0f - alpha) * m_LerpData._prevPos + alpha * m_Body->GetPosition();
    const float interpolatedAngle = (1.0f - alpha) * m_LerpData._prevAngle + alpha * m_Body->GetAngle();

    m_Shape.setPosition(Constants::SCALE * interpolatedPos.x, Constants::SCALE * interpolatedPos.y);
    m_Shape.setRotation(interpolatedAngle * Constants::RAD_TO_DEG);
}

// ---- setters ----
void Player::setInterpolationData(const b2Vec2 &prevPos, float prevAngle) {
    m_LerpData._prevPos = prevPos;
    m_LerpData._prevAngle = prevAngle;
}

// ---- getters ----
b2Body*	Player::getBody() const {
	return m_Body;
}

const sf::CircleShape &Player::getShape() const {
	return m_Shape;
}
