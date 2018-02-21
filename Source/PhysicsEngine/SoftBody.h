#pragma once
#include "PhysicsScene.h"

namespace physics {
	class RigidBody;
	class Spring;

	typedef std::shared_ptr<RigidBody> RigidBodyPtr;
	typedef std::weak_ptr<RigidBody> RigidBodyWeakPtr;

	typedef std::shared_ptr<Spring> SpringPtr;
	typedef std::weak_ptr<Spring> SpringWeakPtr;

	class SoftBody {
	public:
		SoftBody();

		SoftBody(glm::vec2 position, RigidBody* particle, size_t cols, size_t rows, float distance, float strength, float shearStrength, float bendStrength, float damping, glm::vec4 springColour = { 1,1,1,1 });

		// Adds each object from body to scene
		void addToScene(PhysicsScene* scene);

		// Kills all objects making up soft body
		void kill();

		// Sets restoring strength of structure springs
		void setStrength(float strength);

		// Sets restoring strength of shear springs
		void setShearStrength(float strength);

		// sets restoring strength of bend strings
		void setBendStrength(float strength);

		// Sets damping on springs
		void setDamping(float damping);

		const std::vector<std::vector<RigidBodyPtr>>& getParticles() { return m_particles; }

	protected:
		std::vector<std::vector<RigidBodyPtr>> m_particles;
		std::vector<SpringPtr> m_structureSprings;
		std::vector<SpringPtr> m_shearSprings;
		std::vector<SpringPtr> m_bendSprings;
	};
}