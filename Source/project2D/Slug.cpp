#include "Slug.h"

#include "Input.h"

#include "Spring.h"

using namespace physics;

const float Slug::k_particle_distance = 2.f;
const float Slug::k_particle_radius = 1.f;
const float Slug::k_head_radius = 1.5f;
const float Slug::k_head_distance = 3.f;
const float Slug::k_body_mass = 0.2f;
const float Slug::k_head_mass = 0.5f;
const float Slug::k_damping = 0.1f;
const float Slug::k_high_tightness = 10.f;
const float Slug::k_low_tightness = 2.f;
const float Slug::k_bend_multiple = 0.2f;
const float Slug::k_shear_multiple = 0.5f;
const float Slug::k_elasticity = 0.8f;
const float Slug::k_friction = 0.1f;
const float Slug::k_head_drag = 0.5f;
const float Slug::k_body_drag = 0.2f;
const float Slug::k_up_force = 50.f;
const float Slug::k_down_force = 20.f;
const float Slug::k_side_force = 40.f;

const glm::vec4 Slug::k_body_colour = {0,0.3f,0.8f,1};
const glm::vec4 Slug::k_head_colour = {0.8f,0,0.8f,1};

Slug::Slug(glm::vec2 pos) : m_headSprings(), m_relaxed(false)
{
	Sphere particle({ 0,0 }, k_particle_radius, { 0,0 },0,k_body_mass,k_elasticity,k_friction,k_body_drag,0.f,k_body_colour);
	m_body = SoftBody(pos, &particle, k_body_cols, k_body_rows, k_particle_distance, k_high_tightness, k_high_tightness * k_shear_multiple, k_high_tightness * k_bend_multiple, k_damping);
	glm::vec2 headPos = { k_particle_distance * (k_body_cols - 1) + k_head_distance,(k_particle_distance * 0.5f * k_body_rows) - k_particle_radius};
	m_head = SpherePtr(new Sphere(headPos + pos, k_head_radius, { 0,0 }, 0, k_head_mass, k_elasticity, k_friction, k_head_drag, 0.f, k_head_colour));

	// TODO attach head to body
	for (auto bodyPart : m_body.getParticles()[k_body_cols - 1]) {
		glm::vec2 displacement = bodyPart->getPosition() - m_head->getPosition();
		glm::vec2 direction = glm::normalize(displacement);
		float distance = glm::length(displacement);
		//TODO
		SpringPtr spring(new Spring(k_high_tightness, std::max(0.f,distance - k_head_radius - k_particle_radius), k_damping, m_head, bodyPart,direction * k_head_radius, -direction * k_particle_radius));
		m_headSprings.push_back(spring);
	}
}

void Slug::addToScene(physics::PhysicsScene * scene)
{
	m_body.addToScene(scene);
	scene->addActor(m_head);
	for (auto s : m_headSprings) {
		scene->addActor(s);
	}
}

void Slug::update(float deltaTime)
{
	aie::Input* input = aie::Input::getInstance();
	// Relax or tense based on press
	// TODO maybe have it slowly increase or decrease?
	if (input->isKeyDown(aie::INPUT_KEY_LEFT_SHIFT)) {
		if (!m_relaxed) {
			m_relaxed = true;
			m_body.setStrength(k_low_tightness);
			m_body.setShearStrength(k_low_tightness * k_shear_multiple);
			m_body.setBendStrength(k_low_tightness * k_bend_multiple);
			// TODO maybe change head string tightness?
			/*for (auto s : m_headSprings) {
				s->setTightness(k_low_tightness);
			}*/
		}
	}
	else {
		if (m_relaxed) {
			m_relaxed = false;
			m_body.setStrength(k_high_tightness);
			m_body.setShearStrength(k_high_tightness * k_shear_multiple);
			m_body.setBendStrength(k_high_tightness * k_bend_multiple);
			// TODO maybe change head string tightness?
			/*for (auto s : m_headSprings) {
				s->setTightness(k_high_tightness);
			}*/
		}
	}
}

void Slug::fixedUpdate(physics::PhysicsScene * scene)
{
	// TODO apply force on head from movement

	aie::Input* input = aie::Input::getInstance();
	
	if (input->isKeyDown(aie::INPUT_KEY_W)) {
		m_head->applyForce({ 0,k_up_force });
	}
	else if (input->isKeyDown(aie::INPUT_KEY_S)) {
		m_head->applyForce({ 0,-k_down_force });
	}
	if (input->isKeyDown(aie::INPUT_KEY_D)) {
		m_head->applyForce({k_side_force, 0});
	} else if (input->isKeyDown(aie::INPUT_KEY_A)) {
		m_head->applyForce({ -k_side_force, 0 });
	}

}
