#include "RigidBody.h"
#include "ExternalLibraries.h"

physics::RigidBody::RigidBody(glm::vec2 position, glm::vec2 velocity, float orientation, float mass, glm::vec4 colour)
	: PhysicsObject(colour), m_position(position),m_velocity(velocity), m_orientation(orientation), m_mass(mass), m_totalForce({0,0})
{
	if (mass < 0 || isnan(mass)) {
		throw std::invalid_argument("Mass must be positive");
	}
}

void physics::RigidBody::earlyUpdate(float timestep)
{
}

void physics::RigidBody::fixedUpdate(glm::vec2 gravity, float timestep)
{
	applyForce(gravity * m_mass);
	// Update previous position
	m_pastPosition = m_position; 
	// Move before and after changing velocity to avoid integration error
	// This treats acceleration as constant over timestep
	m_position += 0.5f * m_velocity * timestep;
	applyImpulse(m_totalForce * timestep);
	m_position += 0.5f * m_velocity * timestep;
	m_totalForce = { 0,0 };
}

void physics::RigidBody::applyImpulse(glm::vec2 force)
{
	// TODO decide how to deal with infinite/zero masses
	m_velocity += force / m_mass;
}

void physics::RigidBody::applyImpulseFromOther(RigidBody * other, glm::vec2 force)
{
	applyImpulse(force);
	other->applyImpulse(-force);
}

void physics::RigidBody::applyForce(glm::vec2 force)
{
	m_totalForce += force;
}

void physics::RigidBody::applyForceFromOther(RigidBody * other, glm::vec2 force)
{
	applyForce(force);
	other->applyForce(-force);
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
	if (mass <= 0 || isnan(mass)) {
		throw std::invalid_argument("Radius must be positive");
	}
	m_mass = mass;
}

void physics::RigidBody::setOrientation(float orientation)
{
	// TODO decide if degrees should be used instead
	m_orientation = fmod(orientation, glm::pi<float>());
}

float physics::RigidBody::calculateEnergy(glm::vec2 gravity)
{
	float potential = -glm::dot(m_position, gravity) * m_mass;
	float kinetic = 0.5f * m_mass * glm::dot(m_velocity, m_velocity);
	return potential + kinetic;
}

glm::vec2 physics::RigidBody::calculateMomentum()
{
	return m_velocity * m_mass;
}

