#include "../includes/Box.hpp"

Box::Box(Game *game, b2Vec2 &checkPos, const sf::Color &color) : _game(game) {

    // ---- physics attributes ----
    b2BodyDef bodyDef;
    bodyDef.position = checkPos;
	color == sf::Color::Red ? bodyDef.type = b2_dynamicBody : bodyDef.type = b2_staticBody; // red boxes are not static
	if (color == sf::Color::Red) {
		bodyDef.allowSleep = false;
	}
	
	b2Body* body = _game->getWorld().CreateBody(&bodyDef);
	
	b2PolygonShape Shape;
	if (color == sf::Color::Yellow ) {
		Shape.SetAsBox((BOX_WIDTH / 4) / SCALE, (BOX_WIDTH / 4) / SCALE);
	} else {
		Shape.SetAsBox((BOX_WIDTH / 2) / SCALE, (BOX_WIDTH / 2) / SCALE);
	}
	b2FixtureDef fixtureDef;

	// make spawn boxes anad yellow boxes not collide
	if (color == sf::Color::Cyan || color == sf::Color::Yellow) {
		fixtureDef.isSensor = true;
	}
	fixtureDef.shape = &Shape;
	fixtureDef.density = 1.f; // def 1
	fixtureDef.friction = 0.2f;
	fixtureDef.restitution = 0.35f; // bounciness // def 0.35

	body->CreateFixture(&fixtureDef);

	// ---- set color identifier -----
	int* colorIdentifier;
	if (color == sf::Color::Red) {
		colorIdentifier = new int(RED);
		body->SetUserData(colorIdentifier);
	} else if (color == sf::Color::White) {
		colorIdentifier = new int(WHITE);
		body->SetUserData(colorIdentifier);
	} else if (color == sf::Color::Cyan) {
		colorIdentifier = new int(CYAN);
		body->SetUserData(colorIdentifier);
	} else if (color == sf::Color::Green) {
		colorIdentifier = new int(GREEN);
		body->SetUserData(colorIdentifier);
	} else if (color == sf::Color::Yellow) {
		colorIdentifier = new int(YELLOW);
		body->SetUserData(colorIdentifier);
	}

	// ---- drawing attributes ----
	sf::RectangleShape *rectangle = new sf::RectangleShape();
	// make yellow boxes half the size
	if (color == sf::Color::Yellow) {
		rectangle->setOrigin(BOX_WIDTH / 4, BOX_WIDTH / 4);
		rectangle->setSize({BOX_WIDTH / 2, BOX_WIDTH / 2});
	} else {
		rectangle->setOrigin(BOX_WIDTH / 2, BOX_WIDTH / 2);
		rectangle->setSize({BOX_WIDTH, BOX_WIDTH});
	}
	rectangle->setFillColor(color);

	// ---- store them in one data structure ----a
	_body = body;
	_shape = rectangle;

	// ---- set lights ----
	_light = new candle::RadialLight;
	if (color == sf::Color::Yellow) {
		_light->setRange(30);
	} else {
		_light->setRange(43);
	}
	_light->setColor(color);
}

Box::~Box() {
	delete _shape;
	//delete _light;
	delete static_cast<int *>(_body->GetUserData());
}

// ---- getters ----

b2Body*	Box::getBody() const {
	return _body;
}

sf::RectangleShape* Box::getShape() const {
	return _shape;
}

candle::RadialLight* Box::getLight() const {
	return _light;
}
