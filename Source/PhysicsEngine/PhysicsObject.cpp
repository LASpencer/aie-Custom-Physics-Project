#include "PhysicsObject.h"
#include "ExternalLibraries.h"

physics::PhysicsObject::PhysicsObject(glm::vec4 colour) : m_colour(colour), m_alive(true)
{

}

void physics::PhysicsObject::kill()
{
	m_alive = false;
}
