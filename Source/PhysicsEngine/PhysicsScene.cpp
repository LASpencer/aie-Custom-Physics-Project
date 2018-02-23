#include "PhysicsScene.h"
#include "ExternalLibraries.h"
#include "PhysicsObject.h"

using namespace physics;

const float PhysicsScene::k_def_max_frame = 0.05f;

physics::PhysicsScene::PhysicsScene(float timeStep, glm::vec2 gravity) 
	: m_timeStep(timeStep), m_gravity(gravity), m_accumulatedTime(0), m_maxFrameLength(k_def_max_frame)
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

bool physics::PhysicsScene::inScene(IFixedUpdater * actor)
{
	return std::any_of(m_updaters.begin(), m_updaters.end(), [actor](FixedUpdaterPtr a) { return a.get() == actor; });
}

bool physics::PhysicsScene::inScene(FixedUpdaterPtr actor)
{
	return std::any_of(m_updaters.begin(), m_updaters.end(), [actor](FixedUpdaterPtr a) { return a == actor; });
}

bool physics::PhysicsScene::addUpdater(IFixedUpdater * updater)
{
	if (std::any_of(m_updaterToRemove.begin(), m_updaterToRemove.end(), [updater](IFixedUpdater* a) { return a == updater; })) {
			// If was going to be removed, don't
			m_updaterToRemove.erase(std::remove(m_updaterToRemove.begin(), m_updaterToRemove.end(), updater), m_updaterToRemove.end());
	}
	else if (inScene(updater)) {
		// If already in scene, and not to be removed, just return false
		return false;
	}
	else {
		// Add to toAdd
		m_updaters.push_back(FixedUpdaterPtr(updater));
	}
	return true;
}

bool physics::PhysicsScene::addUpdater(FixedUpdaterPtr updater)
{
	if (std::any_of(m_updaterToRemove.begin(), m_updaterToRemove.end(), [updater](IFixedUpdater* a) { return updater.get() == a; })) {
		// If was going to be removed, don't
		m_updaterToRemove.erase(std::remove_if(m_updaterToRemove.begin(), m_updaterToRemove.end(), [updater](IFixedUpdater* a) { return a == updater.get(); }), m_updaterToRemove.end());
	}
	else if (inScene(updater)){
		// If already in scene, and not to be removed, just return false
		return false;
	}
	else {
		// Add to toAdd
		m_updaters.push_back(FixedUpdaterPtr(updater));
	}
	return true;
}

bool physics::PhysicsScene::removeUpdater(IFixedUpdater * updater)
{
	//TODO test removing updater
	if (!std::any_of(m_updaterToRemove.begin(), m_updaterToRemove.end(), [updater](IFixedUpdater* a) { return a == updater; })) {
			m_updaterToRemove.push_back(updater);
	}
	return true;
}

bool physics::PhysicsScene::removeUpdater(FixedUpdaterPtr updater)
{
	//TODO test removing updater
	if (!std::any_of(m_updaterToRemove.begin(), m_updaterToRemove.end(), [updater](IFixedUpdater* a) { return a == updater.get(); })) {
		// Check if already going to be removed
		m_updaterToRemove.push_back(updater.get());
	}
	return true;
}

void physics::PhysicsScene::update(float deltaTime)
{
	m_accumulatedTime = std::min( m_accumulatedTime +deltaTime, m_maxFrameLength);

	while (m_accumulatedTime >= m_timeStep) {

		removePendingUpdaters();
		for (auto updater : m_updaters) {
			updater->fixedUpdate(this);
		}
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
			for (auto otherActor = std::next(firstActor,1); otherActor != m_actors.end(); ++otherActor) {
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
	float timeRatio = m_accumulatedTime / m_timeStep;
	for (auto actor : m_actors) {
		if (actor->shouldDraw()) {
			actor->makeGizmo(timeRatio);
		}
	}
}

void physics::PhysicsScene::removeDeadActors()
{
	m_actors.erase(std::remove_if(m_actors.begin(), m_actors.end(), [](PhysicsObjectPtr a) {return !(a->isAlive()); }), m_actors.end());

}

void physics::PhysicsScene::removePendingUpdaters()
{
	auto toRemoveBegin = m_updaterToRemove.begin();
	auto toRemoveEnd = m_updaterToRemove.end();
	// Erase all updaters from toRemove list
	m_updaters.erase(std::remove_if(m_updaters.begin(), m_updaters.end(), 
		[toRemoveBegin, toRemoveEnd](FixedUpdaterPtr a) {
			// Check each updater against list to be removed
			return std::any_of(toRemoveBegin, toRemoveEnd, [a](IFixedUpdater* remove) {return a.get() == remove; });
		})
		, m_updaters.end());
	m_updaterToRemove.clear();
}

void physics::PhysicsScene::setTimeStep(const float timeStep)
{
	if (timeStep <= 0 || isnan(timeStep) || isinf(timeStep)) {
		throw std::invalid_argument("Timestep must be positive and finite");
	}
	m_timeStep = timeStep;
}

void physics::PhysicsScene::setMaxFrameLength(const float maxFrameLength)
{
	m_maxFrameLength = maxFrameLength;
}

void physics::PhysicsScene::resolveCollision(const Collision& collision)
{
	// Involved objects inform their subscribers
	collision.first->broadcastCollision(collision);
	collision.second->broadcastCollision(collision);

	// If neither are triggers, resolve collision
	if (!collision.first->isTrigger() && !collision.second->isTrigger()) {
		collision.first->resolveCollision(collision.second, collision);
	}


}

float physics::PhysicsScene::calculateEnergy()
{
	float energy = 0;
	for (auto actor : m_actors) {
		energy += actor->calculateEnergy(this);
	}
	return energy;
}
