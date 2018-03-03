#pragma once
#include "ExternalLibraries.h"

namespace physics {
	class PhysicsScene;
	class IFixedUpdater;
	typedef std::shared_ptr<IFixedUpdater> FixedUpdaterPtr;
	typedef std::weak_ptr<IFixedUpdater> FixedUpdaterWeakPtr;
	// Interface for classes to do something each fixed update in
	// a physics scene
	class IFixedUpdater {
	public:
		virtual void fixedUpdate(PhysicsScene* scene) = 0;
	};
}