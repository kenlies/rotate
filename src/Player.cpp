#include "../includes/Player.hpp"

Player::Player(Game *game) : _game(game) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    // default start pos if there is no spawn point
    bodyDef.position = b2Vec2((_game->getWindowSize().x / 2) / Constants::SCALE, (_game->getWindowSize().y / 2) / Constants::SCALE);
    bodyDef.allowSleep = false;
    b2Body* body = _game->getWorld().CreateBody(&bodyDef);

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

    _shape = std::make_unique<sf::CircleShape>();
    _shape->setRadius(Constants::CIRCLE_RADIUS);
    _shape->setFillColor(sf::Color::Cyan);
    _shape->setOrigin({Constants::CIRCLE_RADIUS, Constants::CIRCLE_RADIUS});

	_body = body;
}

Player::~Player() {
    delete static_cast<int *>(_body->GetUserData());
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    const float alpha = _game->getLerpAlpha();
    const b2Vec2 interpolatedPos = (1.0f - alpha) * _lerpData._prevPos + alpha * _body->GetPosition();
    const float interpolatedAngle = (1.0f - alpha) * _lerpData._prevAngle + alpha * _body->GetAngle();

    _shape->setPosition(Constants::SCALE * interpolatedPos.x, Constants::SCALE * interpolatedPos.y);
    _shape->setRotation(interpolatedAngle * Constants::RAD_TO_DEG);

    target.draw(*_shape, states);
}

// ---- setters ----
void Player::setInterpolationData(b2Vec2 prevPos, float prevAngle) {
    _lerpData._prevPos = prevPos;
    _lerpData._prevAngle = prevAngle;
}

// ---- getters ----
b2Body*	Player::getBody() const {
	return _body;
}

const std::unique_ptr<sf::CircleShape> &Player::getShape() const {
	return _shape;
}
