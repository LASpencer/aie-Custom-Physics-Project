#pragma once

namespace physics {

	class PhysicsObject;
	struct Collision;

	class ICollisionObserver {
	public:
		virtual void OnCollision(PhysicsObject* publisher, const Collision& collision) = 0;
	};
}