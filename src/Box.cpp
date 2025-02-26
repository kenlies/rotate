#include "../includes/Box.hpp"
#include "../includes/Game.hpp"
#include "../includes/constants.h"

Box::Box(Game *game, b2Vec2 &checkPos, const sf::Color &color) : m_Game(game) {

    // ---- physics attributes ----
    b2BodyDef bodyDef;
    bodyDef.position = checkPos;
	color == sf::Color::Red ? bodyDef.type = b2_dynamicBody : bodyDef.type = b2_staticBody; // red boxes are not static
	if (color == sf::Color::Red) {
		bodyDef.allowSleep = false;
	}
	
	b2Body* body = m_Game->getWorld().CreateBody(&bodyDef);
	
	b2PolygonShape Shape;
	if (color == sf::Color::Yellow ) {
		Shape.SetAsBox((Constants::BOX_WIDTH / 4) / Constants::SCALE, (Constants::BOX_WIDTH / 4) / Constants::SCALE);
	} else {
		Shape.SetAsBox((Constants::BOX_WIDTH / 2) / Constants::SCALE, (Constants::BOX_WIDTH / 2) / Constants::SCALE);
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
		colorIdentifier = new int(Constants::RED);
		body->SetUserData(colorIdentifier);
	} else if (color == sf::Color::White) {
		colorIdentifier = new int(Constants::WHITE);
		body->SetUserData(colorIdentifier);
	} else if (color == sf::Color::Cyan) {
		colorIdentifier = new int(Constants::CYAN);
		body->SetUserData(colorIdentifier);
	} else if (color == sf::Color::Green) {
		colorIdentifier = new int(Constants::GREEN);
		body->SetUserData(colorIdentifier);
	} else if (color == sf::Color::Yellow) {
		colorIdentifier = new int(Constants::YELLOW);
		body->SetUserData(colorIdentifier);
	} else if (color == sf::Color(25, 25, 25)) {
		colorIdentifier = new int(Constants::GRAY);
		body->SetUserData(colorIdentifier);
	}

	// ---- drawing attributes ----
	// make yellow boxes half the size
	if (color == sf::Color::Yellow) {
		m_Shape.setOrigin(Constants::BOX_WIDTH / 4, Constants::BOX_WIDTH / 4);
		m_Shape.setSize({Constants::BOX_WIDTH / 2, Constants::BOX_WIDTH / 2});
	} else {
		m_Shape.setOrigin(Constants::BOX_WIDTH / 2, Constants::BOX_WIDTH / 2);
		m_Shape.setSize({Constants::BOX_WIDTH, Constants::BOX_WIDTH});
	}
	m_Shape.setFillColor(color);

	// ---- store them in one data structure ----
	m_Body = body;

	// ---- set lights ----
	if (color == sf::Color::Yellow) {
		m_Light.setRange(30);
	} else {
		m_Light.setRange(43);
	}
	m_Light.setColor(color);
}

bool Box::isInView(const sf::View &view) const {
    sf::Transform viewTransform;
    viewTransform.translate(view.getCenter());
    viewTransform.rotate(view.getRotation());
    viewTransform.scale(view.getSize().x / 2.f, view.getSize().y / 2.f); // normalize size to (-1, 1)

    // invert the transformation to go from world space to local view space
    sf::Transform inverseViewTransform = viewTransform.getInverse();

    // transform the object's position into the view's local space
    sf::Vector2f localPos = inverseViewTransform.transformPoint(m_Shape.getPosition());

    // check if the object is within the view's local bounds (-1 to 1 in both x and y)
    return (localPos.x >= -1.05f && localPos.x <= 1.05f && localPos.y >= -1.05f && localPos.y <= 1.05f);
}

Box::~Box() {
	delete static_cast<int *>(m_Body->GetUserData());
}

void Box::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (m_Shape.getFillColor() == sf::Color::Cyan && m_Game->getMode() == Game::Play) {
		return ;
	}
	// ---- draw lighting ----
	// don't draw lighting on dark gray and cyan boxes
	if (m_Game->getMode() == Game::Play && m_Shape.getFillColor() != sf::Color(25, 25, 25)) {
		target.draw(m_Light, states);
	}
	target.draw(m_Shape, states);
}

void Box::update(float deltaTime) {
	if (m_Shape.getFillColor() == sf::Color::Cyan && m_Game->getMode() == Game::Play) {
		return ;
	}
	if (m_Game->getMode() == Game::Play) {
		const float alpha = m_Game->getLerpAlpha();
		const b2Vec2 interpolatedPos = (1.0f - alpha) * m_LerpData._prevPos + alpha * m_Body->GetPosition();
		m_Shape.setPosition(Constants::SCALE * interpolatedPos.x, Constants::SCALE * interpolatedPos.y);
		// don't get rotation for yellow boxes from Box2D, since their shapes are rotated in Game.cpp where this was called
		if (m_Shape.getFillColor() != sf::Color::Yellow) {
			const float interpolatedAngle = (1.0f - alpha) * m_LerpData._prevAngle + alpha * m_Body->GetAngle();
			m_Shape.setRotation(interpolatedAngle * Constants::RAD_TO_DEG);
		} else {
			m_Shape.rotate(50.f * deltaTime);
		}
	} else {
		m_Shape.setPosition(Constants::SCALE * m_Body->GetPosition().x, Constants::SCALE * m_Body->GetPosition().y);
		if (m_Shape.getFillColor() != sf::Color::Yellow) {
			m_Shape.setRotation(m_Body->GetAngle() * Constants::RAD_TO_DEG);
		}
	}
	// ---- update light position ----
	if (m_Game->getMode() == Game::Play && m_Shape.getFillColor() != sf::Color(25, 25, 25) &&
			m_Shape.getFillColor() != sf::Color::Cyan) {
		m_Light.setPosition(Constants::SCALE * m_Body->GetPosition().x, Constants::SCALE * m_Body->GetPosition().y);
	}
}

// ---- setters ----
void Box::setInterpolationData(b2Vec2 prevPos, float prevAngle) {
	m_LerpData._prevPos = prevPos;
	m_LerpData._prevAngle = prevAngle;
}

// ---- getters ----
b2Body*	Box::getBody() const {
	return m_Body;
}

const sf::RectangleShape &Box::getShape() const {
	return m_Shape;
}

const candle::RadialLight &Box::getLight() const {
	return m_Light;
}
