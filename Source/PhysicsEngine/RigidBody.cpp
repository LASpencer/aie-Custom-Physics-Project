#include "RigidBody.h"
#include "ExternalLibraries.h"

#include "PhysicsScene.h"
#include "Plane.h"

physics::RigidBody::RigidBody(glm::vec2 position, glm::vec2 velocity, float orientation, float mass, float elasticity, float angularVelocity, float friction, float drag, float angularDrag, glm::vec4 colour)
	: PhysicsObject(elasticity, friction, colour), m_position(position),m_velocity(velocity), m_orientation(remainderf(orientation, glm::two_pi<float>())),
	m_angularVelocity(angularVelocity), m_totalForce({0,0}), m_totalTorque(0), m_static(false)
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

	setDrag(drag);

	setAngularDrag(angularDrag);

	m_pastPosition = m_position;
	
	calculateAxes();
	m_pastX = m_localX;
	m_pastY = m_localY;
}

physics::RigidBody::RigidBody(const RigidBody & other)
	:PhysicsObject(other), m_position(other.m_position), m_pastPosition(other.m_pastPosition), m_velocity(other.m_velocity),
	m_totalForce({ 0,0 }), m_orientation(other.m_orientation), m_angularVelocity(other.m_angularVelocity), m_totalTorque(0),
	m_localX(other.m_localX),m_localY(other.m_localY),m_pastX(other.m_pastX), m_pastY(other.m_pastY), m_mass(other.m_mass),
	m_invMass(other.m_invMass),m_moment(other.m_moment), m_invMoment(other.m_invMoment), m_static(other.m_static), m_drag(other.m_drag),
	m_angularDrag(other.m_angularDrag)
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

			// Apply drag
			float dragImpulse = std::min(m_drag * scene->getTimeStep() * m_invMass, 1.f);
			m_velocity -= dragImpulse * m_velocity;

			float dragTorque = std::min(m_angularDrag * scene->getTimeStep() * m_invMoment, 1.f);
			m_angularVelocity -= dragTorque * m_angularVelocity;

			applyImpulse(m_totalForce * scene->getTimeStep());
			m_angularVelocity += m_totalTorque * m_invMoment * scene->getTimeStep();
		}
		m_position +=  m_velocity * scene->getTimeStep();
		m_orientation += m_angularVelocity * scene->getTimeStep();
		// modulus 2pi
		m_orientation = remainderf(m_orientation, glm::two_pi<float>());
		calculateAxes();
	}
	m_totalForce = { 0,0 };
	m_totalTorque = 0;
}

void physics::RigidBody::applyImpulse(glm::vec2 force)
{
	// if not dynamic, invMass will be 0
	m_velocity += force * m_invMass;
}

void physics::RigidBody::applyImpulse(glm::vec2 force, glm::vec2 contact)
{
	m_velocity += force * m_invMass;
	// Calculate and apply torque
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
	// Keep running total of force and torque to apply at update
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
			 // 0 or infinity becomes kinematic
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

void physics::RigidBody::setDrag(float drag)
{
	m_drag = abs(drag);
}

void physics::RigidBody::setAngularDrag(float drag)
{
	m_angularDrag = abs(drag);
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
		m_angularVelocity = 0;
	}
}

float physics::RigidBody::getOrientation()
{
	return m_orientation;
}

void physics::RigidBody::setOrientation(float orientation)
{
	m_orientation = remainderf(orientation, glm::two_pi<float>());	// limit to +- 2pi
	calculateAxes();
}

float physics::RigidBody::getAngularVelocity()
{
	return m_angularVelocity;
}

void physics::RigidBody::setAngularVelocity(float angularVelocity)
{
	if (!m_static) {
		m_angularVelocity = angularVelocity;
	}
}

void physics::RigidBody::resetAlive()
{
	m_alive = true;
	// Reset force and torque to 0
	m_totalForce = { 0,0 };
	m_totalTorque = 0;
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
	
	if (isDynamic() || other->isDynamic()) {
		float friction = combineFriction(this, other);
		float elasticity = combineElasticity(this, other);
		glm::vec2 normal = col.normal;
		if (col.first != this) {
			normal = -normal;
		}
		glm::vec2 perpendicular(-normal.y, normal.x);

		// Calculate point of contact's effective radius both along normal and tangent
		float r1 = glm::dot(col.contact - m_position, perpendicular);
		float t1 = glm::dot(col.contact - m_position, -normal);
		float r2 = glm::dot(col.contact - other->m_position, -perpendicular);
		float t2 = glm::dot(col.contact - other->m_position, normal);

		// Get velocity along collision normal
		glm::vec2 relative = other->getVelocity() - getVelocity();
		float normalRvel = glm::dot(relative, normal) + r1 * m_angularVelocity + r2 * other->m_angularVelocity;
		
		if (normalRvel > 0) {
			// Effective inverse mass, taking angular moment into account
			float invEffMass1 = m_invMass + r1 * r1 * m_invMoment;
			float invEffMass2 = other->m_invMass + r2 * r2 * other->m_invMoment;

			// Effective inverse mass for tangent force
			float invEffTanMass1 = m_invMass + t1 * t1*m_invMoment;
			float invEffTanMass2 = other->m_invMass + t2 * t2 * other->m_invMoment;

			// Calculate normal impulse
			float impulse = normalRvel * (1 + elasticity) / (invEffMass1 + invEffMass2);

			applyImpulseFromOther(other, impulse * normal, col.contact);

			relative = other->getVelocity() - getVelocity();
			float tangentRvel = glm::dot(relative, perpendicular) + t1 * m_angularVelocity + t2 * other->m_angularVelocity;

			float frictionToStop = tangentRvel / (invEffTanMass1 + invEffTanMass2);
			// TODO if tangentRvel = 0, maybe get difference in forces to each, get length along perpendicular, and apply that?
			// Calculate maximum friction and clamp between that
			float maxFriction = friction * impulse;
			float frictionImpulse = std::max(std::min(frictionToStop, maxFriction), -maxFriction);
			applyImpulseFromOther(other, frictionImpulse * perpendicular, col.contact);
		}
		seperateObjects(other, normal * col.depth);
	}
	
}

void physics::RigidBody::resolvePlaneCollision(Plane * other, const Collision & col)
{
	if (isDynamic()) {
		float friction = combineFriction(this, other);
		float elasticity = combineElasticity(this, other);
		glm::vec2 normal = col.normal;
		if (col.first != this) {
			normal = -normal;
		}
		glm::vec2 perpendicular(-normal.y, normal.x);
		float radius = glm::dot(col.contact - m_position, perpendicular);
		float tangentRadius = glm::dot(col.contact - m_position, -normal);
		glm::vec2 relative = -getVelocity();
		float normalRvel = glm::dot(relative, normal) + radius * m_angularVelocity;
		if (normalRvel > 0) {
			float invEffMass = m_invMass + radius * radius * m_invMoment;
			float invEffTanMas = m_invMass + tangentRadius * tangentRadius * m_invMoment;

			// Apply impulse
			float impulse = normalRvel * (1 + elasticity) / invEffMass;
			applyImpulse(impulse * normal, col.contact);

			relative = -getVelocity();
			float tangentRvel = glm::dot(relative, perpendicular) + tangentRadius * m_angularVelocity;
			float frictionToStop = tangentRvel / invEffTanMas;
			float maxFriction = friction * impulse;
			float frictionImpulse = std::max(std::min(frictionToStop, maxFriction), -maxFriction);
			applyImpulse(frictionImpulse * perpendicular, col.contact);
		}
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

