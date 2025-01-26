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

bool Box::isInView(const sf::View &view) const {

    sf::Vector2f viewCenter = view.getCenter();
    sf::Vector2f halfSize(view.getSize().x / 2.f, view.getSize().y / 2.f);

    sf::Transform viewTransform;
    viewTransform.rotate(view.getRotation(), viewCenter);
    sf::Vector2f topLeft = viewTransform.transformPoint(viewCenter.x - halfSize.x, viewCenter.y - halfSize.y);
    sf::Vector2f topRight = viewTransform.transformPoint(viewCenter.x + halfSize.x, viewCenter.y - halfSize.y);
    sf::Vector2f bottomLeft = viewTransform.transformPoint(viewCenter.x - halfSize.x, viewCenter.y + halfSize.y);
    sf::Vector2f bottomRight = viewTransform.transformPoint(viewCenter.x + halfSize.x, viewCenter.y + halfSize.y);

    sf::ConvexShape rotatedView(4);
    rotatedView.setPoint(0, topLeft);
    rotatedView.setPoint(1, topRight);
    rotatedView.setPoint(2, bottomRight);
    rotatedView.setPoint(3, bottomLeft);

    return rotatedView.getGlobalBounds().intersects(_shape->getGlobalBounds());
}

Box::~Box() {
	delete _shape;
	delete _light;
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
