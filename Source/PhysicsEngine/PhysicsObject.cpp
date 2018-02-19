#include "PhysicsObject.h"
#include "ExternalLibraries.h"
#include "ICollisionObserver.h"

physics::PhysicsObject::PhysicsObject(float elasticity, glm::vec4 colour) : m_elasticity(elasticity), m_colour(colour), m_alive(true)
{

}

void physics::PhysicsObject::broadcastCollision(const Collision & collision)
{
	auto current = m_observers.begin();
	while (current != m_observers.end()) {
		if (current->expired()) {
			current = m_observers.erase(current);
		}
		else {
			current->lock()->OnCollision(this, collision);
			++current;
		}
	}
}

float physics::PhysicsObject::combineElasticity(PhysicsObject* e1, PhysicsObject* e2)
{
	return e1->getElasticity() * e2->getElasticity();
}

void physics::PhysicsObject::kill()
{
	m_alive = false;
}

bool physics::PhysicsObject::addObserver(const CollisionObserverPtr & observer)
{
	// TODO test add observer
	// Check if already subscribed
	if (!isSubscribed(observer)) {
		m_observers.push_back(CollisionObserverWeakPtr(observer));
		return true;
	}
	return false;
}

bool physics::PhysicsObject::removeObserver(const CollisionObserverPtr & observer)
{
	// TODO test remove observer
	for_each(m_observers.begin(), m_observers.end(),
		[observer](CollisionObserverWeakPtr o) {
			if (o.lock() == observer) {
				o.reset();
			}
		}
	);
	return true;
}

bool physics::PhysicsObject::isSubscribed(const CollisionObserverPtr & observer)
{
	return std::any_of(m_observers.begin(), m_observers.end(), [observer](CollisionObserverWeakPtr c) {return c.lock() == observer; });
}

void physics::PhysicsObject::setElasticity(float elasticity)
{
	m_elasticity = std::max(std::min(elasticity, 1.f), 0.f);
}
