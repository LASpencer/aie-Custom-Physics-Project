#pragma once
#include "ExternalLibraries.h"

namespace physics {

	enum ShapeType {
		spring,
		plane,
		sphere,
		aabox,
		obox
	};

	class PhysicsObject;
	class RigidBody;
	class Sphere;
	class Plane;

	typedef std::shared_ptr<PhysicsObject> PhysicsObjectPtr;
	typedef std::weak_ptr<PhysicsObject> PhysicsObjectWeakPtr;

	struct Collision {
		Collision(bool a_success = false, PhysicsObject* a_first = nullptr, PhysicsObject* a_second = nullptr, glm::vec2 a_normal = glm::vec2(0), float a_depth = 0)
			: success(a_success), first(a_first), second(a_second), normal(a_normal), depth(a_depth)
		{};

		bool success;

		PhysicsObject* first;
		PhysicsObject* second;

		glm::vec2 normal;

		float depth;

		operator bool() const { return success; }

		Collision reverse() {
			return Collision(success, second, first, -normal, depth);
		}
	};

	class PhysicsObject {
		// TODO abstract base class for physics objects
		// TODO system for having different layers for collision detection (scene has map of layers to bitmask?), turn collisions on or off

	protected:
		PhysicsObject(glm::vec4 colour);

		glm::vec4 m_colour;

		bool m_alive;

	public:
		virtual void earlyUpdate(float timestep) = 0;	//
		virtual void fixedUpdate(glm::vec2 gravity, float timestep) = 0;
		virtual void makeGizmo(float timeRatio) = 0;
		virtual glm::vec4 getColour() { return m_colour; }
		virtual void setColour(glm::vec4 colour) { m_colour = colour; }

		//TODO maybe collision data should be passed by reference instead of the return value
		virtual Collision checkCollision(PhysicsObject* other) = 0;
		virtual Collision checkSphereCollision(Sphere* other) = 0;
		virtual Collision checkPlaneCollision(Plane* other) = 0;

		virtual void resolveCollision(PhysicsObject* other, const Collision& col) = 0;
		virtual void resolveRigidbodyCollision(RigidBody * other, const Collision& col) = 0;
		virtual void resolvePlaneCollision(Plane* other, const Collision& col) = 0;

		virtual ShapeType getShapeID() = 0;

		virtual float calculateEnergy(glm::vec2 gravity) = 0;
		virtual glm::vec2 calculateMomentum() = 0;

		virtual bool isStatic() = 0;
		
		bool isAlive() { return m_alive; };

		void kill();
	};
}