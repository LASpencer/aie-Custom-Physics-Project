#pragma once
#include "ExternalLibraries.h"


namespace physics {

	enum ShapeType {
		spring,
		plane,
		sphere,
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

		// Whether the collision occurred
		bool success;

		PhysicsObject* first;
		PhysicsObject* second;

		// Collision normal pointed at first object
		glm::vec2 normal;

		// Contact point of collision
		glm::vec2 contact;

		// Depth by which objects interpenetrate
		float depth;

		operator bool() const { return success; }

		// Returns collision with first and second objects swapped
		Collision reverse() const {
			return Collision(success, second, first, -normal, contact, depth);
		}
	};

	// Abstract base 
	class PhysicsObject {
		
	protected:
		PhysicsObject(float elasticity, float friction, glm::vec4 colour);

		PhysicsObject(const PhysicsObject& other);

		glm::vec4 m_colour;	// Colour to draw object

		float m_elasticity;	// Coefficient of elasticity
		float m_friction;	// Coefficient of friction
		unsigned int m_tags;	// Bitmask for use by observers
		bool m_alive;		// True until object set as dead
		bool m_trigger;		// If true, no physical effect from collision
		bool m_draw;		// If true, gizmo will be created for rendering

		std::vector<CollisionObserverWeakPtr> m_observers;

	public:
		virtual PhysicsObject* clone() = 0;

		virtual void earlyUpdate(PhysicsScene* m_scene) = 0;
		virtual void fixedUpdate(PhysicsScene* m_scene) = 0;

		// Draw object with gizmos
		virtual void makeGizmo(float timeRatio) = 0;

		glm::vec4 getColour() { return m_colour; }
		void setColour(glm::vec4 colour) { m_colour = colour; }

		float getElasticity() { return m_elasticity; };
		void setElasticity(float elasticity);

		float getFriction() { return m_friction; }
		void setFriction(float friction);

		virtual bool isPointInside(glm::vec2 point) = 0;

		// test collision against other object
		// returns struct describing collision
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

		// Informs all observers that object was collided with
		// collision = collision to send to observers
		void broadcastCollision(const Collision& collision);

		// Calculates the combined elasticity for a pair of objects
		// e1,e2 = colliding objects
		static float combineElasticity(PhysicsObject* e1, PhysicsObject* e2);

		// Calculates the coefficient of friction for a pair of objects
		// e1,e2 = colliding objects
		static float combineFriction(PhysicsObject* e1, PhysicsObject* e2);

		// Returns true if the object is set as static
		virtual bool isStatic() = 0;
		
		// Returns true if the object has not been killed
		bool isAlive() { return m_alive; };

		bool isTrigger() { return m_trigger; }
		void setTrigger(bool value) { m_trigger = value; }

		bool shouldDraw() { return m_draw; }
		void setDraw(bool value) { m_draw = value; }

		// Call to set object as dead, so scene and other objects
		// referring to it can remove it
		void kill();

		// Call to set object as alive again, allowing it to be added back to the scene
		virtual void resetAlive();

		// Adds new observer to inform about collisions
		// returns true if added, false if already observing
		bool addObserver(const CollisionObserverPtr& observer);
		// Removes observer from list
		bool removeObserver(const CollisionObserverPtr& observer);

		// Returns true if observer is in list of subscribers
		bool isSubscribed(const CollisionObserverPtr& observer);
		const std::vector<CollisionObserverWeakPtr>& getObservers() { return m_observers; }

		unsigned int getTags() { return m_tags; }

		void setTags(unsigned int tags);

		// Sets passed bits to true in object's tags
		void addTags(unsigned int tags);

		// Sets passed bits to false in object's tags
		void removeTags(unsigned int tags);

		// Returns true if all bits passed are set to true in object's tags
		bool hasTags(unsigned int tags);
	};
}