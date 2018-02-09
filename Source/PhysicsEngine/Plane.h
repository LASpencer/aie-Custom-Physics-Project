#pragma once
#include "ExternalLibraries.h"
#include "PhysicsObject.h"

namespace physics {
	class Plane : public PhysicsObject {
	public:
		Plane(glm::vec2 normal, float distance, glm::vec4 colour = { 1,1,1,1, });


		virtual void earlyUpdate(float timeStep);
		virtual void fixedUpdate(glm::vec2 gravity, float timeStep);
		virtual void makeGizmo();
		virtual Collision checkCollision(PhysicsObject* other);
		virtual Collision checkSphereCollision(Sphere* other);
		virtual Collision checkPlaneCollision(Plane* other);

		glm::vec2 getNormal() { return m_normal; }
		void setNormal(glm::vec2 normal);

		float getDistance() { return m_distance; }
		void setDistance(float distance);

		inline float distanceToPoint(glm::vec2 point);

		virtual ShapeType getShapeID();

	protected:
		glm::vec2 m_normal;
		float m_distance;
	};
}