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
	// Destroy remaining objects
	for (PhysicsObject* actor : m_actors) {
		delete actor;
	}
}

bool physics::PhysicsScene::addActor(PhysicsObject * actor)
{
	m_actors.push_back(actor);
	return true;
}

bool physics::PhysicsScene::removeActor(PhysicsObject * actor)
{
	//TODO rewrite so m_actors won't be changed during loop
	// maybe change to shared_ptr or have a "will die" flag to avoid dangling pointers
	m_actors.erase(std::remove(m_actors.begin(), m_actors.end(), actor), m_actors.end());
	delete actor;
	return true;
}

void physics::PhysicsScene::update(float deltaTime)
{
	m_accumulatedTime += deltaTime;

	while (m_accumulatedTime >= m_timeStep) {

		// TODO inform other objects about fixed update
		for (auto actor : m_actors) {
			actor->fixedUpdate(m_gravity, m_timeStep);
		}
		// TODO collision detection
		for (auto firstActor = m_actors.begin(); firstActor != m_actors.end(); ++firstActor) {
			// TODO skip if set to not collide
			for (auto otherActor = firstActor + 1; otherActor != m_actors.end(); ++otherActor) {
				// TODO skip if set to not collide
				// TODO check layers and masks
				if (!(*firstActor)->isStatic() || !(*otherActor)->isStatic())
				{
					Collision col = (*firstActor)->checkCollision(*otherActor);
					//TODO maybe add collisions to list, and resolve collisions all at once
					if (col) {
						resolveCollision(col);
					}
				}
			}
		}

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
