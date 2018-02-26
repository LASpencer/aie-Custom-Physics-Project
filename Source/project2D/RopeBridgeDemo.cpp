#include "RopeBridgeDemo.h"

#include "Input.h"

#include "Sphere.h"
#include "Box.h"
#include "Plane.h"
#include "Spring.h"
#include "Rope.h"

#include "Application2d.h"

using namespace physics;

const float RopeBridgeDemo::k_stick_force_multiplier = 2.f;
const float RopeBridgeDemo::k_stick_max_force = 100.f;


RopeBridgeDemo::RopeBridgeDemo() : m_scene(new PhysicsScene()), m_cueActive(false)
{
	setup();
}

RopeBridgeDemo::~RopeBridgeDemo()
{
}

void RopeBridgeDemo::update(float deltaTime, Application2D * app)
{
	aie::Input* input = aie::Input::getInstance();
	// TODO R to reset
	if (input->wasKeyPressed(aie::INPUT_KEY_R))
	{
		m_cueActive = false;
		// Reset
		m_scene->clear();
		setup();
	}

	if (m_cueActive) {
		// If cue being dragged back
		if (input->wasMouseButtonReleased(aie::INPUT_MOUSE_BUTTON_LEFT)) {
			// On releasing mouse, shoot cue
			m_cueActive = false;	// cue just released
			shootCue(app);
		}
		else if (input->wasKeyPressed(aie::INPUT_KEY_SPACE)) {
			// Spacebar to cancel shot
			m_cueActive = false;
		}
	}
	else {
		if (input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_LEFT)) {
			// On clicking mouse, activate cue and remember position
			m_cueActive = true;
			m_cueContact = { input->getMouseX(), input->getMouseY() };
		}
	}

	m_scene->update(deltaTime);
}

void RopeBridgeDemo::draw(Application2D * app)
{
	aie::Renderer2D* renderer = app->getRenderer();
	if (m_cueActive) {
		renderer->setRenderColour(0.6f, 0.6f, 0.6f);
		aie::Input* input = aie::Input::getInstance();
		renderer->drawLine(m_cueContact.x, m_cueContact.y, input->getMouseX(), input->getMouseY(), 2);
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

	Sphere particle({ 0,0 }, 2, { 0,0 }, 0, 0.4f, 1, 0, 1.f, 0.1f, {1,1,1,1});

	Rope suspensionRope({ -60,20 }, &particle, 20, 6.f, 40.f, 5.f);

	// Attach rope ends to anchor
	SpringPtr anchorSpring1(new Spring(40.f, 4.f, 5.f));
	SpringPtr anchorSpring2(new Spring(40.f, 4.f, 5.f));
	anchorSpring1->setEnd1(anchor1);
	anchorSpring1->setEnd2(suspensionRope.getSegments()[0]);
	anchorSpring1->setAnchor2({ -2,0 });

	anchorSpring2->setEnd1(anchor2);
	anchorSpring2->setEnd2(suspensionRope.getSegments()[19]);
	anchorSpring2->setAnchor2({ 2,0 });

	m_scene->addActor(anchor1);
	m_scene->addActor(anchor2);
	m_scene->addActor(anchorSpring1);
	m_scene->addActor(anchorSpring2);
	suspensionRope.addToScene(m_scene);
}

void RopeBridgeDemo::shootCue(Application2D * app)
{
	aie::Input* input = aie::Input::getInstance();
	glm::vec2 mousePos = { input->getMouseX(), input->getMouseY() };

	glm::vec2 worldCueContact = app->screenToWorldSpace(m_cueContact);

	// Check cue contact against actors
	for (auto object : m_scene->getActors()) {
		if (object->isPointInside(worldCueContact)) {
			RigidBody* body = dynamic_cast<RigidBody*>(object.get());
			if (body != nullptr) {
				// Apply force to object
				glm::vec2 worldCueEnd = app->screenToWorldSpace(mousePos);
				glm::vec2 displacement = worldCueContact - worldCueEnd;
				glm::vec2 direction = glm::normalize(displacement);
				float length = glm::length(displacement);
				// Determine force to apply from cue
				float cueForce = std::min(length * k_stick_force_multiplier, k_stick_max_force);
				body->applyImpulse(direction * cueForce, worldCueContact);
			}
			break;
		}
	}
}
