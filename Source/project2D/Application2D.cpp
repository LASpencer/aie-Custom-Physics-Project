#include "Application2D.h"
#include "Texture.h"
#include "Font.h"
#include "Input.h"
#include "Gizmos.h"

#include "Sphere.h"
#include "Plane.h"
#include "Spring.h"
#include "Box.h"
#include "SoftBody.h"
#include "Rope.h"

#include "PoolGame.h"

using namespace physics;

Application2D::Application2D() {

}

Application2D::~Application2D() {

}

bool Application2D::startup() {
	
	m_2dRenderer = new aie::Renderer2D();
	m_2dRenderer->setCameraPos(0,0);

	aie::Gizmos::create(255u, 255u, 65535u, 65535u);

	m_font = new aie::Font("./font/consolas.ttf", 32);
	
	m_cameraPos = { 0,0 };
	m_sceneExtent = 100;
	m_timer = 0;

	m_currentDemo = new PoolGame();

	return true;
}

void Application2D::shutdown() {
	
	delete m_font;
	delete m_2dRenderer;
}

void Application2D::update(float deltaTime) {

	m_timer += deltaTime;

	// input example
	aie::Input* input = aie::Input::getInstance();

	// use arrow keys to move camera
	if (input->isKeyDown(aie::INPUT_KEY_UP))
		m_cameraPos.y += 100.0f * deltaTime;

	if (input->isKeyDown(aie::INPUT_KEY_DOWN))
		m_cameraPos.y -= 100.0f * deltaTime;

	if (input->isKeyDown(aie::INPUT_KEY_LEFT))
		m_cameraPos.x -= 100.0f * deltaTime;

	if (input->isKeyDown(aie::INPUT_KEY_RIGHT))
		m_cameraPos.x += 100.0f * deltaTime;

	// exit the application
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();

	aie::Gizmos::clear();

	glm::vec2 mousePos = screenToWorldSpace({(float)input->getMouseX(), (float)input->getMouseY()});


	aie::Gizmos::add2DCircle(mousePos, 1, 6, { 0.2f,.8f,.5f,1 });// Draw circle at mouse

	m_currentDemo->update(deltaTime, this);
}

void Application2D::draw() {

	// wipe the screen to the background colour
	clearScreen();


	// begin drawing sprites
	m_2dRenderer->begin();

	// TODO extract stuff out into variables, make screen to world conversion functions
	float sceneHeight = m_sceneExtent * getWindowHeight() / getWindowWidth();
	aie::Gizmos::draw2D(glm::ortho<float>(m_cameraPos.x - m_sceneExtent, m_cameraPos.x + m_sceneExtent,
						m_cameraPos.y - sceneHeight, m_cameraPos.y + sceneHeight, -1.0f, 1.0f));
	
	m_currentDemo->draw(this);
	
	// output some text in yellow
	m_2dRenderer->setRenderColour(1, 1, 0, 1);
	char fps[32];
	sprintf_s(fps, 32, "FPS: %i", getFPS());
	m_2dRenderer->drawText(m_font, fps, 0, 720 - 32);
	m_2dRenderer->drawText(m_font, "Press ESC to quit!", 0, 720 - 96);

	// done drawing sprites
	m_2dRenderer->end();
}

glm::vec2 Application2D::screenToWorldSpace(glm::vec2 pos)
{
	float scale =2.f * m_sceneExtent / (float)getWindowWidth();
	return (pos - (0.5f * glm::vec2((float)getWindowWidth(), (float)getWindowHeight()))) * scale + m_cameraPos;
}

glm::vec2 Application2D::worldToSceenSpace(glm::vec2 pos)
{
	float scale = 0.5f * (float)getWindowWidth() / m_sceneExtent;
	return scale * (pos - m_cameraPos) + 0.5f * glm::vec2((float)getWindowWidth(), (float)getWindowHeight());
}
