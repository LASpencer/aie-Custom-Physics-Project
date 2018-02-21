#pragma once
#include "ExternalLibraries.h"
#include "PhysicsObject.h"

namespace physics
{
	class PhysicsScene;
	class RigidBody;

	typedef std::shared_ptr<RigidBody> RigidBodyPtr;
	typedef std::weak_ptr<RigidBody> RigidBodyWeakPtr;

	class RigidBody : public PhysicsObject
	{
	public:
		RigidBody(glm::vec2 position, glm::vec2 velocity, float orientation, float mass, float elasticity, float angularVelocity, float friction, float drag, float angularDrag, glm::vec4 colour);

		RigidBody(const RigidBody& other);

		virtual void earlyUpdate(PhysicsScene* scene);

		virtual void fixedUpdate(PhysicsScene* scene);

		void applyImpulse(glm::vec2 force);
		void applyImpulse(glm::vec2 force, glm::vec2 contact);

		void applyImpulseFromOther(RigidBody* other, glm::vec2 force);
		void applyImpulseFromOther(RigidBody* other, glm::vec2 force, glm::vec2 contact);

		void applyForce(glm::vec2 force);
		void applyForce(glm::vec2 force, glm::vec2 contact);

		void applyForceFromOther(RigidBody* other, glm::vec2 force);
		void applyForceFromOther(RigidBody* other, glm::vec2 force, glm::vec2 contact);

		glm::vec2 getPosition() { return m_position; }
		void setPosition(glm::vec2 position);

		glm::vec2 getPastPosition() { return m_pastPosition; }

		glm::vec2 getVelocity() { return m_velocity; }
		void setVelocity(glm::vec2 velocity);

		float getOrientation();
		void setOrientation(float orientation);

		glm::vec2 getLocalX() { return m_localX; }
		glm::vec2 getLocalY() { return m_localY; }

		virtual float getWidth() = 0;
		virtual float getHeight() = 0;

		float getAngularVelocity();
		void setAngularVelocity(float angularVelocity);

		float getMass();
		void setMass(float mass);

		float getDrag() { return m_drag; }
		void setDrag(float drag);

		float getAngularDrag() { return m_angularDrag; }
		void setAngularDrag(float drag);

		float getInvMass();

		float getMoment();

		float getInvMoment();

		virtual bool isStatic() { return m_static; };

		// Sets body as static. On becoming static, body gets infinite mass and zero velocity
		void setStatic(bool value);

		inline bool isKinematic() { return m_invMass == 0; }

		inline bool isDynamic() { return !(isKinematic() || isStatic()); };

		glm::vec2 localToWorldSpace(glm::vec2 localPos);

		glm::vec2 pastLocalToWorldSpace(glm::vec2 localPos);

		glm::vec2 worldToLocalSpace(glm::vec2 worldPos);

		virtual float calculateEnergy(PhysicsScene* scene);

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

		float m_moment;
		float m_invMoment;

		float m_drag;
		float m_angularDrag;

		float m_orientation;

		glm::vec2 m_localX;
		glm::vec2 m_localY;

		glm::vec2 m_pastX;
		glm::vec2 m_pastY;

		float m_angularVelocity;
		float m_totalTorque;

		bool m_static;
		// TODO settings for controlling physics (gravity on/off, dynamic or static)

		void seperateObjects(RigidBody* other, glm::vec2 displacement);

		// Calculates moment of inertia using correct formula for shape
		virtual void calculateMoment() = 0;

		void calculateAxes();
	};

}