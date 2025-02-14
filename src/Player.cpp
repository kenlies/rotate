#include "../includes/Player.hpp"

Player::Player(Game *game) : _game(game) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    // default start pos if there is no spawn point
    bodyDef.position = b2Vec2((_game->getWindowSize().x / 2) / SCALE, (_game->getWindowSize().y / 2) / SCALE);
    bodyDef.allowSleep = false;
    b2Body* body = _game->getWorld().CreateBody(&bodyDef);

    b2CircleShape circleShape;
    circleShape.m_p.Set(0, 0); 
    circleShape.m_radius = (CIRCLE_RADIUS - 1) / SCALE;
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circleShape;
    fixtureDef.density = 1;
    fixtureDef.friction = 0.7;
    fixtureDef.restitution = 0.42; // bounciness
    body->CreateFixture(&fixtureDef);

	int* identifier;
	identifier = new int(PLAYER);
	body->SetUserData(identifier);

    _shape = std::make_unique<sf::CircleShape>();
    _shape->setRadius(CIRCLE_RADIUS);
    _shape->setFillColor(sf::Color::Cyan);
    _shape->setOrigin({CIRCLE_RADIUS, CIRCLE_RADIUS});

	_body = body;
}

Player::~Player() {
    delete static_cast<int *>(_body->GetUserData());
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    _shape->setPosition(SCALE * _body->GetPosition().x, SCALE * _body->GetPosition().y);
    _shape->setRotation(_body->GetAngle() * 180 / b2_pi);
    target.draw(*_shape, states);
}

// ---- getters ----
b2Body*	Player::getBody() const {
	return _body;
}

const std::unique_ptr<sf::CircleShape> &Player::getShape() const {
	return _shape;
}