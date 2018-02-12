#include "PhysicsObject.h"
#include "ExternalLibraries.h"

physics::PhysicsObject::PhysicsObject(float elasticity, glm::vec4 colour) : m_elasticity(elasticity), m_colour(colour), m_alive(true)
{

}

float physics::PhysicsObject::combineElasticity(PhysicsObject* e1, PhysicsObject* e2)
{
	return e1->getElasticity() * e2->getElasticity();
}

void physics::PhysicsObject::kill()
{
	m_alive = false;
}

void physics::PhysicsObject::setElasticity(float elasticity)
{
	m_elasticity = std::fmax(std::fmin(elasticity, 1.f), 0.f);
}
