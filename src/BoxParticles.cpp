#include "../includes/BoxParticles.hpp"
#include "../includes/Game.hpp"

BoxParticles::BoxParticles(unsigned int count, sf::Vector2f position) :
    m_Particles(count),
    m_Vertices(sf::Points, count),
    m_Lifetime(sf::seconds(0.3f)),
	m_Currlife(sf::seconds(0.f))
{
	for (size_t i = 0; i < m_Particles.size(); ++i) {
		// give a random velocity and lifetime to the particle
		float angle = (std::rand() % 360) * 3.14f / 180.f;
		float speed = (std::rand() % 600) + 10.f;
		m_Particles[i]._velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
		m_Particles[i]._lifetime = sf::milliseconds((std::rand() % 800) + 500);

		// reset the position of the corresponding vertex
		m_Vertices[i].position = position;
		m_Vertices[i].color = sf::Color::Yellow;
	}
}

BoxParticles::~BoxParticles() {

}

void BoxParticles::update(sf::Time elapsed) {
	m_Currlife += elapsed;
	for (size_t i = 0; i < m_Particles.size(); ++i) {
		// update the particle lifetime
		Particle& p = m_Particles[i];
		p._lifetime -= elapsed;

		// update the position of the corresponding vertex
		m_Vertices[i].position += p._velocity * elapsed.asSeconds();

		// update the alpha (transparency) of the particle according to its lifetime
		float ratio = p._lifetime.asSeconds() / m_Lifetime.asSeconds();
		m_Vertices[i].color.a = static_cast<sf::Uint8>(ratio * 255);
	}
}

void BoxParticles::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	// apply the transform
	states.transform *= getTransform();

	// our particles don't use a texture
	states.texture = NULL;

	// draw the vertex array
	target.draw(m_Vertices, states);
}

const sf::Time &BoxParticles::getCurrLife() const {
	return m_Currlife;
}
