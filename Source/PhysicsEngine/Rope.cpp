#include "Rope.h"
#include "RigidBody.h"
#include "Spring.h"

physics::Rope::Rope()
{
}

physics::Rope::Rope(glm::vec2 position, RigidBody * particle, size_t segments,
	float distance, float strength, float damping)
	: SoftBody()
{
	m_particles = std::vector<std::vector<RigidBodyPtr>>(1, std::vector<RigidBodyPtr>(segments, RigidBodyPtr()));
	// Reserve memory for springs
	m_structureSprings.reserve(segments - 1);

	glm::vec2 localX = particle->getLocalX();
	float edgeDistance = std::max(0.f, distance - particle->getWidth());
	float xExtent = 0.5f * particle->getWidth();

	// Anchor points
	glm::vec2 right = { xExtent, 0 };
	glm::vec2 left = { -xExtent, 0 };

	for (size_t x = 0; x < segments; ++x) {
		m_particles[0][x] = RigidBodyPtr((RigidBody*)particle->clone());
		m_particles[0][x]->setPosition(position + (float)x * distance * localX);
		if (x >= 1) {
			// Attach structure spring to last particle
			m_structureSprings.push_back(std::make_shared<Spring>(strength, edgeDistance, damping, m_particles[0][x], m_particles[0][x-1],left,right));
		}
	}
}
