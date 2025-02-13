#include "../includes/BoxParticles.hpp"

BoxParticles::BoxParticles(unsigned int count, sf::Vector2f position) :
    m_particles(count),
    m_vertices(sf::Points, count),
    m_lifetime(sf::seconds(0.3f)),
	m_currlife(sf::seconds(0.f))
{
	for (std::size_t i = 0; i < m_particles.size(); ++i) {
		// give a random velocity and lifetime to the particle
		float angle = (std::rand() % 360) * 3.14f / 180.f;
		float speed = (std::rand() % 600) + 10.f;
		m_particles[i].velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
		m_particles[i].lifetime = sf::milliseconds((std::rand() % 800) + 500);

		// reset the position of the corresponding vertex
		m_vertices[i].position = position;
		m_vertices[i].color = sf::Color::Yellow;
	}
}

BoxParticles::~BoxParticles() {

}

void BoxParticles::update(sf::Time elapsed) {
	m_currlife += elapsed;
	for (std::size_t i = 0; i < m_particles.size(); ++i) {
		// update the particle lifetime
		Particle& p = m_particles[i];
		p.lifetime -= elapsed;

		// update the position of the corresponding vertex
		m_vertices[i].position += p.velocity * elapsed.asSeconds();

		// update the alpha (transparency) of the particle according to its lifetime
		float ratio = p.lifetime.asSeconds() / m_lifetime.asSeconds();
		m_vertices[i].color.a = static_cast<sf::Uint8>(ratio * 255);
	}
}

void BoxParticles::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	// apply the transform
	states.transform *= getTransform();

	// our particles don't use a texture
	states.texture = NULL;

	// draw the vertex array
	target.draw(m_vertices, states);
}

const sf::Time &BoxParticles::getCurrLife() const {
	return m_currlife;
}