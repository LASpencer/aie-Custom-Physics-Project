#pragma once
#include "ExternalLibraries.h"
#include "RigidBody.h"


namespace physics {
	class Box;
	class PhysicsScene;

	typedef std::shared_ptr<Box> BoxPtr;
	typedef std::weak_ptr<Box> BoxWeakPtr;

	struct Edge {
		glm::vec2 start;
		glm::vec2 end;
		glm::vec2 direction;
		glm::vec2 getNormal() const { return {direction.y, -direction.x}; };
		float getLength() const { return glm::dot(direction, end - start); }

		void clip(glm::vec2 normal, float offset);

		bool checkIntersection(const Edge& other, glm::vec2& intersection) const;
	};

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
		void setWidth(float width);

		float getHeight();
		void setHeight(float height);

		glm::vec2 getXExtent();
		glm::vec2 getYExtent();

		std::array<glm::vec2, 4> getCorners();

		Edge findBestEdge(glm::vec2 normal);

	protected:
		float m_xExtent;
		float m_yExtent;

		virtual void calculateMoment();
	};


}