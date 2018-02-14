#include "Plane.h"
#include "Sphere.h"
#include "Box.h"

physics::Plane::Plane(glm::vec2 normal, float distance, float elasticity, glm::vec4 colour)
	: PhysicsObject(elasticity, colour), m_normal(glm::normalize(normal)), m_distance(distance)
{
	if (isinf(distance) || isnan(distance)) {
		throw std::invalid_argument("Distance must be finite");
	}
	if (isnan(m_normal.x) || isnan(m_normal.y)) {
		throw std::invalid_argument("Invalid normal");
	}
}

void physics::Plane::earlyUpdate(float timeStep)
{
}

void physics::Plane::fixedUpdate(glm::vec2 gravity, float timeStep)
{
	// Do nothing
}

void physics::Plane::makeGizmo(float timeRatio)
{
	float lineSegmentLength = 300;
	glm::vec2 centerPoint = -m_normal * m_distance;
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
	float distance = distanceToPoint(other->getPosition());
	if (distance < other->getRadius()) {
		collision.success = true;
		collision.normal = m_normal;
		collision.depth = other->getRadius() - distance;
		collision.contact = other->getPosition() - m_normal * other->getRadius();
	}
	return collision;
}

physics::Collision physics::Plane::checkBoxCollision(Box * other)
{
	return other->checkPlaneCollision(this);
}

physics::Collision physics::Plane::checkPlaneCollision(Plane * other)
{
	return Collision(false, this, other);
}

void physics::Plane::setNormal(glm::vec2 normal)
{
	// TODO check normalization succeeded
	glm::vec2 normalized = glm::normalize(normal);
	if (isnan(normalized.x) || isnan(normalized.y)) {
		throw std::invalid_argument("Invalid normal");
	}
	m_normal = normalized;
}

void physics::Plane::setDistance(float distance)
{
	if (isinf(distance) || isnan(distance)) {
		throw std::invalid_argument("Distance must be finite");
	}
	m_distance = distance;
}

inline float physics::Plane::distanceToPoint(glm::vec2 point)
{
	return glm::dot(point, m_normal) + m_distance;
}

physics::ShapeType physics::Plane::getShapeID()
{
	return ShapeType::plane;
}

void physics::Plane::resolveCollision(PhysicsObject* other, const Collision & col)
{
	other->resolvePlaneCollision(this, col);
}

void physics::Plane::resolveRigidbodyCollision(RigidBody * other, const Collision & col)
{
	other->resolvePlaneCollision(this, col);
}

void physics::Plane::resolvePlaneCollision(Plane* other, const Collision & col)
{
	// Nothing happens, they're both static
}
