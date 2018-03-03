#include "BouncingBallsDemo.h"

#include "Input.h"

#include "Sphere.h"
#include "Box.h"
#include "Plane.h"
#include "Spring.h"
#include "SoftBody.h"

#include "Application2d.h"

using namespace physics;

const float BouncingBallsDemo::k_stick_force_multiplier = 5.f;
const float BouncingBallsDemo::k_stick_max_force = 200.f;

BouncingBallsDemo::BouncingBallsDemo() : m_scene(new PhysicsScene()), m_cueActive(false), m_gravityOn(true)
{
	setup();
}

BouncingBallsDemo::~BouncingBallsDemo()
{
	delete m_scene;
}

void BouncingBallsDemo::update(float deltaTime, Application2D * app)
{
	aie::Input* input = aie::Input::getInstance();

	//middle mouse to center camera
	if (input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_MIDDLE)) {
		app->setCameraPos({ 0,0 });
	}

	//  R to reset
	if (input->wasKeyPressed(aie::INPUT_KEY_R))
	{
		app->setCameraPos({ 0,0 });
		m_cueActive = false;
		m_gravityOn = true;
		m_scene->setGravity({ 0,-10 });
		// Reset
		m_scene->clear();
		setup();
	}

	// G turns gravity on and off
	if (input->wasKeyPressed(aie::INPUT_KEY_G))
	{
		if (m_gravityOn) {
			m_gravityOn = false;
			m_scene->setGravity({ 0,0 });
		}
		else {
			m_gravityOn = true;
			m_scene->setGravity({ 0,-10 });
		}
	}

	// cue to push things
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

void BouncingBallsDemo::draw(Application2D * app)
{
	aie::Renderer2D* renderer = app->getRenderer();
	if (m_cueActive) {
		renderer->setRenderColour(0.6f, 0.6f, 0.6f);
		aie::Input* input = aie::Input::getInstance();
		renderer->drawLine(m_cueContact.x, m_cueContact.y, input->getMouseX(), input->getMouseY(), 2);
	}
}

void BouncingBallsDemo::setup()
{
	m_scene->addActor(new physics::Plane({ -1,0 }, 90, 1, 1));
	m_scene->addActor(new physics::Plane({ 1,0 }, 90, 1, 1));
	m_scene->addActor(new physics::Plane({ -1,1 }, 40, 1, 1));
	m_scene->addActor(new physics::Plane({ 0,1 }, 40, 1, 1));
	m_scene->addActor(new physics::Plane({ 0,-1 }, 40, 1, 1));

	m_scene->addActor(new physics::Sphere({ 20,0 }, 3, { 0,-40 }, 0, 3.f, 1, 0.0f, 0, 0.1f, {1,0,0,1}));
	m_scene->addActor(new physics::Sphere({ -20,0 }, 3, { 10,0.5f }, 0, 3.f, 0.8f, 0.2f, 0, 0.1f, { 0,1,0,1 }));
	m_scene->addActor(new physics::Sphere({ 20,-20 }, 3, { 0,0 }, 0, 3.f, 1, 0.2f, 0, 0.1f, { 0,0,1,1 }));
	m_scene->addActor(new physics::Box({ 0,10 }, 8, 3, 0.5f, { 10,-3 }, 0, 2, 0.8f, 0.2f, 0, 0.1f, { 1,1,0,1 }));
	m_scene->addActor(new physics::Box({ 30,10 }, 5, 5, 0, { -2,-1 }, 5, 2, 0.8f, 0.2f, 0, 0.1f, {0,1,1,1}));

	// Spring connected to kinematic centre
	SpherePtr s1(new Sphere({ 10,0 }, 3, { 0,0 }, 0, INFINITY));
	m_scene->addActor(s1);
	SpherePtr s2(new Sphere({ 5,-3 }, 3, { 0,0 }, 0, 1));
	SpringPtr spring(new Spring(100.f, 20,3.f, s1, s2, { 0,-3 }, { 0,3 }));
	m_scene->addActor(s2);
	m_scene->addActor(spring);


	Box particle({ 0,0 }, 4, 4, 0, { 0,0 }, 0, 1, 1, 0.1f, 0, 0, { 1,0,0,1 });

	SoftBody body({ -10,20 }, &particle, 10, 3, 5.f, 30.f, 30.f, 20.f, 1.f, { 1,1,1,1 });
	body.addToScene(m_scene);
}

void BouncingBallsDemo::shootCue(Application2D * app)
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
