#include "../includes/Player.hpp"
#include "../includes/Game.hpp"
#include "../includes/constants.h"
#include "../includes/utils.h"
#include <iostream>

Player::Player(Game* game) : m_Game(game) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    // default start pos if there is no spawn point
    bodyDef.position = b2Vec2((m_Game->getWindow().getWindowSize().x / 2) / Constants::SCALE,
                                (m_Game->getWindow().getWindowSize().y / 2) / Constants::SCALE);
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

    // ---- start jumper cooldown ----
    m_JumpCoolDownClock.restart();
}

Player::~Player() {
    delete static_cast<int*>(m_Body->GetUserData());
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(m_Shape, states);
}

void Player::update(float radians) {
    // ---- spawn player at spawn position ----
    if (m_LetsRespawn) {
        if (m_WaitTilRespawnClock.getElapsedTime().asSeconds() > 2) {
            m_Body->SetTransform(m_SpawnPos, 0);
            m_Body->SetLinearVelocity(b2Vec2(0, 0));
            m_Body->SetAngularVelocity(0);
            m_LetsRespawn = false;
            m_Game->getWindow().setViewOnPlayer(true);
            m_Game->getFade().setActive();
            m_Game->setLevelCoins(0);
            m_Game->setLevelScore(0);
            m_Game->getBoxMap().loadMap(ResourceManager::getLevelFilePath("level") + std::to_string(m_Game->getCurrLevel()));
            m_Game->getHud().refreshScore(m_Game->getLevelScore());
        }
    }

    // ---- collect yellow box ----
    if (m_TouchYellowBox) {
        if (m_TouchYellowBox->GetType() != b2_dynamicBody) {
            m_Game->getSoundManager().playSound("pickup1", "pickup2", "pickup3", 80.f);
        }
        m_TouchYellowBox->SetType(b2_dynamicBody);
        m_TouchYellowBox->ApplyLinearImpulseToCenter(Utils::createForce(-Constants::PICKUP_FORCE, radians), true);
        m_TouchYellowBox = nullptr;
    }

    // ---- level clear ----
    if (m_TouchGreenBox) {
        m_Game->getWindow().setViewOnPlayer(false);
        m_LetsRespawn = true;
        m_Game->setCurrLevel(m_Game->getCurrLevel() + 1);
        m_Game->setTotalScore(m_Game->getTotalScore() + m_Game->getLevelScore());
        if (m_Game->getCurrLevel() >= 4) {
            m_Game->setCurrLevel(1);
            m_Game->setTotalScore(0);
        }
        m_TouchGreenBox->SetType(b2_dynamicBody);
        m_TouchGreenBox->ApplyLinearImpulseToCenter(Utils::createForce(-Constants::WIN_FORCE, radians), true);
        m_TouchGreenBox->ApplyTorque(m_Body->GetAngle(), true);
        m_TouchGreenBox = nullptr;
        std::cout << "LEVEL CLEAR\n";
    }

    // ---- death by red box ----
    if (m_TouchRedBox) {
        m_Game->getWindow().setViewOnPlayer(false);
        m_LetsRespawn = true;
        b2Vec2 explosionForce = Utils::createForce(-Constants::EXPLOSION_FORCE, radians);
        b2ContactEdge* contacts = m_TouchRedBox->GetContactList();

        // apply the world breaking effect: break things!!!
        while (contacts) {
            b2Contact* contact = contacts->contact;

            if (contact->IsTouching()) {
                b2Fixture* fixtureA = contact->GetFixtureA();
                b2Fixture* fixtureB = contact->GetFixtureB();

                int* fixtureA_Color = static_cast<int*>(fixtureA->GetBody()->GetUserData());
                int* fixtureB_Color = static_cast<int*>(fixtureB->GetBody()->GetUserData());

                if ((fixtureA_Color && *fixtureA_Color == Constants::RED && fixtureB_Color && *fixtureB_Color == Constants::WHITE) ||
                    fixtureA_Color && *fixtureA_Color == Constants::WHITE && fixtureB_Color && *fixtureB_Color == Constants::RED) {
                        
                    b2Body* whiteBox = (*fixtureA_Color == Constants::WHITE) ? fixtureA->GetBody() : fixtureB->GetBody();
                    whiteBox->SetType(b2_dynamicBody);
                    whiteBox->ApplyLinearImpulseToCenter(explosionForce, true);
                    whiteBox->ApplyTorque(m_Body->GetAngle(), true);
                    whiteBox->SetSleepingAllowed(false);
                }
            }
            contacts = contacts->next;
        }
        m_TouchRedBox->ApplyLinearImpulseToCenter(explosionForce, true);
        m_TouchRedBox->ApplyTorque(m_Body->GetAngle(), true);
        m_TouchRedBox = nullptr;
        std::cout << "DEATH BY RED BOX\n";
    }

    // ---- death by void ----
    if ((std::abs(m_Body->GetLinearVelocity().x) > 40.f
        || std::abs(m_Body->GetLinearVelocity().y) > 40.f)
        && !m_LetsRespawn) {
        m_Game->getWindow().setViewOnPlayer(false);
        m_LetsRespawn = true;
        m_WaitTilRespawnClock.restart();
        std::cout << "DEATH BY VOID\n"; 
    }

    // ---- jump ----
    if (m_Jump && m_CanJump > 0 && !m_LetsRespawn && m_JumpCoolDownClock.getElapsedTime().asSeconds() > 1.5f) {
            m_Body->ApplyLinearImpulseToCenter(Utils::createForce(Constants::JUMP_FORCE, radians), false);
        m_JumpCoolDownClock.restart();
    }

    updateTransform();
}

