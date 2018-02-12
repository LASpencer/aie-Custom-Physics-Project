#pragma once
#include "Joint.h"

namespace physics {
	class Spring;

	typedef std::shared_ptr<Spring> SpringPtr;
	typedef std::weak_ptr<Spring> SpringWeakPtr;

	class Spring : public Joint {
	public:
		Spring(float tightness, float length, float damping, RigidBodyPtr end1 = RigidBodyPtr(), RigidBodyPtr end2 = RigidBodyPtr(), glm::vec4 colour = { 1,1,1,1 });

		float getTightness() {return m_tightness;};
		void setTightness(float tightness);

		float getLength() { return m_length; };
		void setLength(float length);

		float getDamping() { return m_damping; };
		void setDamping(float damping);

		virtual void earlyUpdate(float timestep);

		virtual void makeGizmo(float timeRatio);

		virtual ShapeType getShapeID() { return ShapeType::spring; };

		virtual float calculateEnergy(glm::vec2 gravity);

	protected:
		float m_tightness;
		float m_length;
		float m_damping;
	};
}