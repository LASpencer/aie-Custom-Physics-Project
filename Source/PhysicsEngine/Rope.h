#pragma once
#include "SoftBody.h"

namespace physics
{
	class Rope : public SoftBody {
	public:
		Rope();
		Rope(glm::vec2 position, RigidBody* particle, size_t segments, float distance, float strength, float damping);

		const std::vector<RigidBodyPtr>& getSegments() { return m_particles[0]; }
	};
}