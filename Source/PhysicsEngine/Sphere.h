#pragma once
#include "ExternalLibraries.h"
#include "RigidBody.h"

namespace physics {
	class Sphere;

	typedef std::shared_ptr<Sphere> SpherePtr;
	typedef std::weak_ptr<Sphere> SphereWeakPtr;

	class Sphere : public RigidBody {
	public:
		Sphere(glm::vec2 position, glm::vec2 velocity, float radius, float mass = 1.f, float elasticity = 1.f, glm::vec4 colour = { 1,1,1,1 });

		virtual void makeGizmo(float timeRatio);
		virtual Collision checkCollision(PhysicsObject* other);
		virtual Collision checkSphereCollision(Sphere* other);
		virtual Collision checkBoxCollision(Box* other);
		virtual Collision checkPlaneCollision(Plane* other);


		virtual ShapeType getShapeID();

		float getRadius() { return m_radius; }
		void setRadius(float radius);

	protected:
		float m_radius;

		virtual void calculateMoment();
	};
}