#include "RigidBody.h"
#include "ExternalLibraries.h"

#include "PhysicsScene.h"
#include "Plane.h"

physics::RigidBody::RigidBody(glm::vec2 position, glm::vec2 velocity, float orientation, float mass, float elasticity, float angularVelocity, glm::vec4 colour)
	: PhysicsObject(elasticity, colour), m_position(position),m_velocity(velocity), m_orientation(orientation), m_angularVelocity(angularVelocity)
	, m_totalForce({0,0}), m_totalTorque(0), m_static(false)
{
	if (mass < 0 || isnan(mass)) {
		throw std::invalid_argument("Mass must be positive");
	}
	else {
		if (mass == 0 || isinf(mass)) {
			m_mass = INFINITY;
			m_invMass = 0;
			m_moment = INFINITY;
			m_invMoment = 0;
		}
		else {
			m_mass = mass;
			m_invMass = 1 / mass;
		}
	}

	m_pastPosition = m_position;
	
	calculateAxes();
	m_pastX = m_localX;
	m_pastY = m_localY;
}

physics::RigidBody::RigidBody(const RigidBody & other)
	:PhysicsObject(other), m_position(other.m_position), m_pastPosition(other.m_pastPosition), m_velocity(other.m_velocity),
	m_totalForce({ 0,0 }), m_orientation(other.m_orientation), m_angularVelocity(other.m_angularVelocity), m_totalTorque(0),
	m_localX(other.m_localX),m_localY(other.m_localY),m_pastX(other.m_pastX), m_pastY(other.m_pastY), m_mass(other.m_mass),
	m_invMass(other.m_invMass),m_moment(other.m_moment), m_invMoment(other.m_invMoment), m_static(other.m_static)
{

}

void physics::RigidBody::earlyUpdate(PhysicsScene* scene)
{
}

void physics::RigidBody::fixedUpdate(PhysicsScene* scene)
{
	// Update previous position
	m_pastPosition = m_position; 
	m_pastX = m_localX;
	m_pastY = m_localY;
	if (!m_static) {
		if (!isKinematic()) {
			applyForce(scene->getGravity() * m_mass);
			applyImpulse(m_totalForce * scene->getTimeStep());
			m_angularVelocity += m_totalTorque * m_invMoment * scene->getTimeStep();
		}
		m_position +=  m_velocity * scene->getTimeStep();
		m_orientation += m_angularVelocity * scene->getTimeStep();
		// TODO modulus of 2pi?
		// TODO threshold contraints on velocity/angular velocity
		calculateAxes();
	}
	m_totalForce = { 0,0 };
	m_totalTorque = 0;
}

void physics::RigidBody::applyImpulse(glm::vec2 force)
{
	// TODO decide how to deal with infinite/zero masses
	m_velocity += force * m_invMass;
}

void physics::RigidBody::applyImpulse(glm::vec2 force, glm::vec2 contact)
{
	// TODO apply the torque from this
	// TODO apply some force
	m_velocity += force * m_invMass;
	glm::vec2 pos = contact - m_position;
	m_angularVelocity += (force.y * pos.x - force.x * pos.y) * m_invMoment;
}

void physics::RigidBody::applyImpulseFromOther(RigidBody * other, glm::vec2 force)
{
	applyImpulse(force);
	other->applyImpulse(-force);
}

void physics::RigidBody::applyImpulseFromOther(RigidBody * other, glm::vec2 force, glm::vec2 contact)
{
	applyImpulse(force, contact);
	other->applyImpulse(-force, contact);
}

void physics::RigidBody::applyForce(glm::vec2 force)
{
	m_totalForce += force;
}

void physics::RigidBody::applyForce(glm::vec2 force, glm::vec2 contact)
{
	m_totalForce += force;
	glm::vec2 pos = contact - m_position;
	m_totalTorque += (force.y * pos.x - force.x * pos.y);
}

void physics::RigidBody::applyForceFromOther(RigidBody * other, glm::vec2 force)
{
	applyForce(force);
	other->applyForce(-force);
}

void physics::RigidBody::applyForceFromOther(RigidBody * other, glm::vec2 force, glm::vec2 contact)
{
	applyForce(force, contact);
	other->applyForce(-force, contact);
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
			m_moment = INFINITY;
			m_invMoment = 0;
		}
		else {
			m_mass = mass;
			m_invMass = 1 / mass;
			calculateMoment();
		}
	}
}

float physics::RigidBody::getInvMass()
{
	return m_invMass;
}

float physics::RigidBody::getMoment()
{
	return m_moment;
}

float physics::RigidBody::getInvMoment()
{
	return m_invMoment;
}

void physics::RigidBody::setStatic(bool value)
{
	m_static = value;
	if (m_static) {
		m_mass = INFINITY;
		m_invMass = 0;
		m_moment = INFINITY;
		m_invMoment = 0;
		m_velocity = { 0,0 };
	}
}

