#include "Box.h"

physics::Box::Box(glm::vec2 position, float width, float height, float orientation, glm::vec2 velocity,
	float angularVelocity, float mass, float elasticity, glm::vec4 colour) :
	RigidBody(position,velocity,orientation,mass,elasticity,angularVelocity,colour), m_xExtent(0.5f * width), m_yExtent(0.5f * height)
{
	if (m_xExtent <= 0 || isnan(m_xExtent) || isinf(m_xExtent)) {
		throw std::invalid_argument("Width must be positive and finite");
	}
	if (m_yExtent <= 0 || isnan(m_yExtent) || isinf(m_yExtent)) {
		throw std::invalid_argument("Height must be positive and finite");
	}
	calculateMoment();
}

void physics::Box::makeGizmo(float timeRatio)
{
	glm::vec2 interpPos = glm::mix(m_pastPosition, m_position, timeRatio);
	// HACK not normalizing to save time, might need to do so if it looks bad/based on speed
	glm::vec2 interX = glm::mix(m_pastX, m_localX , timeRatio) * m_xExtent;
	glm::vec2 interY = glm::mix(m_pastY, m_localY, timeRatio) * m_yExtent;

	glm::vec2 p1 = m_position - interX - interY;
	glm::vec2 p2 = m_position + interX - interY;
	glm::vec2 p3 = m_position + interX + interY;
	glm::vec2 p4 = m_position - interX + interY;

	aie::Gizmos::add2DTri(p1, p2, p3, m_colour);
	aie::Gizmos::add2DTri(p1, p3, p4, m_colour);
}

physics::Collision physics::Box::checkCollision(PhysicsObject * other)
{
	return other->checkBoxCollision(this);
}

void physics::Box::calculateMoment()
{
	// based on 1/12 * m * width * height. Some sources say 1/12 * mass * (width^2 + height^2)
	m_moment = (4.f / 12.f) * m_mass * m_xExtent * m_yExtent;	// 4/12 because extents are half total
	if (m_moment == INFINITY) {
		m_invMoment = 0;
	}
	else {
		m_invMoment = 1.f / m_moment;
	}
}

physics::ShapeType physics::Box::getShapeID()
{
	return ShapeType::obox;
}

float physics::Box::getWidth()
{
	return 2 * m_xExtent;
}

float physics::Box::setWidth(float width)
{
	if (width <= 0 || isnan(width) || isinf(width)) {
		throw std::invalid_argument("Width must be positive and finite");
	}
	m_xExtent = 0.5f * width;
}

float physics::Box::getHeight()
{
	return 2 * m_yExtent;
}

float physics::Box::setHeight(float height)
{
	if (height <= 0 || isnan(height) || isinf(height)) {
		throw std::invalid_argument("Height must be positive and finite");
	}
	m_yExtent = 0.5f * height;
}

glm::vec2 physics::Box::getXExtent()
{
	return m_localX * m_xExtent;
}

glm::vec2 physics::Box::getYExtent()
{
	return m_localY * m_yExtent;
}
