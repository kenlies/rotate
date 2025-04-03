#pragma once

#include <SFML/Graphics.hpp>

class Game;

class BoxParticles : public sf::Drawable, public sf::Transformable {
	public:
		BoxParticles(unsigned int count, sf::Vector2f position);
		~BoxParticles();

		void					update(sf::Time elapsed);
		virtual void			draw(sf::RenderTarget& target, sf::RenderStates states) const;

		const sf::Time&			getCurrLife() const;
		
	private:
		struct Particle
		{
			sf::Vector2f		_velocity;
			sf::Time			_lifetime;
		};

		std::vector<Particle>	m_Particles;
		sf::VertexArray 		m_Vertices;
		sf::Time				m_Lifetime;
		sf::Time				m_Currlife;
		sf::Vector2f			m_Emitter;
};
