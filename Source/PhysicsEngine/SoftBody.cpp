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
	std::vector<std::vector<RigidBodyPtr>> particles;
	for (size_t x = 0; x < cols; ++x) {
		for (size_t y = 0; y < rows; ++y) {
			particles[x][y] = RigidBodyPtr((RigidBody*)particle->clone());
			particles[x][y]->setPosition(position + glm::vec2((float)x * distance, (float)y * distance));
			m_objects.push_back(particles[x][y]);
			// TODO when y >= 1, attach structure spring to last row
			// when y >= 2, attach bend spring two rows back
			// when x >= 1, attach structure spring to last column
			// when x >=2, attach bend spring two columns back
			// when x>=1 and y >= 1, attach shear spring to last column and row
			// when x>=1 amd y < rows - 1, attach shear spring to last column and next row
			// any springs are added to object list
		}
	}
}

void physics::SoftBody::addToScene(physics::PhysicsScene * scene)
{
	for (auto object : m_objects) {
		scene->addActor(object);
	}
}

void physics::SoftBody::kill()
{
	for (auto object : m_objects) {
		object->kill();
	}
}
