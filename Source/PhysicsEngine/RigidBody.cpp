#include "RigidBody.h"
#include "ExternalLibraries.h"

physics::RigidBody::RigidBody(glm::vec2 position, glm::vec2 velocity, float orientation, float mass, glm::vec4 colour)
	: PhysicsObject(colour), m_position(position),m_velocity(velocity), m_orientation(orientation), m_mass(mass)
{
	// TODO check arguments for validity
}

void physics::RigidBody::fixedUpdate(glm::vec2 gravity, float timestep)
{
	applyForce(gravity * m_mass * timestep);
	m_position += m_velocity * timestep;
}

void physics::RigidBody::applyForce(glm::vec2 force)
{
	// TODO decide how to deal with infinite/zero masses
	m_velocity += force / m_mass;
}

void physics::RigidBody::applyForceFromOther(RigidBody * other, glm::vec2 force)
{
}

