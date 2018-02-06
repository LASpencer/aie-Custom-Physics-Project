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


	protected:
		glm::vec2 m_position;
		glm::vec2 m_velocity;
		float m_mass;
		float m_orientation;
		// TODO settings for controlling physics (gravity on/off, dynamic or static)

	};
}