float physics::RigidBody::getOrientation()
{
	return m_orientation;
}

void physics::RigidBody::setOrientation(float orientation)
{
	m_orientation = orientation;	// TODO maybe limit to +- 2pi?
	calculateAxes();
}

float physics::RigidBody::getAngularVelocity()
{
	return m_angularVelocity;
}

void physics::RigidBody::setAngularVelocity(float angularVelocity)
{
	m_angularVelocity = angularVelocity;
}

glm::vec2 physics::RigidBody::localToWorldSpace(glm::vec2 localPos)
{
	return m_position + localPos.x * m_localX + localPos.y * m_localY;
}

glm::vec2 physics::RigidBody::pastLocalToWorldSpace(glm::vec2 localPos)
{
	return m_pastPosition + m_pastX * localPos.x + m_pastY * localPos.y;
}

glm::vec2 physics::RigidBody::worldToLocalSpace(glm::vec2 worldPos)
{
	glm::vec2 displacement = worldPos - m_position;
	return { glm::dot(displacement, m_localX), glm::dot(displacement, m_localY) };
}

float physics::RigidBody::calculateEnergy(PhysicsScene* scene)
{
	// TODO change to work with static/kinematic
	if (isDynamic()) {
		float potential = -glm::dot(m_position, scene->getGravity()) * m_mass;
		float kinetic = 0.5f * m_mass * glm::dot(m_velocity, m_velocity);
		float rotational = 0.5f * m_moment * m_angularVelocity * m_angularVelocity;
		return potential + kinetic + rotational;
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
	broadcastCollision(col);
	other->broadcastCollision(col);
	if (isDynamic() || other->isDynamic()) {
		//TODO implement friction between objects
		float friction = 1;
		float elasticity = combineElasticity(this, other);
		glm::vec2 normal = col.normal;
		if (col.first != this) {
			normal = -normal;
		}
		glm::vec2 perpendicular(-normal.y, normal.x);

		//HACK maybe signs on perpendicular/v1 v2 are wrong, check when boxes implemented
		float r1 = glm::dot(col.contact - m_position, perpendicular);
		float t1 = glm::dot(col.contact - m_position, -normal);
		float r2 = glm::dot(col.contact - other->m_position, -perpendicular);
		float t2 = glm::dot(col.contact - other->m_position, normal);

		glm::vec2 relative = other->getVelocity() - getVelocity();
		float normalRvel = glm::dot(relative, normal) + r1 * m_angularVelocity + r2 * other->m_angularVelocity;
		float tangentRvel = glm::dot(relative, perpendicular) + t1 * m_angularVelocity + t2 * other->m_angularVelocity;
		if (normalRvel > 0) {
			float invEffMass1 = m_invMass + r1 * r1 * m_invMoment;
			float invEffMass2 = other->m_invMass + r2 * r2 * other->m_invMoment;

			// Apply impulse
			float impulse = normalRvel * (1 + elasticity) / (invEffMass1 + invEffMass2);
			applyImpulseFromOther(other, impulse * normal, col.contact);
		}
		else {
			// TODO still apply friction for perpendicular hit
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
	broadcastCollision(col);
	other->broadcastCollision(col);
	if (isDynamic()) {
		//TODO implement friction between objects
		float elasticity = combineElasticity(this, other);
		glm::vec2 normal = col.normal;
		if (col.first != this) {
			normal = -normal;
		}
		glm::vec2 perpendicular(-normal.y, normal.x);
		float radius = glm::dot(col.contact - m_position, perpendicular);
		glm::vec2 relative = -getVelocity();
		float normalRvel = glm::dot(relative, normal) + radius * m_angularVelocity;
		if (normalRvel > 0) {
			float invEffMass = m_invMass + radius * radius * m_invMoment;
			// Apply impulse
			float impulse = normalRvel * (1 + elasticity) / invEffMass;
			applyImpulse(impulse * normal, col.contact);
		}
		// TODO maybe only do it if not moving apart fast enough?
		m_position += normal * col.depth;
	}
}

void physics::RigidBody::seperateObjects(RigidBody * other, glm::vec2 displacement)
{
	// TODO maybe proportion = 1/ (invMass + other->invMass), apply movement as proportion * invMass
	 /*float total = m_invMass + other->m_invMass;
	 if (total != 0) {
	     float factor = 1.f / total;
	     m_position += displacement * factor * m_invMass;
	     other->m_position -= displacement * factor * other->m_invMass;
	 }*/
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

void physics::RigidBody::calculateAxes()
{
	float cos = cosf(m_orientation);
	float sin = sinf(m_orientation);
	m_localX = { cos,sin };
	m_localY = { -sin,cos };
}

