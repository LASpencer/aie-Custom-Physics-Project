#include "RopeBridgeDemo.h"

#include "Input.h"

#include "Sphere.h"
#include "Box.h"
#include "Plane.h"
#include "Spring.h"

#include "Application2d.h"

using namespace physics;

const float RopeBridgeDemo::k_pull_force_multiplier = 2.f;
const float RopeBridgeDemo::k_max_pull_force = 200.f;
const float RopeBridgeDemo::k_def_tightness = 10.f;
const float RopeBridgeDemo::k_min_tightness = 1.f;
const float RopeBridgeDemo::k_max_tightness = 200.f;

RopeBridgeDemo::RopeBridgeDemo() : m_scene(new PhysicsScene()), m_selectedObject()
{
	setup();
}

RopeBridgeDemo::~RopeBridgeDemo()
{
}

void RopeBridgeDemo::update(float deltaTime, Application2D * app)
{
	aie::Input* input = aie::Input::getInstance();

	//middle mouse to center camera
	if (input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_MIDDLE)) {
		app->setCameraPos({ 0,0 });
	}

	// TODO R to reset
	if (input->wasKeyPressed(aie::INPUT_KEY_R))
	{
		m_selectedObject.reset();
		// Reset
		m_scene->clear();
		setup();
	}

	// TODO drag object with mouse
	if (input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_LEFT)) {
		m_selectedObject.reset();

		// Try selecting object
		glm::vec2 mousePos = { input->getMouseX(), input->getMouseY() };

		glm::vec2 worldMousePos = app->screenToWorldSpace(mousePos);

		for (auto object : m_scene->getActors()) {
			if (object->isPointInside(worldMousePos)) {
				RigidBodyPtr body = std::dynamic_pointer_cast<RigidBody>(object);
				if (body && body->isDynamic()) {
					// Select clicked body
					m_selectedObject = body;
				}
				break;
			}
		}

	}
	if (input->wasMouseButtonReleased(aie::INPUT_MOUSE_BUTTON_LEFT)) {
		m_selectedObject.reset();
	}
	if (input->isMouseButtonDown(aie::INPUT_MOUSE_BUTTON_LEFT)) {
		// Drag selected object
		if (m_selectedObject) {
			glm::vec2 mousePos = { input->getMouseX(), input->getMouseY() };

			glm::vec2 worldMousePos = app->screenToWorldSpace(mousePos);

			glm::vec2 displacement = worldMousePos - m_selectedObject->getPosition();
			glm::vec2 direction = glm::normalize(displacement);
			float length = glm::length(displacement);
			float forceMagnitude = std::min(length * k_pull_force_multiplier, k_max_pull_force);
			m_selectedObject->applyForce(direction * forceMagnitude);
		}
	}

	// Increase/decrease tightness of rope
	if (input->isKeyDown(aie::INPUT_KEY_W)) {
		// TODO extract out max/min tightness as constant
		// TODO damping also relative to tightness
		m_ropeTightness = std::min(200.f, m_ropeTightness + deltaTime * 10.f);
		m_suspensionRope.setStrength(m_ropeTightness);
		m_anchorSpring1->setTightness(m_ropeTightness);
		m_anchorSpring2->setTightness(m_ropeTightness);
	}
	else if (input->isKeyDown(aie::INPUT_KEY_S)) {
		m_ropeTightness = std::max(1.f, m_ropeTightness - deltaTime * 10.f);
		m_suspensionRope.setStrength(m_ropeTightness);
		m_anchorSpring1->setTightness(m_ropeTightness);
		m_anchorSpring2->setTightness(m_ropeTightness);
	}

	m_scene->update(deltaTime);
}

void RopeBridgeDemo::draw(Application2D * app)
{
	aie::Input* input = aie::Input::getInstance();
	aie::Renderer2D* renderer = app->getRenderer();
	if (m_selectedObject) {
		glm::vec2 selectedPos = app->worldToScreenSpace(m_selectedObject->getPosition());
		renderer->setRenderColour(0.8f, 0.8f, 0.8f);
		renderer->drawLine(selectedPos.x, selectedPos.y, input->getMouseX(), input->getMouseY(), 1);
	}
}

void RopeBridgeDemo::setup()
{
	// Static anchors hold up bridge
	SpherePtr anchor1(new Sphere({ -80, 20 }, 2, { 0,0 }));
	anchor1->setStatic(true);
	anchor1->setColour({ 0.7f, 0.7f,0.7f,1 });
	SpherePtr anchor2((Sphere*)anchor1->clone());
	anchor2->setPosition({ 80,20 });

	Sphere particle({ 0,0 }, 2, { 0,0 }, 0, 0.1f, 1, 0, 0.f, 0.1f, {1,1,1,1});

	m_ropeTightness = k_def_tightness;

	m_suspensionRope = Rope({ -74,20 }, &particle, 20, 6.f, m_ropeTightness, 1.f);

	// Attach rope ends to anchor
	m_anchorSpring1 = SpringPtr(new Spring(m_ropeTightness, 4.f,1.f));
	m_anchorSpring2 = SpringPtr(new Spring(m_ropeTightness, 4.f,1.f));
	m_anchorSpring1->setEnd1(anchor1);
	m_anchorSpring1->setEnd2(m_suspensionRope.getSegments()[0]);
	m_anchorSpring1->setAnchor2({ -2,0 });

	m_anchorSpring2->setEnd1(anchor2);
	m_anchorSpring2->setEnd2(m_suspensionRope.getSegments()[19]);
	m_anchorSpring2->setAnchor2({ 2,0 });

	m_scene->addActor(anchor1);
	m_scene->addActor(anchor2);
	m_scene->addActor(m_anchorSpring1);
	m_scene->addActor(m_anchorSpring2);
	m_suspensionRope.addToScene(m_scene);
}
