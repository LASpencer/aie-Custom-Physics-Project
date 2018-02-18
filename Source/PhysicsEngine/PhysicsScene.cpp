#include "PhysicsScene.h"
#include "ExternalLibraries.h"
#include "PhysicsObject.h"

using namespace physics;

physics::PhysicsScene::PhysicsScene(float timeStep, glm::vec2 gravity) 
	: m_timeStep(timeStep), m_gravity(gravity), m_accumulatedTime(0)
{
	if (timeStep <= 0 || isnan(timeStep) || isinf(timeStep)) {
		throw std::invalid_argument("Timestep must be positive and finite");
	}
}

physics::PhysicsScene::~PhysicsScene()
{
}

bool physics::PhysicsScene::inScene(PhysicsObject * actor)
{
	return std::any_of(m_actors.begin(), m_actors.end(), [actor](PhysicsObjectPtr a) { return a.get() == actor; });
}

bool physics::PhysicsScene::inScene(PhysicsObjectPtr actor)
{
	return std::any_of(m_actors.begin(), m_actors.end(), [actor](PhysicsObjectPtr a) { return a == actor; });
}

bool physics::PhysicsScene::addActor(PhysicsObject * actor)
{
	if (!inScene(actor)) {
		m_actors.push_back(PhysicsObjectPtr(actor));
		return true;
	}
	else {
		return false;
	}
}

bool physics::PhysicsScene::addActor(PhysicsObjectPtr actor)
{
	if (!inScene(actor)) {
		m_actors.push_back(actor);
		return true;
	}
	else {
		return false;
	}
}

bool physics::PhysicsScene::removeActor(PhysicsObject * actor)
{
	//TODO rewrite so m_actors won't be changed during loop
	// maybe change to shared_ptr or have a "will die" flag to avoid dangling pointers
	m_actors.erase(std::remove_if(m_actors.begin(), m_actors.end(), [actor](PhysicsObjectPtr a) {return a.get() == actor; }), m_actors.end());
	actor->kill();
	return true;
}

bool physics::PhysicsScene::removeActor(PhysicsObjectPtr actor)
{
	m_actors.erase(std::remove(m_actors.begin(), m_actors.end(), actor), m_actors.end());
	actor->kill();
	return true;
}

void physics::PhysicsScene::update(float deltaTime)
{
	m_accumulatedTime += deltaTime;

	while (m_accumulatedTime >= m_timeStep) {

		// TODO inform other objects about fixed update
		for (auto actor : m_actors) {
			actor->earlyUpdate(this);
		}
		for (auto actor : m_actors) {
			actor->fixedUpdate(this);
		}
		// TODO collision detection
		for (auto firstActor = m_actors.begin(); firstActor != m_actors.end(); ++firstActor) {
			// TODO skip if set to not collide
			// TODO skip if killed
			for (auto otherActor = firstActor + 1; otherActor != m_actors.end(); ++otherActor) {
				// TODO skip if set to not collide
				// TODO skip if killed
				// TODO check layers and masks
				if (!(*firstActor)->isStatic() || !(*otherActor)->isStatic())
				{
					Collision col = (*firstActor)->checkCollision(otherActor->get());
					//TODO maybe add collisions to list, and resolve collisions all at once
					if (col) {
						resolveCollision(col);
					}
				}
			}
		}
		removeDeadActors();
		m_accumulatedTime -= m_timeStep;
	}
	updateGizmos();
}

void physics::PhysicsScene::updateGizmos()
{
	// TODO have actors create their gizmos
	for (auto actor : m_actors) {
		actor->makeGizmo(m_accumulatedTime/m_timeStep);
	}
}

void physics::PhysicsScene::removeDeadActors()
{
	m_actors.erase(std::remove_if(m_actors.begin(), m_actors.end(), [](PhysicsObjectPtr a) {return !(a->isAlive()); }), m_actors.end());

}

void physics::PhysicsScene::setTimeStep(const float timeStep)
{
	if (timeStep <= 0 || isnan(timeStep) || isinf(timeStep)) {
		throw std::invalid_argument("Timestep must be positive and finite");
	}
	m_timeStep = timeStep;
}

void physics::PhysicsScene::resolveCollision(Collision collision)
{
	//TODO

	//TODO check for static
	collision.first->resolveCollision(collision.second, collision);


}

float physics::PhysicsScene::calculateEnergy()
{
	float energy = 0;
	for (auto actor : m_actors) {
		energy += actor->calculateEnergy(this);
	}
	return energy;
}
