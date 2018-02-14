#include "Box.h"

#include "Plane.h"
#include "Sphere.h"

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

physics::Collision physics::Box::checkSphereCollision(Sphere * other)
{
	glm::vec2 circleLocalPos = worldToLocalSpace(other->getPosition());
	bool withinWidth = abs(circleLocalPos.x) <= m_xExtent;
	bool withinHeight = abs(circleLocalPos.y) <= m_yExtent;
	if (withinHeight && withinWidth) {
		// TODO centre entirely within box
	}
	else if (withinWidth) {
		//TODO check circle.y is less that yExtent + radius
	}
	else if (withinHeight) {
		// TODO check circle.x is less than xExtent + radius
	}
	else {
		// TODO check corner distances 
	}

	return Collision();
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

physics::Collision physics::Box::checkPlaneCollision(Plane * other)
{
	Collision collision(false, this, other);
	std::array<glm::vec2, 4> corners = getCorners();
	float minDistance = INFINITY;
	float sumDepth = 0;
	glm::vec2 contact = { 0,0 };
	// Check each corner's distance to plane, and get lowest
	for (size_t i = 0; i < 4; ++i) {
		float distance = other->distanceToPoint(corners[i]);
		if (distance < minDistance) {
			minDistance = distance;
		}
		if (distance < 0) {
			// If colliding, add to contact for weighted average
			collision.success = true;
			sumDepth += distance;
			contact += corners[i] * distance;
		}
	}
	if (collision.success) {
		collision.depth = -minDistance;
		collision.normal = other->getNormal();
		collision.contact = contact / sumDepth;
	}
	return collision;
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

std::array<glm::vec2, 4> physics::Box::getCorners()
{
	glm::vec2 x = m_xExtent * m_localX;
	glm::vec2 y = m_yExtent * m_localY;
	return { m_position - x - y, m_position + x - y,
			 m_position + x + y, m_position - x + y };
}
