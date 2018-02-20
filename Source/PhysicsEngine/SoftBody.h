#pragma once
#include "PhysicsScene.h"

namespace physics {
	class SoftBody {
	public:
		SoftBody();

		SoftBody(glm::vec2 position, RigidBody* particle, size_t cols, size_t rows, float distance, float strength, float shearStrength, float bendStrength, float damping, glm::vec4 springColour = { 1,1,1,1 });

		void addToScene(PhysicsScene* scene);

		void kill();

	protected:
		std::vector<std::vector<RigidBodyPtr>> m_particles;
		std::vector<SpringPtr> m_structureSprings;
		std::vector<SpringPtr> m_shearSprings;
		std::vector<SpringPtr> m_bendSprings;
	};
}