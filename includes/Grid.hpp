#pragma once

#include <SFML/Graphics.hpp>

class Game;

class Grid : public sf::Drawable {
	public:
		Grid(Game *game);
		~Grid();

		void 			update();
		virtual void	draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	private:
		Game			*m_Game;
		sf::VertexArray m_Grid;
};
