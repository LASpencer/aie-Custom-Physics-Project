#pragma once
#include "PhysicsObject.h"

namespace physics {

	class Joint;

	typedef std::shared_ptr<Joint> JointPtr;
	typedef std::weak_ptr<Joint> JointWeakPtr;

	class RigidBody;

	typedef std::shared_ptr<RigidBody> RigidBodyPtr;
	typedef std::weak_ptr<RigidBody> RigidBodyWeakPtr;

	// Base class for springs, any other joints
	class Joint : public PhysicsObject {
	public:
		Joint(RigidBodyPtr end1, RigidBodyPtr end2, glm::vec4 colour);
		//TODO have ends connected at contact points (when rotational forces to be done)
	
		void fixedUpdate(glm::vec2 gravity, float timestep);

		virtual bool setEnd1(RigidBodyPtr end);
		virtual bool setEnd2(RigidBodyPtr end);

		virtual RigidBodyPtr getEnd1() { return m_end1; }
		virtual RigidBodyPtr getEnd2() { return m_end2; }
		

		virtual Collision checkCollision(PhysicsObject* other);
		virtual Collision checkSphereCollision(Sphere* other);
		virtual Collision checkPlaneCollision(Plane* other);

		virtual void resolveCollision(PhysicsObject* other, const Collision& col);
		virtual void resolveRigidbodyCollision(RigidBody * other, const Collision& col);
		virtual void resolvePlaneCollision(Plane* other, const Collision& col);

		virtual glm::vec2 calculateMomentum() { return { 0,0 }; };

		virtual bool isStatic() { return true; };

	protected:
		RigidBodyPtr m_end1;
		RigidBodyPtr m_end2;

		void removeKilledEnd();
	};

}