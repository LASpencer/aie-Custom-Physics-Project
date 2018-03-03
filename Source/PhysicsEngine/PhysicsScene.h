#pragma once
#include "ExternalLibraries.h"

#include "IFixedUpdater.h"

namespace physics {
	class PhysicsObject;
	struct Collision;

	typedef std::shared_ptr<PhysicsObject> PhysicsObjectPtr;
	typedef std::weak_ptr<PhysicsObject> PhysicsObjectWeakPtr;

	class PhysicsScene {
	public:
		static const float k_def_max_frame;

		PhysicsScene(float timeStep = 0.01f, glm::vec2 gravity = glm::vec2(0,-10));
		~PhysicsScene();

		bool inScene(PhysicsObject* actor);
		bool inScene(PhysicsObjectPtr actor);

		bool addActor(PhysicsObject* actor);
		bool addActor(PhysicsObjectPtr actor);

		bool removeActor(PhysicsObject* actor);
		bool removeActor(PhysicsObjectPtr actor);

		bool inScene(IFixedUpdater* updater);
		bool inScene(FixedUpdaterPtr updater);

		bool addUpdater(IFixedUpdater* updater);
		bool addUpdater(FixedUpdaterPtr updater);

		bool removeUpdater(IFixedUpdater* updater);
		bool removeUpdater(FixedUpdaterPtr updater);

		// clears updaters and actors
		void clear();

		void update(float deltaTime);

		std::vector<PhysicsObjectPtr> const& getActors() { return m_actors; }
	
		glm::vec2 getGravity() { return m_gravity; }
		void setGravity(glm::vec2 gravity) { m_gravity = gravity; }

		void setTimeStep(const float timeStep);
		float getTimeStep() const { return m_timeStep; }

		void setMaxFrameLength(const float maxFrameLength);
		float getMaxFrameLength() const { return m_maxFrameLength; }

		void resolveCollision(const Collision& collision);

		float calculateEnergy();

	protected:
		glm::vec2 m_gravity;
		float m_timeStep;
		float m_maxFrameLength;
		float m_accumulatedTime;
		std::vector<PhysicsObjectPtr> m_actors;
		std::vector<FixedUpdaterPtr> m_updaters;
		std::vector<IFixedUpdater *> m_updaterToRemove;

		void updateGizmos();

		void removeDeadActors();

		void removePendingUpdaters();
	};
}