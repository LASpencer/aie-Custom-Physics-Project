#pragma once
#include "ExternalLibraries.h"
#include "RigidBody.h"

namespace physics
{
	class CompositeBody;

	class CompositeBody : public RigidBody {

		// TODO set bodies

		//TODO collision: test against each, work out total by combining

		// TODO parallel axis theorem to calculate moment

		// TODO transforming bodies to world space

		// TODO bodies drawn with a transform applied

	protected:
		std::vector<RigidBody> m_bodies;
	};
}
