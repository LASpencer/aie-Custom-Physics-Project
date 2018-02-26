#include "TitleScreen.h"

#include "Renderer2D.h"

#include "Application2D.h"

TitleScreen::TitleScreen()
{
}

void TitleScreen::update(float deltaTime, Application2D * app)
{
	// TODO maybe have a physics scene with some minor stuff happening in background?
}

void TitleScreen::draw(Application2D * app)
{
	// TODO list available demos

	aie::Renderer2D* renderer = app->getRenderer();

	renderer->setRenderColour(1, 1, 1, 1);

	renderer->drawText(app->getFont(), "1: Pool Game", 500, 400);
	renderer->drawText(app->getFont(), "2: Slug Game", 500, 360);
	renderer->drawText(app->getFont(), "3: Bouncing Balls", 500, 320);
	renderer->drawText(app->getFont(), "4: Rope Bridge", 500, 280);
	renderer->drawText(app->getFont(), "Press Esc to quit", 500, 216);
}

void TitleScreen::setup()
{
}
