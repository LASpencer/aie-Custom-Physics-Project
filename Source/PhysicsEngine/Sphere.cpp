#include "Sphere.h"
#include "ExternalLibraries.h"
#include "Plane.h"

physics::Sphere::Sphere(glm::vec2 position, glm::vec2 velocity, float radius, float mass, glm::vec4 colour)
	: RigidBody(position,velocity, 0,mass,colour), m_radius(radius)
{

	if (radius <= 0 || isnan(radius) || isinf(radius)) {
		throw std::invalid_argument("Radius must be positive and finite");
	}
}

void physics::Sphere::makeGizmo(float timeRatio)
{
	glm::vec2 interpolated = glm::mix(m_pastPosition, m_position, timeRatio );
	aie::Gizmos::add2DCircle(interpolated, m_radius, 20, m_colour);		//TODO have segments be set as a constant
}

physics::Collision physics::Sphere::checkCollision(PhysicsObject * other)
{
	return other->checkSphereCollision(this);
}

physics::Collision physics::Sphere::checkSphereCollision(Sphere * other)
{
	Collision collision(false, this, other);
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

physics::Collision physics::Sphere::checkPlaneCollision(Plane * other)
{
	return other->checkSphereCollision(this);
}


physics::ShapeType physics::Sphere::getShapeID()
{
	return ShapeType::sphere;
}

void physics::Sphere::setRadius(float radius)
{
	if (radius <= 0 || isnan(radius) || isinf(radius)) {
		throw std::invalid_argument("Radius must be positive and finite");
	}
	m_radius = radius;
}
