#include "PoolGame.h"

#include "Input.h"

#include "Plane.h"
#include "Sphere.h"
#include "Box.h"

#include "Application2d.h"
#include "PoolBall.h"


using namespace physics;

const float PoolGame::k_table_width = 160;
const float PoolGame::k_table_height = 80;
const float PoolGame::k_rail_friction = 0.3f;
const float PoolGame::k_rail_elasticity = 1.f;
const float PoolGame::k_stick_force_multiplier = 2.f;
const float PoolGame::k_stick_max_force = 300.f;
const float PoolGame::k_pocket_width = 8.f;
const glm::vec4 PoolGame::k_felt_colour = { 0.f,1.f,0.f,1.f };

PoolGame::PoolGame() : m_balls(16,PoolBallPtr()), m_cueActive(false)
{
	m_scene = new PhysicsScene(0.01f, { 0,0 });
	setup();
}

void PoolGame::update(float deltaTime, Application2D* app)
{
	// TODO cuestick

	// TODO only allow play when balls have stopped

	aie::Input* input = aie::Input::getInstance();
	// Check if balls have stopped
	if (std::all_of(m_balls.begin(), m_balls.end(), [](PoolBallPtr b) { return b->isStopped(); })) {
		if (m_cueActive) {
			if (input->wasMouseButtonReleased(aie::INPUT_MOUSE_BUTTON_LEFT)) {
				m_cueActive = false;
				Sphere* cueball = m_balls[0]->getSphere();
				shootCue(cueball, app);
			}
			// Spacebar to cancel shot
			if (input->wasKeyPressed(aie::INPUT_KEY_SPACE)) {
				m_cueActive = false;
			}
		}
		else {
			if (input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_LEFT)) {
				m_cueActive = true;
				m_cueContact = { input->getMouseX(), input->getMouseY() };
			}
		}

		// HACK just replacing cueball at default spot for now
		if (m_balls[0]->isSunk()) {
			m_balls[0]->place({ 0.25f * k_table_width, 0 }, m_scene);
		}
	}

	m_scene->update(deltaTime);
}

void PoolGame::draw(Application2D * app)
{
	// TODO draw
	aie::Renderer2D* renderer = app->getRenderer();
	glm::vec2 centre = app->worldToScreenSpace({ 0,0 });
	float scale = app->worldToScreenScale();
	renderer->setRenderColour(k_felt_colour.r, k_felt_colour.g, k_felt_colour.b, k_felt_colour.a);
	//renderer->drawBox(centre.x, centre.y, scale * k_table_width, scale * k_table_height,0,2);

	// TODO URGENT figure out how to get felt to render behind gizmos

	// TODO extract colour, width out as constants
	if (m_cueActive) {
		renderer->setRenderColour(0.8f,0.8f,0.3f);
		aie::Input* input = aie::Input::getInstance();
		renderer->drawLine(m_cueContact.x, m_cueContact.y, input->getMouseX(), input->getMouseY(), 2);
	}
}

