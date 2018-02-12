#pragma once
#include "ExternalLibraries.h"
#include "PhysicsObject.h"

namespace physics {
	class Plane;

	typedef std::shared_ptr<Plane> PlanePtr;
	typedef std::weak_ptr<Plane> PlaneWeakPtr;

	class Plane : public PhysicsObject {
	public:
		Plane(glm::vec2 normal, float distance, float elasticity = 1.f, glm::vec4 colour = { 1,1,1,1, });


		virtual void earlyUpdate(float timeStep);
		virtual void fixedUpdate(glm::vec2 gravity, float timeStep);
		virtual void makeGizmo(float timeRatio);

		virtual Collision checkCollision(PhysicsObject* other);
		virtual Collision checkSphereCollision(Sphere* other);
		virtual Collision checkPlaneCollision(Plane* other);


		virtual void resolveCollision(PhysicsObject* other, const Collision & col) override;
		virtual void resolveRigidbodyCollision(RigidBody * other, const Collision & col) override;
		virtual void resolvePlaneCollision(Plane* other, const Collision & col) override;

		glm::vec2 getNormal() { return m_normal; }
		void setNormal(glm::vec2 normal);

		float getDistance() { return m_distance; }
		void setDistance(float distance);

		inline float distanceToPoint(glm::vec2 point);

		virtual ShapeType getShapeID();

		virtual float calculateEnergy(glm::vec2 gravity) { return 0; };
		virtual glm::vec2 calculateMomentum() { return { 0,0 }; };

		virtual bool isStatic() { return true; };

	protected:
		glm::vec2 m_normal;
		float m_distance;
	};
}