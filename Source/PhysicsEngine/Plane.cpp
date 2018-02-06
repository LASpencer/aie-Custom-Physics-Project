#include "Plane.h"
#include "Sphere.h"

physics::Plane::Plane(glm::vec2 normal, float distance, glm::vec4 colour)
	: PhysicsObject(colour), m_normal(glm::normalize(normal)), m_distance(distance)
{
	//TODO check normalization succeeded
}

void physics::Plane::fixedUpdate(glm::vec2 gravity, float timeStep)
{
	// Do nothing
}

void physics::Plane::makeGizmo()
{
	float lineSegmentLength = 300;
	glm::vec2 centerPoint = m_normal * m_distance;
	glm::vec2 parallel(m_normal.y, -m_normal.x);
	glm::vec2 start = centerPoint + (parallel * lineSegmentLength);
	glm::vec2 end = centerPoint - (parallel * lineSegmentLength);
	aie::Gizmos::add2DLine(start, end, m_colour);
}

physics::Collision physics::Plane::checkCollision(PhysicsObject * other)
{
	return other->checkPlaneCollision(this);
}

physics::Collision physics::Plane::checkSphereCollision(Sphere * other)
{
	Collision collision(false, other, this);
	if (distanceToPoint(other->getPosition()) < other->getRadius()) {
		collision.success = true;
		collision.normal = m_normal;
	}
	return collision;
}

physics::Collision physics::Plane::checkPlaneCollision(Plane * other)
{
	return Collision(false, this, other);
}

void physics::Plane::setNormal(glm::vec2 normal)
{
	// TODO check normalization succeeded
	m_normal = glm::normalize(normal);
}

inline float physics::Plane::distanceToPoint(glm::vec2 point)
{
	return glm::dot(point, m_normal) - m_distance;
}

ShapeType physics::Plane::getShapeID()
{
	return ShapeType::plane;
}