#include "Sphere.h"
#include "ExternalLibraries.h"
#include "Plane.h"
#include "Box.h"

physics::Sphere::Sphere(glm::vec2 position, float radius, glm::vec2 velocity, float angularVelocity, float mass, float elasticity, float friction, float drag, float angularDrag, glm::vec4 colour, bool showLine)
	: RigidBody(position,velocity, 0,mass,elasticity,angularVelocity,friction, drag, angularDrag,colour), m_radius(radius), m_showLine(showLine)
{

	if (radius <= 0 || isnan(radius) || isinf(radius)) {
		throw std::invalid_argument("Radius must be positive and finite");
	}
	calculateMoment();
}

physics::Sphere::Sphere(const Sphere & other) :
	RigidBody(other), m_radius(other.m_radius)
{
}

physics::PhysicsObject * physics::Sphere::clone()
{
	return new Sphere(*this);
}

void physics::Sphere::makeGizmo(float timeRatio)
{
	glm::vec2 interpolated = glm::mix(m_pastPosition, m_position, timeRatio);
	aie::Gizmos::add2DCircle(interpolated, m_radius, k_segments, m_colour);
	// draw a line to show orientation
	if (m_showLine) {
		glm::vec2 interpolatedLine = glm::mix(m_pastY * m_radius, m_localY * m_radius, timeRatio);
		aie::Gizmos::add2DLine(interpolated, interpolatedLine + interpolated, { 0,0,0,1 });
	}
}

bool physics::Sphere::isPointInside(glm::vec2 point)
{
	glm::vec2 displacement = point - m_position;
	return glm::dot(displacement, displacement) < m_radius * m_radius;
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
			collision.depth = m_radius + other->m_radius - distance;

			// point of contact is at half depth from edge
			collision.contact = (0.5f * collision.depth - m_radius) * collision.normal + m_position;
		}
		else {
			collision.normal = { 1,0 };
			collision.depth = m_radius + other->m_radius;
			collision.contact = m_position;
		}

	}
	return collision;
}

physics::Collision physics::Sphere::checkBoxCollision(Box * other)
{
	return other->checkSphereCollision(this);
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
	if (isDynamic()) {
		calculateMoment();
	}
}

float physics::Sphere::getWidth()
{
	return 2 * m_radius;
}

float physics::Sphere::getHeight()
{
	return 2 * m_radius;
}

float physics::Sphere::getDiagonalLength()
{
	return 2 * m_radius;
}

void physics::Sphere::calculateMoment()
{
	m_moment = 0.5f * m_mass * m_radius * m_radius;
	if (m_moment == INFINITY) {
		m_invMoment = 0;
	}
	else {
		m_invMoment = 1.f / m_moment;
	}
}
