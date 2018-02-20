#include "SoftBody.h"
#include "Rigidbody.h"
#include "Sphere.h"
#include "Spring.h"

physics::SoftBody::SoftBody()
{
}

physics::SoftBody::SoftBody(glm::vec2 position, RigidBody* particle, size_t cols, size_t rows,
	float distance, float strength, float shearStrength, float bendStrength,
	float damping, glm::vec4 springColour)
{
	// Reserve memory for new objects
	m_particles.reserve(cols);
	m_structureSprings.reserve((cols + rows - 2) * (cols + rows - 2));
	if (shearStrength > 0) {
		m_shearSprings.reserve(2 * (cols + rows - 2) * (cols + rows - 2));
	}
	if (bendStrength > 0) {
		m_bendSprings.reserve((cols + rows - 4) * (cols + rows - 4));
	}

	float diagonalDistance = sqrtf(2 * distance * distance);

	for (size_t x = 0; x < cols; ++x) {
		m_particles[x].reserve(rows);
		for (size_t y = 0; y < rows; ++y) {
			m_particles[x][y] = RigidBodyPtr((RigidBody*)particle->clone());
			m_particles[x][y]->setPosition(position + glm::vec2((float)x * distance, (float)y * distance));
			if (y >= 1) {
				// Attach structure spring to last row
				m_structureSprings.push_back(std::make_shared<Spring>(strength, distance, damping, m_particles[x][y], m_particles[x][y-1]));
				if (bendStrength > 0 && y >= 2) {
					// Attach bend spring two rows back
					m_bendSprings.push_back(std::make_shared<Spring>(bendStrength, 2.f * distance, 0, m_particles[x][y], m_particles[x][y-2]));	// TODO figure out if bend needs damping too
				}
			}
			if (x >= 1) {
				// Attach structure spring to last column
				m_structureSprings.push_back(std::make_shared<Spring>(strength, distance, damping, m_particles[x][y], m_particles[x-1][y]));
				if (bendStrength > 0 && x >= 2) {
					// Attach bend spring two columns back
					m_bendSprings.push_back(std::make_shared<Spring>(bendStrength, 2.f * distance, 0, m_particles[x][y], m_particles[x-2][y]));	// TODO figure out if bend needs damping too
				}
				if (shearStrength > 0) {
					if (y >= 1) {
						// Attach shear spring to last column and row
						m_shearSprings.push_back(std::make_shared<Spring>(shearStrength, diagonalDistance, 0, m_particles[x][y], m_particles[x - 1][y - 1]));	// TODO figure out if shear needs damping too
					}
					if (y + 1 < rows) {
						// Attach shear spring to last column and next row
						m_shearSprings.push_back(std::make_shared<Spring>(shearStrength, diagonalDistance, 0, m_particles[x][y], m_particles[x - 1][y + 1]));
					}
				}
			}
		}
	}
}

void physics::SoftBody::addToScene(physics::PhysicsScene * scene)
{
	for (auto column : m_particles) {
		for (auto particle : column) {
			PhysicsObjectPtr object = std::dynamic_pointer_cast<PhysicsObject, RigidBody>(particle);
			if (object) {
				scene->addActor(object);
			}
		}
	}
	for (auto spring : m_structureSprings) {
		PhysicsObjectPtr object = std::dynamic_pointer_cast<PhysicsObject, Spring>(spring);
		if (object) {
			scene->addActor(object);
		}
	}
	for (auto spring : m_shearSprings) {
		PhysicsObjectPtr object = std::dynamic_pointer_cast<PhysicsObject, Spring>(spring);
		if (object) {
			scene->addActor(object);
		}
	}
	for (auto spring : m_bendSprings) {
		PhysicsObjectPtr object = std::dynamic_pointer_cast<PhysicsObject, Spring>(spring);
		if (object) {
			scene->addActor(object);
		}
	}
}

void physics::SoftBody::kill()
{
	for (auto column : m_particles) {
		for (auto particle : column) {
			particle->kill();
		}
	}
	for (auto spring : m_structureSprings) {
		spring->kill();
	}
	for (auto spring : m_shearSprings) {
		spring->kill();
	}
	for (auto spring : m_bendSprings) {
		spring->kill();
	}
}
