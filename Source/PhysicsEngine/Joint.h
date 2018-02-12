#pragma once
#include "PhysicsObject.h"

namespace physics {
	// Base class for springs, any other joints
	class Joint : public PhysicsObject {
	public:
		Joint(glm::vec4 colour);
		//TODO put in all the stuff about adding ends once scene uses smart pointers
	};
}