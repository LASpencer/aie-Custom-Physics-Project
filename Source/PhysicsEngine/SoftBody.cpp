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
	: m_particles(cols, std::vector<RigidBodyPtr>(rows, RigidBodyPtr())), m_structureSprings(), m_shearSprings(), m_bendSprings()
{
	// Reserve memory for new objects
	if (cols > 0 && rows > 0) {
		m_structureSprings.reserve((cols + rows - 2) * (cols + rows - 2));
	}
	else {
		m_structureSprings.reserve(std::max(cols,rows) - 1);
	}

	if (cols > 0 && rows > 0) {
		m_shearSprings.reserve(2 * (cols + rows - 2) * (cols + rows - 2));
	}

	if (cols > 0 && rows > 0) {
		m_bendSprings.reserve((cols + rows - 4) * (cols + rows - 4));
	}
	else {
		m_structureSprings.reserve(std::max(cols, rows) - 2);
	}

	//float diagonalDistance = sqrtf(2 * distance * distance);

	glm::vec2 localX = particle->getLocalX();
	glm::vec2 localY = particle->getLocalY();

	// Get distance between edges of particles
	float xEdgeDistance = std::max(0.f, distance - particle->getWidth());	// HACK assumes spring length must be positive
	float yEdgeDistance = std::max(0.f, distance - particle->getHeight());

	//float diagonalDistance = sqrtf(xEdgeDistance * xEdgeDistance + yEdgeDistance + yEdgeDistance);
	float diagonalDistance = std::max(0.f, sqrtf(2 * distance * distance) - particle->getDiagonalLength());

	float xExtent = 0.5f * particle->getWidth();
	float yExtent = 0.5f * particle->getHeight();
	float diagonal = 0.5f * particle->getDiagonalLength();

	// Anchor points for springs
	glm::vec2 top = { 0,yExtent };
	glm::vec2 bottom = { 0, -yExtent };
	glm::vec2 right = { xExtent, 0 };
	glm::vec2 left = { -xExtent, 0 };
	glm::vec2 topRight = glm::normalize(top + right) * diagonal;
	glm::vec2 bottomRight = glm::normalize(bottom + right) * diagonal;

	// TODO maybe should set springs at ends of particles to allow friction

	for (size_t x = 0; x < cols; ++x) {
		for (size_t y = 0; y < rows; ++y) {
			m_particles[x][y] = RigidBodyPtr((RigidBody*)particle->clone());
			m_particles[x][y]->setPosition(position +(float)x * distance * localX + (float)y * distance * localY);
			if (y >= 1) {
				// Attach structure spring to last row
				m_structureSprings.push_back(std::make_shared<Spring>(strength, yEdgeDistance, damping, m_particles[x][y], m_particles[x][y - 1], bottom, top));
				if (y >= 2) {
					// Attach bend spring two rows back
					m_bendSprings.push_back(std::make_shared<Spring>(bendStrength, 2* distance, 0, m_particles[x][y], m_particles[x][y-2]));	// TODO figure out if bend needs damping too
					// TODO figure out if bend springs should be at edge too
				}
			}
			if (x >= 1) {
				// Attach structure spring to last column
				m_structureSprings.push_back(std::make_shared<Spring>(strength, xEdgeDistance, damping, m_particles[x][y], m_particles[x-1][y], left, right));
				if ( x >= 2) {
					// Attach bend spring two columns back
					m_bendSprings.push_back(std::make_shared<Spring>(bendStrength, 2* distance, 0, m_particles[x][y], m_particles[x-2][y]));	// TODO figure out if bend needs damping too
				}
				if (y >= 1) {
					// Attach shear spring to last column and row
					m_shearSprings.push_back(std::make_shared<Spring>(shearStrength, diagonalDistance, damping, m_particles[x][y], m_particles[x - 1][y - 1],-topRight,topRight));	// TODO figure out if shear needs damping too
					// TODO figure out if shear springs should be at edge too
				}
				if (y + 1 < rows) {
					// Attach shear spring to last column and next row
					m_shearSprings.push_back(std::make_shared<Spring>(shearStrength, diagonalDistance, damping, m_particles[x][y], m_particles[x - 1][y + 1], -bottomRight, bottomRight));
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
