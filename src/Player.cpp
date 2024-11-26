#include "../includes/Player.hpp"

Player::Player(Game *game) : _game(game) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    // default start pos if there is no spawn point
    bodyDef.position = b2Vec2((_game->getWindowSize().x / 2) / SCALE, (_game->getWindowSize().y / 2) / SCALE);
    //myBodyDef.bullet = true;
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

    sf::CircleShape *circle = new sf::CircleShape();
    circle->setRadius(CIRCLE_RADIUS);
    circle->setFillColor(sf::Color::Cyan);
    circle->setOrigin({CIRCLE_RADIUS, CIRCLE_RADIUS});

	_body = body;
    _shape = circle;
}

Player::~Player() {
	delete _shape;
    delete static_cast<int *>(_body->GetUserData());
}

// ---- getters ----

b2Body*	Player::getBody() const {
	return _body;
}

sf::CircleShape* Player::getShape() const {
	return _shape;
}