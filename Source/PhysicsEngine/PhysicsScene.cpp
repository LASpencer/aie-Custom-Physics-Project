#include "PhysicsScene.h"
#include "ExternalLibraries.h"
#include "PhysicsObject.h"

physics::PhysicsScene::PhysicsScene(float timeStep, glm::vec2 gravity) 
	: m_timeStep(timeStep), m_gravity(gravity), m_accumulatedTime(0)
{

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
				Collision col = (*firstActor)->checkCollision(*otherActor);
				if (col) {
					(*firstActor)->resolveCollision(col);
				}
			}
		}

		m_accumulatedTime -= m_timeStep;
	}
}

void physics::PhysicsScene::updateGizmos()
{
	// TODO have actors create their gizmos
}
