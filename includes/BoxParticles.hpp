#pragma once

#include "Game.hpp"

class Game;

class BoxParticles : public sf::Drawable, public sf::Transformable {
	public:
		BoxParticles(Game *game, unsigned int count, sf::Vector2f position);
		~BoxParticles();

		void update(sf::Time elapsed);
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		// ---- getters ----

		const sf::Time &getCurrLife() const;
		
	private:

		struct Particle
		{
			sf::Vector2f velocity;
			sf::Time lifetime;
		};

		Game *_game;

		std::vector<Particle> m_particles;
		sf::VertexArray m_vertices;
		sf::Time m_lifetime;
		sf::Time m_currlife;
		sf::Vector2f m_emitter;
};
