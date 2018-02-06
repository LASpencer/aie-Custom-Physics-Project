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

void physics::RigidBody::setPosition(glm::vec2 position)
{
	m_position = position;
}

void physics::RigidBody::setVelocity(glm::vec2 velocity)
{
	m_velocity = velocity;
}

void physics::RigidBody::setMass(float mass)
{
	m_mass = mass;
}

void physics::RigidBody::setOrientation(float orientation)
{
	// TODO decide if degrees should be used instead
	m_orientation = fmod(orientation, glm::pi<float>());
}

