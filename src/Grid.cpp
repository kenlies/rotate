#include "../includes/Grid.hpp"
#include "../includes/Game.hpp"
#include "../includes/constants.h"

Grid::Grid(Game *game) : m_Game(game), m_Grid(sf::Lines) {

}

Grid::~Grid() {

}

void Grid::update() {
    sf::Vector2f topLeft = m_Game->getWindow().getWindow().mapPixelToCoords(sf::Vector2i(0, 0));
    sf::Vector2f bottomRight = m_Game->getWindow().getWindow().mapPixelToCoords(sf::Vector2i(m_Game->getWindow().getWindowSize().x, m_Game->getWindow().getWindowSize().y));
    // store the lines to be drawed in a vertex array
    m_Grid.clear();

    // calculate the vertical lines to be drawn
    for (int x = static_cast<int>(topLeft.x / Constants::BOX_WIDTH) * Constants::BOX_WIDTH; x <= bottomRight.x; x += Constants::BOX_WIDTH) {
        m_Grid.append(sf::Vertex(sf::Vector2f(x, topLeft.y), sf::Color::Red));
        m_Grid.append(sf::Vertex(sf::Vector2f(x, bottomRight.y), sf::Color::Red));
    }

    // calculate the horizontal lines to be drawn
    for (int y = static_cast<int>(topLeft.y / Constants::BOX_WIDTH) * Constants::BOX_WIDTH; y <= bottomRight.y; y += Constants::BOX_WIDTH) {
        m_Grid.append(sf::Vertex(sf::Vector2f(topLeft.x, y), sf::Color::Red));
        m_Grid.append(sf::Vertex(sf::Vector2f(bottomRight.x, y), sf::Color::Red));
    }
}

void Grid::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(m_Grid, states);
}
