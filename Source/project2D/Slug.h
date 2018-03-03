#pragma once
#include "SoftBody.h"
#include "Sphere.h"
#include "Box.h"
#include "IFixedUpdater.h"
#include "ICollisionObserver.h"

class Slug : public physics::IFixedUpdater, public physics::ICollisionObserver {
public:
	static const size_t k_body_cols = 8;
	static const size_t k_body_rows = 4;
	static const float k_particle_distance;
	static const float k_particle_radius;
	static const float k_head_radius;
	static const float k_head_distance;
	static const float k_body_mass;
	static const float k_head_mass;
	static const float k_damping;
	static const float k_high_tightness;
	static const float k_low_tightness;
	static const float k_bend_multiple;
	static const float k_shear_multiple;
	static const float k_elasticity;
	static const float k_friction;
	static const float k_head_drag;
	static const float k_body_drag;
	static const float k_up_force;
	static const float k_down_force;
	static const float k_side_force;

	static const glm::vec4 k_body_colour;
	static const glm::vec4 k_head_colour;
	static const glm::vec4 k_eye_colour;
	static const glm::vec4 k_pupil_colour;

	Slug(glm::vec2 pos);

	physics::SoftBody& getBody() { return m_body; }
	physics::SpherePtr getHead() { return m_head; }

	bool hasWon() { return m_won; }

	void addToScene(physics::PhysicsScene* scene);
	virtual void update(float deltaTime);

	virtual void fixedUpdate(physics::PhysicsScene* scene);

	virtual void OnCollision(physics::PhysicsObject * publisher, const physics::Collision & collision) override;

	void drawEyes();

private:
	physics::SoftBody m_body;
	physics::SpherePtr m_head;
	std::vector<physics::SpringPtr> m_headSprings;
	bool m_relaxed;
	bool m_won;
};