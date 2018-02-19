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

	class PhysicsScene;
	class PhysicsObject;
	class RigidBody;
	class Sphere;
	class Box;
	class Plane;
	class ICollisionObserver;

	typedef std::shared_ptr<PhysicsObject> PhysicsObjectPtr;
	typedef std::weak_ptr<PhysicsObject> PhysicsObjectWeakPtr;

	typedef std::shared_ptr<ICollisionObserver> CollisionObserverPtr;
	typedef std::weak_ptr<ICollisionObserver> CollisionObserverWeakPtr;

	struct Collision {
		Collision(bool a_success = false, PhysicsObject* a_first = nullptr, PhysicsObject* a_second = nullptr, glm::vec2 a_normal = glm::vec2(0), glm::vec2 a_contact = glm::vec2(0), float a_depth = 0)
			: success(a_success), first(a_first), second(a_second), normal(a_normal), contact(a_contact), depth(a_depth)
		{};

		bool success;

		PhysicsObject* first;
		PhysicsObject* second;

		glm::vec2 normal;
		glm::vec2 contact;

		float depth;

		operator bool() const { return success; }

		Collision reverse() {
			return Collision(success, second, first, -normal, contact, depth);
		}
	};

	class PhysicsObject {
		// TODO abstract base class for physics objects
		// TODO system for having different layers for collision detection (scene has map of layers to bitmask?), turn collisions on or off

	protected:
		PhysicsObject(float elasticity, glm::vec4 colour);

		PhysicsObject(const PhysicsObject& other);

		glm::vec4 m_colour;

		float m_elasticity;
		bool m_alive;

		std::vector<CollisionObserverWeakPtr> m_observers;

	public:
		virtual PhysicsObject* clone() = 0;

		virtual void earlyUpdate(PhysicsScene* m_scene) = 0;	//
		virtual void fixedUpdate(PhysicsScene* m_scene) = 0;
		virtual void makeGizmo(float timeRatio) = 0;

		glm::vec4 getColour() { return m_colour; }
		void setColour(glm::vec4 colour) { m_colour = colour; }

		float getElasticity() { return m_elasticity; };
		void setElasticity(float elasticity);

		//TODO maybe collision data should be passed by reference instead of the return value
		virtual Collision checkCollision(PhysicsObject* other) = 0;
		virtual Collision checkSphereCollision(Sphere* other) = 0;
		virtual Collision checkBoxCollision(Box* other) = 0;
		virtual Collision checkPlaneCollision(Plane* other) = 0;

		virtual void resolveCollision(PhysicsObject* other, const Collision& col) = 0;
		virtual void resolveRigidbodyCollision(RigidBody * other, const Collision& col) = 0;
		virtual void resolvePlaneCollision(Plane* other, const Collision& col) = 0;

		virtual ShapeType getShapeID() = 0;

		virtual float calculateEnergy(PhysicsScene* m_scene) = 0;
		virtual glm::vec2 calculateMomentum() = 0;

		void broadcastCollision(const Collision& collision);

		static float combineElasticity(PhysicsObject* e1, PhysicsObject* e2);

		virtual bool isStatic() = 0;
		
		bool isAlive() { return m_alive; };

		void kill();

		bool addObserver(const CollisionObserverPtr& observer);
		bool removeObserver(const CollisionObserverPtr& observer);
		bool isSubscribed(const CollisionObserverPtr& observer);
		const std::vector<CollisionObserverWeakPtr>& getObservers() { return m_observers; }

	};
}