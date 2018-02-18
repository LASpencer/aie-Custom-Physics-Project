#include "Joint.h"
#include "RigidBody.h"
#include "Sphere.h"
#include "Plane.h"
#include "Box.h"

physics::Joint::Joint(RigidBodyPtr end1, RigidBodyPtr end2, glm::vec2 anchor1, glm::vec2 anchor2, glm::vec4 colour)
	: PhysicsObject(0.f,colour), m_end1(end1), m_end2(end2), m_anchor1(anchor1), m_anchor2(anchor2)
{
	if (m_end1 == m_end2) {
		m_end2.reset();
	}
}

void physics::Joint::fixedUpdate(PhysicsScene* scene)
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

void physics::Joint::setAnchor1(glm::vec2 anchorPoint)
{
	m_anchor1 = anchorPoint;
}

void physics::Joint::setAnchor2(glm::vec2 anchorPoint)
{
	m_anchor2 = anchorPoint;
}

physics::Collision physics::Joint::checkCollision(PhysicsObject * other)
{
	return Collision(false, this, other);
}

physics::Collision physics::Joint::checkSphereCollision(Sphere * other)
{
	return Collision(false, this, other);
}

physics::Collision physics::Joint::checkBoxCollision(Box * other)
{
	return Collision(false, this,other);
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
