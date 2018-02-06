#pragma once
#include "ExternalLibraries.h"

enum ShapeType {
	plane,
	sphere,
	box
};

class Sphere;
class Plane;

namespace physics {
	struct Collision {
		Collision(bool a_success, PhysicsObject* a_first, PhysicsObject* a_second, glm::vec2 a_normal = glm::vec2(0))
			: success(a_success), first(a_first), second(a_second), normal(a_normal)
		{};

		bool success;

		PhysicsObject* first;
		PhysicsObject* second;

		glm::vec2 normal;

		operator bool() { return success; }

		Collision reverse() {
			return Collision(success, second, first, -normal);
		}
	};

	class PhysicsObject {
		// TODO abstract base class for physics objects
		// TODO system for having different layers for collision detection (scene has map of layers to bitmask?), turn collisions on or off

	protected:
		PhysicsObject(glm::vec4 colour);

		glm::vec4 m_colour;

	public:
		virtual void fixedUpdate(glm::vec2 gravity, float timestep) = 0;
		virtual void makeGizmo() = 0;
		virtual glm::vec4 getColour() { return m_colour; }
		virtual void setColour(glm::vec4 colour) { m_colour = colour; }

		virtual Collision checkCollision(PhysicsObject* other) = 0;
		virtual Collision checkSphereCollision(Sphere* other) = 0;
		virtual Collision checkPlaneCollision(Plane* other) = 0;
		virtual void resolveCollision(Collision collision) = 0;

		virtual ShapeType getShapeID() = 0;
	};
}