#include "../includes/Renderer.hpp"

Renderer::Renderer(sf::RenderWindow& window) : m_Window(window) {

}

Renderer::~Renderer() {

}

void Renderer::addToRenderQueue(const sf::Drawable &drawable) {
	m_Drawables.push_back(&drawable);
}

void Renderer::renderFrame() {
	m_Window.clear();

	for (const auto &d : m_Drawables) {
		m_Window.draw(*d);
	}

	m_Window.display();
	m_Drawables.clear();
}
