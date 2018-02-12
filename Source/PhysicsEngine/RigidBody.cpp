#include "RigidBody.h"
#include "ExternalLibraries.h"

#include "Plane.h"

physics::RigidBody::RigidBody(glm::vec2 position, glm::vec2 velocity, float orientation, float mass, float elasticity, glm::vec4 colour)
	: PhysicsObject(elasticity, colour), m_position(position),m_velocity(velocity), m_orientation(orientation), m_totalForce({0,0}),
	m_static(false)
{
	setMass(mass);
}

void physics::RigidBody::earlyUpdate(float timestep)
{
}

void physics::RigidBody::fixedUpdate(glm::vec2 gravity, float timestep)
{
	// Update previous position
	m_pastPosition = m_position; 
	if (!m_static) {
		if (isKinematic()) {
			m_position += m_velocity * timestep;
		}
		else {
			applyForce(gravity * m_mass);
			applyImpulse(m_totalForce * timestep);
			m_position +=  m_velocity * timestep;
		}
	}
	m_totalForce = { 0,0 };
}

void physics::RigidBody::applyImpulse(glm::vec2 force)
{
	// TODO decide how to deal with infinite/zero masses
	m_velocity += force * m_invMass;
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
	if (!m_static) {
		m_velocity = velocity;
	}
}

float physics::RigidBody::getMass()
{
	return m_mass;
}

void physics::RigidBody::setMass(float mass)
{
	if (mass < 0 || isnan(mass)) {
		throw std::invalid_argument("Mass must be positive");
	}
	else if (!m_static) {
		 if (mass == 0 || isinf(mass)) {
			m_mass = INFINITY;
			m_invMass = 0;
		}
		else {
			m_mass = mass;
			m_invMass = 1 / mass;
		}
	}
}

float physics::RigidBody::getInvMass()
{
	return m_invMass;
}

void physics::RigidBody::setStatic(bool value)
{
	m_static = value;
	if (m_static) {
		m_mass = INFINITY;
		m_invMass = 0;
		m_velocity = { 0,0 };
	}
}

void physics::RigidBody::setOrientation(float orientation)
{
	// TODO decide if degrees should be used instead
	m_orientation = fmod(orientation, glm::pi<float>());
}

float physics::RigidBody::calculateEnergy(glm::vec2 gravity)
{
	// TODO change to work with static/kinematic
	if (isDynamic()) {
		float potential = -glm::dot(m_position, gravity) * m_mass;
		float kinetic = 0.5f * m_mass * glm::dot(m_velocity, m_velocity);
		return potential + kinetic;
	}
	else {
		return 0;
	}
}

glm::vec2 physics::RigidBody::calculateMomentum()
{
	if (isDynamic()) {
		return m_velocity * m_mass;
	}
	else {
		return { 0,0 };
	}
}

void physics::RigidBody::resolveCollision(PhysicsObject * other, const Collision & col)
{
	other->resolveRigidbodyCollision(this, col);
}

void physics::RigidBody::resolveRigidbodyCollision(RigidBody * other, const Collision & col)
{

	if (isDynamic() || other->isDynamic()) {
		//TODO get elasticity (from collision? Calculate and get rule from somewhere? Where is rule determined)
		float elasticity = combineElasticity(this, other);
		glm::vec2 normal = col.normal;
		if (col.first != this) {
			normal = -normal;
		}
		glm::vec2 relative = other->getVelocity() - getVelocity();
		float normalRvel = glm::dot(relative, normal);
		if (normalRvel > 0) {
			// Apply impulse
			float impulse = normalRvel * (1 + elasticity) / (m_invMass + other->getInvMass());
			applyImpulseFromOther(other, impulse * normal);
		}
		// Get new relative velocity
		relative = other->getVelocity() - getVelocity();
		normalRvel = glm::dot(relative, normal);
		// TODO maybe check if moving apart fast enough before resolving penetration?
		seperateObjects(other, normal * col.depth);
		
	}
	
}

void physics::RigidBody::resolvePlaneCollision(Plane * other, const Collision & col)
{
	if (isDynamic()) {
		float elasticity = combineElasticity(this, other);
		glm::vec2 normal = col.normal;
		if (col.first != this) {
			normal = -normal;
		}
		glm::vec2 relative = -getVelocity();
		float normalRvel = glm::dot(relative, normal);
		if (normalRvel > 0) {
			// Apply impulse
			float impulse = normalRvel * (1 + elasticity) * m_mass;
			applyImpulse(impulse * normal);
		}
		// TODO maybe only do it if not moving apart fast enough?
		m_position += normal * col.depth;
	}
}

void physics::RigidBody::seperateObjects(RigidBody * other, glm::vec2 displacement)
{
	// TODO maybe proportion = 1/ (invMass + other->invMass), apply movement as proportion * invMass
	float proportion = 1;
	if (isDynamic() && other->isDynamic()) {
		proportion = 0.5f;
	}
	if (isDynamic()) {
		m_position += displacement * proportion;
	}
	if (other->isDynamic()) {
		other->m_position -= displacement * proportion;
	}
}

