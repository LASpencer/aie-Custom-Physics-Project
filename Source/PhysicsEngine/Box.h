#pragma once
#include "ExternalLibraries.h"
#include "RigidBody.h"


namespace physics {
	class Box;

	typedef std::shared_ptr<Box> BoxPtr;
	typedef std::weak_ptr<Box> BoxWeakPtr;

	class Box : public RigidBody {
	public:
		Box(glm::vec2 position, float width, float height, float orientation,
			glm::vec2 velocity = { 0,0 }, float angularVelocity = 0, float mass = 1.f,
			float elasticity = 1.f, glm::vec4 colour = { 1,1,1,1 });

		virtual void makeGizmo(float timeRatio);
		virtual Collision checkCollision(PhysicsObject* other);
		virtual Collision checkSphereCollision(Sphere* other);
		virtual Collision checkBoxCollision(Box* other);
		virtual Collision checkPlaneCollision(Plane* other);

		virtual ShapeType getShapeID();

		float getWidth();
		float setWidth(float width);

		float getHeight();
		float setHeight(float height);

		glm::vec2 getXExtent();
		glm::vec2 getYExtent();

	protected:
		float m_xExtent;
		float m_yExtent;

		virtual void calculateMoment();
	};
}