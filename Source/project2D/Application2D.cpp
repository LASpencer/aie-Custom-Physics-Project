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

#include "TitleScreen.h"
#include "PoolGame.h"
#include "BouncingBallsDemo.h"
#include "RopeBridgeDemo.h"

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

	m_titleScreen = new TitleScreen();
	m_pool = new PoolGame();
	m_ballDemo = new BouncingBallsDemo();
	m_ropeDemo = new RopeBridgeDemo();

	m_currentDemo = m_titleScreen;

	return true;
}

void Application2D::shutdown() {
	
	delete m_ropeDemo;
	delete m_ballDemo;
	delete m_pool;
	delete m_titleScreen;
	delete m_font;
	delete m_2dRenderer;
}

void Application2D::update(float deltaTime) {

	m_timer += deltaTime;

	// input example
	aie::Input* input = aie::Input::getInstance();

	// use arrow keys to move camera
	//if (input->isKeyDown(aie::INPUT_KEY_UP))
	//	m_cameraPos.y += 100.0f * deltaTime;

	//if (input->isKeyDown(aie::INPUT_KEY_DOWN))
	//	m_cameraPos.y -= 100.0f * deltaTime;

	//if (input->isKeyDown(aie::INPUT_KEY_LEFT))
	//	m_cameraPos.x -= 100.0f * deltaTime;

	//if (input->isKeyDown(aie::INPUT_KEY_RIGHT))
	//	m_cameraPos.x += 100.0f * deltaTime;

	// exit the application
	if (input->wasKeyPressed(aie::INPUT_KEY_ESCAPE))
	{
		if (m_currentDemo == m_titleScreen) {
			quit();
		}
		else {
			m_currentDemo = m_titleScreen;
		}
	}
	else if (input->wasKeyPressed(aie::INPUT_KEY_1)) {
		m_currentDemo = m_pool;
	}
	else if (input->wasKeyPressed(aie::INPUT_KEY_2)) {
		m_currentDemo = m_ballDemo;
	}
	else if (input->wasKeyPressed(aie::INPUT_KEY_3)) {
		m_currentDemo = m_ropeDemo;
	}

	aie::Gizmos::clear();

	//glm::vec2 mousePos = screenToWorldSpace({(float)input->getMouseX(), (float)input->getMouseY()});


	//aie::Gizmos::add2DCircle(mousePos, 1, 6, { 0.2f,.8f,.5f,1 });// Draw circle at mouse

	m_currentDemo->update(deltaTime, this);
}

void Application2D::draw() {

	// wipe the screen to the background colour
	clearScreen();


	// begin drawing sprites
	m_2dRenderer->begin();

	m_2dRenderer->setRenderColour(1,1,1,1);

	float sceneHeight = m_sceneExtent * getWindowHeight() / getWindowWidth();
	aie::Gizmos::draw2D(glm::ortho<float>(m_cameraPos.x - m_sceneExtent, m_cameraPos.x + m_sceneExtent,
						m_cameraPos.y - sceneHeight, m_cameraPos.y + sceneHeight, -1.0f, 1.0f));
	
	m_currentDemo->draw(this);
	
	// output some text in yellow
	m_2dRenderer->setRenderColour(1, 1, 0, 1);
	char fps[32];
	sprintf_s(fps, 32, "FPS: %i", getFPS());
	m_2dRenderer->drawText(m_font, fps, 0, 720 - 32);

	// done drawing sprites
	m_2dRenderer->end();
}

glm::vec2 Application2D::screenToWorldSpace(glm::vec2 pos)
{
	return (pos - (0.5f * glm::vec2((float)getWindowWidth(), (float)getWindowHeight()))) * screenToWorldScale() + m_cameraPos;
}

glm::vec2 Application2D::worldToScreenSpace(glm::vec2 pos)
{
	return worldToScreenScale() * (pos - m_cameraPos) + 0.5f * glm::vec2((float)getWindowWidth(), (float)getWindowHeight());
}

float Application2D::screenToWorldScale()
{
	return 2.f * m_sceneExtent / (float)getWindowWidth();
}

float Application2D::worldToScreenScale()
{
	return 0.5f * (float)getWindowWidth() / m_sceneExtent;
}
