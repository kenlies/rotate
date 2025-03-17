#include "../includes/Window.hpp"
#include "../includes/Game.hpp"
#include "../includes/constants.h"
#include <iostream>

Window::Window(Game *game) :
	m_Game(game),
    m_Window({800, 600}, "rotate", sf::Style::Close, sf::ContextSettings(0, 0, 8)),
    m_WindowSize(m_Window.getSize()), 
	m_View(sf::Vector2f(m_WindowSize.x / 2, m_WindowSize.y / 2), sf::Vector2f())
{
    m_Window.setMouseCursorVisible(false);
    m_View.setSize(sf::Vector2f(m_WindowSize.x, m_WindowSize.y));
}

Window::~Window() {

}

void Window::processViewMovement(float deltaTime) {
    // ---- rotate the view ----
	if (m_Game->getMode() == Game::Play) {
		if (isKeyPressed(sf::Keyboard::A) && !m_Game->getPlayer().getLetsRespawn()) {
			m_RotVel += Constants::ROTATE_SPEED;
		}
		if (isKeyPressed(sf::Keyboard::D) && !m_Game->getPlayer().getLetsRespawn()) {
			m_RotVel -= Constants::ROTATE_SPEED;
		}
		m_View.rotate(m_RotVel * deltaTime); // frame rate independent
		m_RotVel *= 0.92; // smoothing rotation

        // make camera follow player with lerp
        if (m_ViewOnPlayer) {
            sf::Vector2f playerPos = m_Game->getPlayer().getShape().getPosition();
            sf::Vector2f currentCenter = m_View.getCenter();
            float k = 4.0f;
            float lerpFactor = 1 - std::exp(-k * deltaTime);

            sf::Vector2f interpolatedPos = currentCenter + lerpFactor * (playerPos - currentCenter);
            m_View.setCenter(interpolatedPos);
        }

	// ---- move editor view ----
	} else {
		if (isKeyPressed(sf::Keyboard::A)) {
			m_View.move({-Constants::EDITOR_MOVE_SPEED * deltaTime, 0});
		}
		if (isKeyPressed(sf::Keyboard::D)) {
			m_View.move({Constants::EDITOR_MOVE_SPEED * deltaTime, 0});
		}
		if (isKeyPressed(sf::Keyboard::W)) {
			m_View.move({0, -Constants::EDITOR_MOVE_SPEED * deltaTime});
		}
		if (isKeyPressed(sf::Keyboard::S)) {
			m_View.move({0, Constants::EDITOR_MOVE_SPEED * deltaTime});
		}
		m_Window.setView(m_View); // not needed bcs we set view in hud update but better to do it here too....
	}
}

void Window::pollEvents() {
	for (auto event = sf::Event(); m_Window.pollEvent(event);) {
		switch (event.type) {
            case sf::Event::Closed:
                m_Window.close();
                break;
            case sf::Event::MouseWheelScrolled:
                if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                    if (event.mouseWheelScroll.delta > 0) {
                        m_Game->setSrollWheelInput(Game::ScrollUp);
                    } else {
                        m_Game->setSrollWheelInput(Game::ScrollDown);
                    }
                }
                break;
			case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::M) {
                    m_ChangeMode = true;
                    break;
                } else if (m_Game->getMode() == Game::Editor) {
                    handleEditorModeKeys(event.key.code);
                    break;
                }
				m_KeyStates[event.key.code] = true;
				break;
			case sf::Event::KeyReleased:
				m_KeyStates[event.key.code] = false;
				break;
            default:
                break;
		}
	}
}

void Window::handleEditorModeKeys(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::Up:
            m_Game->setBoxColorIndex(m_Game->getBoxColorIndex() + 1);
            break;
        case sf::Keyboard::Down:
            m_Game->setBoxColorIndex(m_Game->getBoxColorIndex() - 1);
            break;
        case sf::Keyboard::Comma:
            std::cout << "Saving map\n";
            m_Game->getBoxMap().saveMap(ResourceManager::getLevelFilePath("tmp"));
            break;
        case sf::Keyboard::Period:
            std::cout << "Loading map\n";
            m_Game->getBoxMap().loadMap(ResourceManager::getLevelFilePath("tmp"));
            m_Game->getPlayer().getBody()->SetTransform(m_Game->getPlayer().getSpawnPos(), 0);
            break;
        default:
            break;
    }
}

bool Window::isKeyPressed(sf::Keyboard::Key key) const {
    auto it = m_KeyStates.find(key);
    return it != m_KeyStates.end() && it->second;
}

bool Window::isOpen() const {
	return m_Window.isOpen();
}

void Window::setChangeMode(bool value) {
    m_ChangeMode = value;
}

void Window::setViewOnPlayer(bool value) {
    m_ViewOnPlayer = value;
}

sf::RenderWindow &Window::getWindow() {
	return m_Window;
}

sf::Vector2u &Window::getWindowSize() {
	return m_WindowSize;
}

sf::View &Window::getView() {
	return m_View;
}

bool Window::getChangeMode() const {
    return m_ChangeMode;
}

bool Window::getViewOnPlayer() const {
    return m_ViewOnPlayer;
}

