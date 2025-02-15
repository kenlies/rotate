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

	// make spawn boxes, yellow and gray boxes not collide
	if (color == sf::Color::Cyan || color == sf::Color::Yellow || color == sf::Color(25, 25, 25)) {
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
	} else if (color == sf::Color(25, 25, 25)) {
		colorIdentifier = new int(GRAY);
		body->SetUserData(colorIdentifier);
	}

	// ---- drawing attributes ----
	_shape = std::make_unique<sf::RectangleShape>();
	// make yellow boxes half the size
	if (color == sf::Color::Yellow) {
		_shape->setOrigin(BOX_WIDTH / 4, BOX_WIDTH / 4);
		_shape->setSize({BOX_WIDTH / 2, BOX_WIDTH / 2});
	} else {
		_shape->setOrigin(BOX_WIDTH / 2, BOX_WIDTH / 2);
		_shape->setSize({BOX_WIDTH, BOX_WIDTH});
	}
	_shape->setFillColor(color);

	// ---- store them in one data structure ----
	_body = body;

	// ---- set lights ----
	_light = std::make_unique<candle::RadialLight>();
	if (color == sf::Color::Yellow) {
		_light->setRange(30);
	} else {
		_light->setRange(43);
	}
	_light->setColor(color);
}

bool Box::isInView(const sf::View &view) const {
    sf::Transform viewTransform;
    viewTransform.translate(view.getCenter());
    viewTransform.rotate(view.getRotation());
    viewTransform.scale(view.getSize().x / 2.f, view.getSize().y / 2.f); // normalize size to (-1, 1)

    // invert the transformation to go from world space to local view space
    sf::Transform inverseViewTransform = viewTransform.getInverse();

    // transform the object's position into the view's local space
    sf::Vector2f localPos = inverseViewTransform.transformPoint(_shape->getPosition());

    // check if the object is within the view's local bounds (-1 to 1 in both x and y)
    return (localPos.x >= -1.05f && localPos.x <= 1.05f && localPos.y >= -1.05f && localPos.y <= 1.05f);
}

Box::~Box() {
	delete static_cast<int *>(_body->GetUserData());
}

void Box::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (_game->getMode() == Game::Play) {
		const float alpha = _game->getLerpAlpha();
		const b2Vec2 interpolatedPos = (1.0f - alpha) * _lerpData._prevPos + alpha * _body->GetPosition();
		_shape->setPosition(SCALE * interpolatedPos.x, SCALE * interpolatedPos.y);
		// don't get rotation for yellow boxes from Box2D, since their shapes are rotated in Game.cpp where this was called
		if (_shape->getFillColor() != sf::Color::Yellow) {
			const float interpolatedAngle = (1.0f - alpha) * _lerpData._prevAngle + alpha * _body->GetAngle();
			_shape->setRotation(interpolatedAngle * 180 / b2_pi);
		}
	} else {
		_shape->setPosition(SCALE * _body->GetPosition().x, SCALE * _body->GetPosition().y);
		if (_shape->getFillColor() != sf::Color::Yellow) {
			_shape->setRotation(_body->GetAngle() * 180 / b2_pi);
		}
	}
	target.draw(*_shape, states);
}

// ---- setters ----
void Box::setInterpolationData(b2Vec2 prevPos, float prevAngle) {
	_lerpData._prevPos = prevPos;
	_lerpData._prevAngle = prevAngle;
}

// ---- getters ----
b2Body*	Box::getBody() const {
	return _body;
}

const std::unique_ptr<sf::RectangleShape> &Box::getShape() const {
	return _shape;
}

const std::unique_ptr<candle::RadialLight> &Box::getLight() const {
	return _light;
}
