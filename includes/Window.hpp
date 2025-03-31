#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>

class Game;

class Window {
	public:
		Window(Game* game);
		~Window();

		void pollEvents();
		bool isOpen() const;
		void processViewMovement(float deltaTime);
		bool isKeyPressed(sf::Keyboard::Key key) const;

		void				setChangeMode(bool value);
		void				setViewOnPlayer(bool value);

		sf::RenderWindow&	getWindow();
		sf::Vector2u&		getWindowSize();
		sf::View&			getView();
		bool				getChangeMode() const;
		bool				getViewOnPlayer() const;
	
	private:
		void 				handleEditorModeKeys(sf::Keyboard::Key key);
	private:
		Game*				m_Game;
		sf::RenderWindow	m_Window;
		sf::Vector2u		m_WindowSize;
		sf::View			m_View;
		
		std::unordered_map<sf::Keyboard::Key, bool> m_KeyStates;
		bool m_ChangeMode = false;
		bool m_ViewOnPlayer = true;

		float m_RotVel = 0; // rotation speed where x: left y: right
};