void PoolGame::setup()
{
	m_scene->addActor(new Plane({ 1,0 }, 0.5f * k_table_width, k_rail_elasticity, k_rail_friction, { 0,1,0,1 }));
	m_scene->addActor(new Plane({ -1,0 }, 0.5f * k_table_width, k_rail_elasticity, k_rail_friction, { 0,1,0,1 }));
	m_scene->addActor(new Plane({ 0,1 }, 0.5f * k_table_height, k_rail_elasticity, k_rail_friction, { 0,1,0,1 }));
	m_scene->addActor(new Plane({ 0,-1 }, 0.5f * k_table_height, k_rail_elasticity, k_rail_friction, { 0,1,0,1 }));
	
	// TODO maybe rails should be static boxes, with pockets behind them?
	// top/bottom rails have width 0.5 * table_width - 0.5 * (pocket_width + sqrt(2) * pocket_width )  
	// side rails have height table_height - sqrt(2) * pocket_width
	// Set position as back by half of rail_width
	// Also top/bottom pockets should be moved back, to just barely be behind edge

	// TODO adding pockets
	// TODO extract pocket dimensions out to constant
	// TODO pocket colour?
	Box pocket({ 0,0 }, k_pocket_width, k_pocket_width, 0);
	pocket.setStatic(true);
	pocket.setTrigger(true);
	pocket.setTags(EPoolTags::pocket);
	
	// Add each pocket
	pocket.setPosition({ 0, 0.5f * k_table_height });
	m_scene->addActor(pocket.clone());
	pocket.setPosition({ 0,-0.5f * k_table_height });
	m_scene->addActor(pocket.clone());
	pocket.setOrientation(glm::quarter_pi<float>());
	pocket.setPosition({ -0.5f * k_table_width, 0.5f * k_table_height });
	m_scene->addActor(pocket.clone());
	pocket.setPosition({ 0.5f * k_table_width, -0.5f * k_table_height });
	m_scene->addActor(pocket.clone());
	pocket.setOrientation(-glm::quarter_pi<float>());
	pocket.setPosition({ 0.5f * k_table_width, 0.5f * k_table_height });
	m_scene->addActor(pocket.clone());
	pocket.setPosition({ -0.5f * k_table_width, -0.5f * k_table_height });
	m_scene->addActor(pocket.clone());

	// create balls
	for (size_t i = 0; i < 16; ++i) {
		m_balls[i] = std::make_shared<PoolBall>(i, this);
		m_scene->addUpdater(std::static_pointer_cast<IFixedUpdater>(m_balls[i]));
		m_balls[i]->getSphere()->addObserver(std::static_pointer_cast<ICollisionObserver>(m_balls[i]));
	}
	// rack balls
	rack();
}

void PoolGame::rack()
{
	// TODO place each ball in starting position
	// TODO let cue be placed anywhere behind line?
	m_balls[0]->place({0.25f * k_table_width, 0}, m_scene);

	// First ball in triangle is at {-0.25f * k_table_width,0}
	// Next row is at 60 degrees from start, times 2*radius ( + epsilon?), next ball on row at {0,-2*radius + epsilon)
	const float sixtyDegrees = glm::third<float>() * glm::pi<float>();
	const float spacing = PoolBall::k_radius * 2.f + 0.001f;
	glm::vec2 diagonal = { -sinf(sixtyDegrees), cosf(sixtyDegrees) };
	glm::vec2 down = { 0, -1 };
	glm::vec2 footPosition = { -0.25f * k_table_width,0 };	// Point of triangle

	auto triangleSpot = [=](int row, int place) 
		{ return footPosition + row * spacing * diagonal + place * spacing * down; };

	m_balls[1]->place(triangleSpot(0, 0), m_scene);
	m_balls[2]->place(triangleSpot(1, 1), m_scene);
	m_balls[3]->place(triangleSpot(2, 0), m_scene);
	m_balls[4]->place(triangleSpot(3, 1), m_scene);
	m_balls[5]->place(triangleSpot(3, 3), m_scene);
	m_balls[6]->place(triangleSpot(4, 0), m_scene);
	m_balls[7]->place(triangleSpot(4, 3), m_scene);
	m_balls[8]->place(triangleSpot(2, 1), m_scene);
	m_balls[9]->place(triangleSpot(1, 0), m_scene);
	m_balls[10]->place(triangleSpot(2, 2), m_scene);
	m_balls[11]->place(triangleSpot(3, 0), m_scene);
	m_balls[12]->place(triangleSpot(3, 2), m_scene);
	m_balls[13]->place(triangleSpot(4, 1), m_scene);
	m_balls[14]->place(triangleSpot(4, 2), m_scene);
	m_balls[15]->place(triangleSpot(4, 4), m_scene);
}

void PoolGame::shootCue(physics::Sphere* cue, Application2D* app)
{
	aie::Input* input = aie::Input::getInstance();
	glm::vec2 mousePos = { input->getMouseX(), input->getMouseY() };

	glm::vec2 worldCueContact = app->screenToWorldSpace(m_cueContact);
	if (cue->isPointInside(worldCueContact)) {
		glm::vec2 worldCueEnd = app->screenToWorldSpace(mousePos);
		glm::vec2 displacement = worldCueContact - worldCueEnd;
		glm::vec2 direction = glm::normalize(displacement);
		float length = glm::length(displacement);
		float cueForce = std::max(length * k_stick_force_multiplier, k_stick_max_force);
		// TODO maybe raycast and move contact to edge of sphere?
		cue->applyImpulse(direction * cueForce, worldCueContact);
	}
}