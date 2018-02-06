#pragma once
#include "ExternalLibraries.h"

enum ShapeType {
	plane,
	sphere,
	box
};

class Sphere;

namespace physics {
	struct Collision {
		bool success;

		PhysicsObject* first;
		PhysicsObject* second;

		glm::vec2 normal;

		operator bool() { return success; }

		Collision reverse() {
			return { success, second, first, -normal };
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
		virtual void resolveCollision(Collision collision) = 0;

		virtual ShapeType getShapeID() = 0;
	};
}