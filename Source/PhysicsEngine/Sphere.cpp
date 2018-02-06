#include "Sphere.h"
#include "ExternalLibraries.h"

void physics::Sphere::makeGizmo()
{
	aie::Gizmos::add2DCircle(m_position, m_radius, 20, m_colour);		//TODO have segments be set as a constant
}

physics::Collision physics::Sphere::checkCollision(PhysicsObject * other)
{
	Collision collision = other->checkSphereCollision(this);
	return collision;
}

physics::Collision physics::Sphere::checkSphereCollision(Sphere * other)
{
	Collision collision = { false, this, other, {0,0} };
	glm::vec2 displacement = m_position - other->m_position;
	float distance = glm::length(displacement);
	if (distance < m_radius + other->m_radius) {
		collision.success = true;
		if (distance != 0) {		//TODO use a "nearly zero" check instead
			collision.normal = glm::normalize(displacement);
		}
		else {
			collision.normal = { 1,0 };
		}
	}
	return collision;
}

ShapeType physics::Sphere::getShapeID()
{
	return ShapeType::sphere;
}
