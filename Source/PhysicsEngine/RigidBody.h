#pragma once
#include "ExternalLibraries.h"
#include "PhysicsObject.h"

namespace physics
{
	class RigidBody : public PhysicsObject
	{
	public:
		RigidBody(glm::vec2 position, glm::vec2 velocity, float orientation, float mass, glm::vec4 colour);


		virtual void earlyUpdate(float timestep);

		virtual void fixedUpdate(glm::vec2 gravity, float timestep);

		void applyImpulse(glm::vec2 force);
		void applyImpulseFromOther(RigidBody* other, glm::vec2 force);

		void applyForce(glm::vec2 force);

		void applyForceFromOther(RigidBody* other, glm::vec2 force);

		glm::vec2 getPosition() { return m_position; }
		void setPosition(glm::vec2 position);

		glm::vec2 getVelocity() { return m_velocity; }
		void setVelocity(glm::vec2 velocity);

		float getMass() { return m_mass; }
		void setMass(float mass);

		float getOrientation() { return m_orientation; }
		void setOrientation(float orientation);

		virtual bool calculateEnergy(glm::vec2 gravity);

		virtual glm::vec2 calculateMomentum();

	protected:
		glm::vec2 m_position;
		glm::vec2 m_pastPosition; // Used to avoid temporal aliasing
		glm::vec2 m_velocity;
		glm::vec2 m_totalForce;
		float m_mass;
		float m_orientation;
		// TODO settings for controlling physics (gravity on/off, dynamic or static)

	};
}