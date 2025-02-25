#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class Renderer {
	public:
		Renderer(sf::RenderWindow& window);
		~Renderer();

		void addToRenderQueue(const sf::Drawable &drawable);
		void renderFrame();

	private:
		std::vector<const sf::Drawable*>	m_Drawables;
		sf::RenderWindow&					m_Window;
};
