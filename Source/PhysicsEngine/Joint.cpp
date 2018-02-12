#include "Joint.h"
#include "RigidBody.h"
#include "Sphere.h"
#include "Plane.h"

physics::Joint::Joint(RigidBodyPtr end1, RigidBodyPtr end2, glm::vec4 colour) 
	: PhysicsObject(0.f,colour), m_end1(end1), m_end2(end2)
{
	if (m_end1 == m_end2) {
		m_end2.reset();
	}
}

void physics::Joint::fixedUpdate(glm::vec2 gravity, float timestep)
{
}

bool physics::Joint::setEnd1(RigidBodyPtr end)
{
	if (end != m_end2) {
		m_end1 = end;
		return true;
	}
	else {
		return false;
	}
}

bool physics::Joint::setEnd2(RigidBodyPtr end)
{
	if (end != m_end1) {
		m_end2 = end;
		return true;
	}
	else {
		return false;
	}
}

physics::Collision physics::Joint::checkCollision(PhysicsObject * other)
{
	return Collision(false, this, other);
}

physics::Collision physics::Joint::checkSphereCollision(Sphere * other)
{
	return Collision(false, this, other);
}

physics::Collision physics::Joint::checkPlaneCollision(Plane * other)
{
	return Collision(false, this, other);
}

void physics::Joint::resolveCollision(PhysicsObject * other, const Collision & col)
{
	// Does nothing
}

void physics::Joint::resolveRigidbodyCollision(RigidBody * other, const Collision & col)
{
}

void physics::Joint::resolvePlaneCollision(Plane * other, const Collision & col)
{
}

void physics::Joint::removeKilledEnd()
{
	if (m_end1 && !m_end1->isAlive()) {
		m_end1.reset();
	}
	if (m_end2 && !m_end2->isAlive()) {
		m_end2.reset();
	}
}
