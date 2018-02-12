#pragma once
#include "ExternalLibraries.h"
#include "PhysicsObject.h"

namespace physics
{
	class RigidBody;

	typedef std::shared_ptr<RigidBody> RigidBodyPtr;
	typedef std::weak_ptr<RigidBody> RigidBodyWeakPtr;

	class RigidBody : public PhysicsObject
	{
	public:
		RigidBody(glm::vec2 position, glm::vec2 velocity, float orientation, float mass, float elasticity, glm::vec4 colour);


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

		float getMass();
		void setMass(float mass);

		float getInvMass();

		virtual bool isStatic() { return m_static; };

		// Sets body as static. On becoming static, body gets infinite mass and zero velocity
		void setStatic(bool value);

		inline bool isKinematic() { return m_invMass == 0; }

		inline bool isDynamic() { return !(isKinematic() || isStatic()); };

		float getOrientation() { return m_orientation; }
		void setOrientation(float orientation);

		virtual float calculateEnergy(glm::vec2 gravity);

		virtual glm::vec2 calculateMomentum();

		virtual void resolveCollision(PhysicsObject* other, const Collision & col) override;
		virtual void resolveRigidbodyCollision(RigidBody * other, const Collision & col) override;
		virtual void resolvePlaneCollision(Plane* other, const Collision & col) override;

	protected:
		glm::vec2 m_position;
		glm::vec2 m_pastPosition; // Used to avoid temporal aliasing
		glm::vec2 m_velocity;
		glm::vec2 m_totalForce;
		
		float m_mass;
		float m_invMass;
		float m_orientation;

		bool m_static;
		// TODO settings for controlling physics (gravity on/off, dynamic or static)

		void seperateObjects(RigidBody* other, glm::vec2 displacement);
	};

}