void Player::updateTransform() {
    const float alpha = m_Game->getLerpAlpha();
    const b2Vec2 interpolatedPos = (1.0f - alpha) * m_LerpData._prevPos + alpha * m_Body->GetPosition();
    const float interpolatedAngle = (1.0f - alpha) * m_LerpData._prevAngle + alpha * m_Body->GetAngle();

    m_Shape.setPosition(Constants::SCALE * interpolatedPos.x, Constants::SCALE * interpolatedPos.y);
    m_Shape.setRotation(interpolatedAngle * Constants::RAD_TO_DEG);
}

void Player::processInput(float deltaTime) {
    if (m_Game->getMode() == Game::Play) {
        m_Game->getWindow().isKeyPressed(sf::Keyboard::Space) ? m_Jump = true : m_Jump = false;
    }
}

void Player::BeginContact(b2Contact* contact) {

    // don't do anything if the contact doesn't involve player or if the player is respawning
    if ((contact->GetFixtureB()->GetBody() != m_Body 
        && contact->GetFixtureA()->GetBody() != m_Body)) {
        return ;
    }

    int* fixtureA_UserData = static_cast<int*>(contact->GetFixtureA()->GetBody()->GetUserData());
    int* fixtureB_UserData = static_cast<int*>(contact->GetFixtureB()->GetBody()->GetUserData());
    int* color;

    b2Body* box = nullptr;
    if (fixtureA_UserData && *fixtureA_UserData == Constants::PLAYER) {
        box = contact->GetFixtureB()->GetBody();
        color = fixtureB_UserData;
    } else if (fixtureB_UserData && *fixtureB_UserData == Constants::PLAYER) {
        box = contact->GetFixtureA()->GetBody();
        color = fixtureA_UserData;
    }

    // don't don anything if in contact with spawn point
    if (color && *color == Constants::CYAN) {
        return;
    }
    else if (color && *color == Constants::YELLOW) {
        m_TouchYellowBox = box;
        return;
    } else if (color && *color == Constants::RED && !m_LetsRespawn) {
        m_WaitTilRespawnClock.restart();
        m_TouchRedBox = box;
    } else if (color && *color == Constants::GREEN && !m_LetsRespawn) {
        m_WaitTilRespawnClock.restart();
        m_TouchGreenBox = box;
    }
    m_CanJump++;
}

void Player::EndContact(b2Contact* contact) {
    // don't do anything if the contact doesn't involve player or if the player is respawning
    if ((contact->GetFixtureB()->GetBody() != m_Body 
        && contact->GetFixtureA()->GetBody() != m_Body)) {
        return ;
    }

    int* fixtureA_UserData = static_cast<int*>(contact->GetFixtureA()->GetBody()->GetUserData());
    int* fixtureB_UserData = static_cast<int*>(contact->GetFixtureB()->GetBody()->GetUserData());
    int* color;

    b2Body* box = nullptr;
    if (fixtureA_UserData && *fixtureA_UserData == Constants::PLAYER) {
        box = contact->GetFixtureB()->GetBody();
        color = fixtureB_UserData;
    } else if (fixtureB_UserData && *fixtureB_UserData == Constants::PLAYER) {
        box = contact->GetFixtureB()->GetBody();
        color = fixtureA_UserData;
    }

    if (color && *color == Constants::CYAN || color && *color == Constants::YELLOW) {
        return ;
    }
    m_CanJump--;
}

void Player::setInterpolationData(const b2Vec2& prevPos, float prevAngle) {
    m_LerpData._prevPos = prevPos;
    m_LerpData._prevAngle = prevAngle;
}

void Player::setSpawnPos(const b2Vec2& spawnPos) {
    m_SpawnPos = spawnPos;
}

b2Body*	Player::getBody() const {
	return m_Body;
}

const sf::CircleShape& Player::getShape() const {
	return m_Shape;
}

bool Player::getLetsRespawn() const {
    return m_LetsRespawn;
}

const b2Vec2& Player::getSpawnPos() const {
    return m_SpawnPos;
}
