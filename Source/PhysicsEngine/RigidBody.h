#pragma once
#include "ExternalLibraries.h"
#include "PhysicsObject.h"

namespace physics
{
	class RigidBody : public PhysicsObject
	{
	public:
		RigidBody(glm::vec2 position, glm::vec2 velocity, float orientation, float mass, glm::vec4 colour);


		virtual void fixedUpdate(glm::vec2 gravity, float timestep);

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

	protected:
		glm::vec2 m_position;
		glm::vec2 m_velocity;
		float m_mass;
		float m_orientation;
		// TODO settings for controlling physics (gravity on/off, dynamic or static)

	};
}