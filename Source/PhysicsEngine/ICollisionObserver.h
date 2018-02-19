#pragma once
#include "ExternalLibraries.h"

namespace physics {

	class PhysicsObject;
	struct Collision;

	class ICollisionObserver;
	typedef std::shared_ptr<ICollisionObserver> CollisionObserverPtr;
	typedef std::weak_ptr<ICollisionObserver> CollisionObserverWeakPtr;

	class ICollisionObserver {
	public:
		virtual void OnCollision(PhysicsObject* publisher, const Collision& collision) = 0;
	};
}