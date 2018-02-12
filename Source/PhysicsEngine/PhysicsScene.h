#pragma once
#include "ExternalLibraries.h"

namespace physics {
	class PhysicsObject;
	struct Collision;

	class PhysicsScene {
	public:
		PhysicsScene(float timeStep = 0.01f, glm::vec2 gravity = glm::vec2(0,-10)); //TODO default gravity, timestep arguments
		~PhysicsScene();

		bool addActor(PhysicsObject* actor);
		bool removeActor(PhysicsObject* actor);
		// TODO have some kind of FixedUpdateListener object that can have FixedUpdate called

		void update(float deltaTime);
	

		glm::vec2 getGravity() { return m_gravity; }
		void setGravity(glm::vec2 gravity) { m_gravity = gravity; }

		void setTimeStep(const float timeStep);
		float getTimeStep() const { return m_timeStep; }

		void resolveCollision(Collision collision);

		float calculateEnergy();

	protected:
		glm::vec2 m_gravity;
		float m_timeStep;
		float m_accumulatedTime;
		std::vector<PhysicsObject*> m_actors;		//TODO start using smart pointers

		void updateGizmos();


	};
}