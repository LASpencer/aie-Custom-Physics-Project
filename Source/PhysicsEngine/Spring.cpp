#include "Spring.h"

#include "ExternalLibraries.h"

#include "Rigidbody.h"

physics::Spring::Spring(float tightness, float length, float damping, 
	RigidBodyPtr end1, RigidBodyPtr end2, glm::vec2 anchor1, glm::vec2 anchor2, glm::vec4 colour)
	: Joint(end1,end2,anchor1,anchor2,colour), m_tightness(tightness), m_length(length), m_damping(damping)
{
	if (m_tightness < 0 || isnan(m_tightness) || isinf(m_tightness)) {
		throw std::invalid_argument("Tightness must be non-negative and finite");
	}
	if (m_length < 0 || isnan(m_length) || isinf(m_length)) {
		throw std::invalid_argument("Length must be non-negative and finite");
	}
	if (m_damping < 0 || isnan(m_damping) || isinf(m_damping)) {
		throw std::invalid_argument("Damping must be non-negative and finite");
	}
}

void physics::Spring::setTightness(float tightness)
{
	if (tightness < 0 || isnan(tightness) || isinf(tightness)) {
		throw std::invalid_argument("Tightness must be non-negative and finite");
	}
	m_tightness = tightness;
}

void physics::Spring::setLength(float length)
{
	if (length < 0 || isnan(length) || isinf(length)) {
		throw std::invalid_argument("Length must be non-negative and finite");
	}
	m_length = length;
}

void physics::Spring::setDamping(float damping)
{
	if (damping < 0 || isnan(damping) || isinf(damping)) {
		throw std::invalid_argument("Damping must be non-negative and finite");
	}
	m_damping = damping;
}

void physics::Spring::earlyUpdate(float timestep)
{
	if (m_end1 && m_end2 && m_end1->isAlive() && m_end2->isAlive()) {
		// get distance between ends, calculate difference to length, multiply by tightness
		glm::vec2 pos1 = m_end1->localToWorldSpace(m_anchor1);
		glm::vec2 pos2 = m_end2->localToWorldSpace(m_anchor2);
		glm::vec2 displacement = pos2 - pos1;
		float distance = glm::length(displacement);
		glm::vec2 direction = { 1,0 };
		if (distance > 0) {
			direction = glm::normalize(displacement);
		}
		glm::vec2 perpendicular(-direction.y, direction.x);
		glm::vec2 force = direction * (distance - m_length) * m_tightness;
		// get relative velocity along spring, multiply by damping
		
		// Calculate movement along spring due to rotation
		// TODO write tests to make sure signs are right
		float rot1 = glm::dot(pos1 - m_end1->getPosition(), -perpendicular) * m_end1->getAngularVelocity();
		float rot2 = glm::dot(pos2 - m_end2->getPosition(), perpendicular) * m_end2->getAngularVelocity();

		glm::vec2 rVel = m_end1->getVelocity() - m_end2->getVelocity();
		float rSpeed = glm::dot(rVel, direction) + rot1 + rot2;
		force -= rSpeed * direction * m_damping;
		// apply force to both
		//m_end1->applyImpulseFromOther(m_end2.get(), force * timestep);
		m_end1->applyForce(force, pos1);
		m_end2->applyForce(-force, pos2);
	}
	else {
		removeKilledEnd();
	}
}

void physics::Spring::makeGizmo(float timeRatio)
{
	if (m_end1 && m_end2) {
		glm::vec2 pos1 = glm::mix(m_end1->pastLocalToWorldSpace(m_anchor1),
									m_end1->localToWorldSpace(m_anchor1), timeRatio);

		glm::vec2 pos2 = glm::mix(m_end2->pastLocalToWorldSpace(m_anchor2),
									m_end2->localToWorldSpace(m_anchor2), timeRatio);

		aie::Gizmos::add2DLine(pos1, pos2, m_colour);
	}
}

float physics::Spring::calculateEnergy(glm::vec2 gravity)
{
	if (m_end1 && m_end2 && m_end1->isAlive() && m_end2->isAlive()) {
		// get distance between ends, calculate difference to length, multiply by tightness
		glm::vec2 pos1 = m_end1->localToWorldSpace(m_anchor1);
		glm::vec2 pos2 = m_end2->localToWorldSpace(m_anchor2);
		glm::vec2 displacement = pos2 - pos1;
		float distance = m_length - glm::length(displacement);
		return 0.5f * distance * distance * m_tightness;
	}
	else {
		return 0;
	}
}
