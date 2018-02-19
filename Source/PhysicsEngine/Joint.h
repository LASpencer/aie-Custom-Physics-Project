#pragma once
#include "PhysicsObject.h"

namespace physics {

	class Joint;
	class PhysicsScene;

	typedef std::shared_ptr<Joint> JointPtr;
	typedef std::weak_ptr<Joint> JointWeakPtr;

	class RigidBody;

	typedef std::shared_ptr<RigidBody> RigidBodyPtr;
	typedef std::weak_ptr<RigidBody> RigidBodyWeakPtr;

	// Base class for springs, any other joints
	class Joint : public PhysicsObject {
	public:
		Joint(RigidBodyPtr end1, RigidBodyPtr end2, glm::vec2 anchor1, glm::vec2 anchor2, glm::vec4 colour);
		
		Joint(const Joint& other);

		void fixedUpdate(PhysicsScene* scene);

		bool setEnd1(RigidBodyPtr end);
		bool setEnd2(RigidBodyPtr end);

		void setAnchor1(glm::vec2 anchorPoint);
		void setAnchor2(glm::vec2 anchorPoint);

		RigidBodyPtr getEnd1() { return m_end1; }
		RigidBodyPtr getEnd2() { return m_end2; }
		
		glm::vec2 getAnchor1() { return m_anchor1; }
		glm::vec2 getAnchor2() { return m_anchor2; }

		virtual Collision checkCollision(PhysicsObject* other);
		virtual Collision checkSphereCollision(Sphere* other);
		virtual Collision checkBoxCollision(Box* other);
		virtual Collision checkPlaneCollision(Plane* other);

		virtual void resolveCollision(PhysicsObject* other, const Collision& col);
		virtual void resolveRigidbodyCollision(RigidBody * other, const Collision& col);
		virtual void resolvePlaneCollision(Plane* other, const Collision& col);

		virtual glm::vec2 calculateMomentum() { return { 0,0 }; };

		virtual bool isStatic() { return true; };

	protected:
		RigidBodyPtr m_end1;
		RigidBodyPtr m_end2;

		glm::vec2 m_anchor1;
		glm::vec2 m_anchor2;

		void removeKilledEnd();
	};

}