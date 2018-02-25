#include "PoolGame.h"

#include <iostream>

#include "Input.h"

#include "Plane.h"
#include "Sphere.h"
#include "Box.h"

#include "Application2d.h"
#include "PoolBall.h"
#include "PoolPlayer.h"


using namespace physics;

const float PoolGame::k_table_width = 160;
const float PoolGame::k_table_height = 80;
const float PoolGame::k_rail_friction = 0.3f;
const float PoolGame::k_rail_elasticity = 1.f;
const float PoolGame::k_rail_width = 8.f;
const float PoolGame::k_stick_force_multiplier = 2.f;
const float PoolGame::k_stick_max_force = 300.f;
const float PoolGame::k_pocket_width = 8.f;
const glm::vec4 PoolGame::k_felt_colour = { 0.f,1.f,0.f,1.f };
const glm::vec4 PoolGame::k_rail_colour = { 0.f,1.f,0.f,1.f };

PoolGame::PoolGame() : m_balls(16, PoolBallPtr()), m_pockets(), m_cueActive(false), m_playerIndex(0), m_sunkThisRound(), m_message()
{
	m_player[0] = PoolPlayer();
	m_player[1] = PoolPlayer();
	m_scene = new PhysicsScene(0.01f, { 0,0 });
	setup();
}

void PoolGame::update(float deltaTime, Application2D* app)
{
	// TODO cuestick

	// TODO only allow play when balls have stopped

	aie::Input* input = aie::Input::getInstance();
	// Check if balls have stopped

	switch (m_state) {
	case shot:
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
		break;
	case place_cue:
		// TODO draw cueball at mouse position
		glm::vec2 mouseWorldPos = app->screenToWorldSpace({ input->getMouseX(), input->getMouseY() });
		aie::Gizmos::add2DCircle(mouseWorldPos, PoolBall::k_radius, 20, PoolBall::k_cue_colour);
		// TODO on click, check if legal location and if so place cue
		if (input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_LEFT)) {
			if (isLegalCuePosition(mouseWorldPos)) {
				m_balls[0]->place(mouseWorldPos, m_scene);
				m_state = shot;
			}
		}
		break;
	case moving:
		// TODO check for end of state

		if (std::all_of(m_balls.begin(), m_balls.end(), [](PoolBallPtr b) { return b->isStopped(); })) {
			bool legalSink = true;					// Will a sunk ball be treated as legal?
			bool cueHit = m_firstHit != none;		// If firstHit set, cue must have hit something
			bool endTurn = true;					// Should the turn pass to next player?
			bool penalize = false;					// Should next player get a penalty shot?
			bool win = false;						// Did player just win?
			EPoolGameStates nextState = shot;
			EBallSuits playerSuit = currentPlayer().getSuit();
			m_message = "";
			if (m_break) {
				if (m_balls[8]->isSunk()) {
					// Sinking black off break restarts game with new first player
					rack();
					legalSink = false;
					m_message = "Eight-ball sunk, restarting game.";
				}
				else if (!cueHit) {
					// Failure to break is a penalty
					m_balls[0]->place({ 0.25f * k_table_width, 0 }, m_scene);
					penalize = true;
					legalSink = false;
					m_message = "Foul break.";
				}
				else if (m_balls[0]->isSunk()) {
					// Sinking cue off fair break is a foul, but not penalized
					nextState = place_cue;
					currentPlayer().resetPenalty();		// Lose any penalty shot from the foul
					legalSink = false;
					m_break = false;
					m_message = "Cue sunk off break.";
				}
				else {
					// Fair break
					m_break = false;
				}
			}
			else {
				if (m_balls[8]->isSunk()) {
					// Black sunk, so either win or lose
					if (playerSuit == none || AreAnySuitLeft(playerSuit)) {
						// If player hasn't cleared suit, they lose
						// HACK show on screen instead
						legalSink = false;
						m_message = "Eight-ball illegally sunk.";
						nextState = game_over;
					}
					else {
						// If only black sunk, or has penalty and only off-suit and black sunk, win, else lose
						bool blackReached = false;
						bool onSuitHit = false;				// Black sunk in combination with on-suit
						bool penaltyRuleApplies = false;	// Off-suit sunk, but penalty means legal unless on-suit sunk
						if (m_balls[0]->isSunk()) {
							// Lose if cue sunk
							legalSink = false;
							m_message = "Cue sunk with eight-ball.";
							nextState = game_over;
						}
						else {
							while (!m_sunkThisRound.empty()) {
								PoolBall* sunkBall = m_sunkThisRound.front();
								m_sunkThisRound.pop();
								if(sunkBall->getSuit() == eight){
									// Black sunk, so no on-suit can be sunk after
									blackReached = true;
								}
								else if (sunkBall->getSuit() == playerSuit) {
									if (blackReached) {
										// On suit ball sunk after black
										legalSink = false;
										m_message = "Eight-ball illegally sunk before others.";
										nextState = game_over;
										break;
									}
									else if (penaltyRuleApplies) {
										// On suit and off suit both sunk with black
										legalSink = false;
										m_message = "Eight-ball illegally sunk with off-colour balls.";
										nextState = game_over;
										break;
									}
									else {
										onSuitHit = true;
									}
								}
								else {
									if (currentPlayer().onFirstPenaltyShot() && !onSuitHit) {
										// Can sink off-suit on penalty shot if none of own on table
										penaltyRuleApplies = true;
									}
									else {
										// If any on suit were on table, or no penalty, illegal to sink off suit and black
										legalSink = false;
										m_message = "Eight-ball illegally sunk with off-colour balls.";
										nextState = game_over;
										break;
									}
								}
							}
							if (legalSink) {
								// All sunk balls resolved without losing, so must win
								win = true;
								m_message = "Eight-ball sunk!";
								nextState = game_over;
							}
						}
					}
				}
				else if (m_balls[0]->isSunk()) {
					// If cue sunk, reset cue, next player goes and gets penalty
					legalSink = false;
					penalize = true;
					nextState = place_cue;
					// HACK display on screen
					m_message = "Cue sunk.";
				}
				else if (!cueHit) {
					// If cue misses, foul unless first penalty shot
					if (!currentPlayer().onFirstPenaltyShot()) {
						legalSink = false;
						penalize = true;
						// HACK display on screen
						m_message = "Failed to hit any balls.";
					}
				}
				else if (m_firstHit == eight) {
					// If hit black while own on table, foul unless first penalty shot
					if (!currentPlayer().onFirstPenaltyShot() && 
						(playerSuit == none ||					// Not sunk any
							AreAnySuitLeft(playerSuit) ||		// Any on-suit remain on table
							!m_sunkThisRound.empty())) {		// Either sunk own after eight (foul) or sunk other (also foul)
						legalSink = false;
						penalize = true;
						// HACK display on screen
						m_message = "Foul hit on eight-ball.";
					}
				}
				else if (playerSuit != none && playerSuit != m_firstHit) {
					// If hit off-suit, foul unless first penalty shot
					if (!currentPlayer().onFirstPenaltyShot()) {
						legalSink = false;
						penalize = true;
						// HACK display on screen
						m_message = "Foul hit on off-colour ball.";
					}
				}
			}
			if (legalSink && !m_sunkThisRound.empty()) {
				// No foul committed, and balls sunk
				// If eightball was sunk, queue already emptied or illegal sinking
				if (playerSuit == none) {
					// Player without suit has suit set
					// TODO maybe let player pick if both groups?
					EBallSuits sunkSuit = m_sunkThisRound.front()->getSuit();
					EBallSuits otherSuit;
					if (sunkSuit == solid) {
						otherSuit = striped;
					}
					else {
						otherSuit = solid;
					}
					currentPlayer().setSuit(sunkSuit);
					otherPlayer().setSuit(otherSuit);
				}
				else {
					// Check if off-suit pocketed while not on first penalty
					if (!currentPlayer().onFirstPenaltyShot()) {
						while (!m_sunkThisRound.empty()) {
							if (m_sunkThisRound.front()->getSuit() != playerSuit) {
								// HACK display on screen
								legalSink = false;
								penalize = true;
								m_message = "Off-colour sunk.";
								break;
							}
							m_sunkThisRound.pop();
						}
					}
				}
				if (legalSink) {
					// If no foul, player gets to keep playing
					endTurn = false;
					// HACK display on screen
					m_message = "Ball sunk!";
				}
			}
			currentPlayer().useFirstShot();		// Use up first penalty shot if any
			if (penalize) {
				currentPlayer().resetPenalty();	// Lose penalty when penalized
			}
			if (endTurn && currentPlayer().hasPenalty()) {
				// Instead of ending turn, use up penalty
				endTurn = false;
				currentPlayer().resetPenalty();
			}
			switch (nextState) {
			case shot:
				if (endTurn) {
					nextPlayer();
					if (penalize) {
						m_message += " Penalty shot.";
						currentPlayer().givePenalty();
					}
				}
				else {
					// HACK display current player on screen instead
					m_message += " Shoot again.";
				}
				break;
			case place_cue:
				nextPlayer();
				// HACK display current player on screen instead
				if (penalize) {
					m_message += " Penalty shot.";
					currentPlayer().givePenalty();
				}
				break;
			case game_over:
				m_message += " Game over.";
				if (!win) {
					nextPlayer();
				}
				currentPlayer().addWin();
				break;
			default:
				break;
			}
			// Reset firstHit and sunkThisRound
			m_firstHit = none;
			m_sunkThisRound = std::queue<PoolBall*>();
			m_state = nextState;
		}
		break;
	case game_over:
		if (input->wasKeyPressed(aie::INPUT_KEY_SPACE)) {
			rack();
		}
		break;
	default:
		break;
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

	// TODO print scores
	// TODO make colours constants
	renderer->setRenderColour(1, 1, 0, 1);
	char score[32];
	sprintf_s(score, 32, "Score: %i - %i", m_player[0].getScore(), m_player[1].getScore());
	renderer->drawText(app->getFont(), score, app->getWindowWidth() - 250, app->getWindowHeight() - 32);
	// TODO at bottom of screen, print extra text about ie fouls, win or loss, etc
	renderer->drawText(app->getFont(), m_message.c_str(), 100, 16);
	// TODO print which player is currently playing, colour of their suit
	char turn[32];
	glm::vec4 playerColour;
	switch (currentPlayer().getSuit()) {
	case none:
		playerColour = { 1,1,1,1 };
		break;
	case solid:
		playerColour = PoolBall::k_solid_colour;
		break;
	case striped:
		playerColour = PoolBall::k_striped_colour;
		break;
	default:
		playerColour = { 1,1,1,1 };
		break;
	}
	renderer->setRenderColour(playerColour.r, playerColour.g, playerColour.b);
	if (m_state == game_over) {
		sprintf_s(turn, 32, "Player %i wins!", playerNumber());
	}
	else {
		sprintf_s(turn, 32, "Player %i's turn", playerNumber());
	}
	renderer->drawText(app->getFont(), turn, 300, app->getWindowHeight() - 32);
}

void PoolGame::OnCueHitBall(EBallSuits suit)
{
	// If no balls hit yet, set firstHit suit
	if (m_firstHit == none) {
		m_firstHit = suit;
	}
}

void PoolGame::OnBallSunk(PoolBall * ball)
{
	m_sunkThisRound.push(ball);
}

bool PoolGame::AreAnySuitLeft(EBallSuits suit)
{
	// Predicate for ball being on table
	auto ballNotSunk = [](PoolBallPtr b) { return !(b->isSunk()); };
	switch (suit) {
	case solid:
		// Start at m_balls[1] and stop on m_balls[8]
		return std::any_of(m_balls.begin() + 1, m_balls.begin() + 8, ballNotSunk);
		break;
	case striped:
		// Start at m_balls[9] and stop at end
		return std::any_of(m_balls.begin() + 9, m_balls.end(), ballNotSunk);
		break;
	case eight:
		return ballNotSunk(m_balls[8]);
		break;
	default:
		return false;
		break;
	}
}

PoolPlayer& PoolGame::currentPlayer()
{
	return m_player[m_playerIndex];
}

PoolPlayer& PoolGame::otherPlayer()
{
	return m_player[1 - m_playerIndex];
}

int PoolGame::playerNumber()
{
	return m_playerIndex + 1;
}

void PoolGame::nextPlayer()
{
	m_playerIndex = 1 - m_playerIndex;
}

void PoolGame::setup()
{
	m_scene->addActor(new Plane({ 1,0 }, 0.5f * k_table_width + k_rail_width, k_rail_elasticity, k_rail_friction, { 0,1,0,1 }));
	m_scene->addActor(new Plane({ -1,0 }, 0.5f * k_table_width + k_rail_width, k_rail_elasticity, k_rail_friction, { 0,1,0,1 }));
	m_scene->addActor(new Plane({ 0,1 }, 0.5f * k_table_height + k_rail_width, k_rail_elasticity, k_rail_friction, { 0,1,0,1 }));
	m_scene->addActor(new Plane({ 0,-1 }, 0.5f * k_table_height + k_rail_width, k_rail_elasticity, k_rail_friction, { 0,1,0,1 }));
	
	// TODO maybe rails should be static boxes, with pockets behind them?
	// top/bottom rails have width 0.5 * table_width - 0.5 * (pocket_width + sqrt(2) * pocket_width )  
	// side rails have height table_height - sqrt(2) * pocket_width
	// Set position as back by half of rail_width
	// Also top/bottom pockets should be moved back, to just barely be behind edge

	// Setting up rail prototype
	const float top_rail_length = 0.5f * (k_table_width - k_pocket_width - glm::root_two<float>() * k_pocket_width);
	const float side_rail_length = k_table_height - glm::root_two<float>() * k_pocket_width;
	const float top_rail_x = 0.5f * (top_rail_length + k_pocket_width);
	Box rail({ 0,0 }, top_rail_length, k_rail_width, 0, { 0,0 }, 0, INFINITY, k_rail_elasticity, k_rail_friction, 0, 0, k_rail_colour);
	rail.setStatic(true);

	// Top rails
	rail.setPosition({ top_rail_x, 0.5f * (k_table_height + k_rail_width) });
	m_scene->addActor(rail.clone());
	rail.setPosition({ -top_rail_x, 0.5f * (k_table_height + k_rail_width) });
	m_scene->addActor(rail.clone());
	rail.setPosition({ top_rail_x, -0.5f * (k_table_height + k_rail_width) });
	m_scene->addActor(rail.clone());
	rail.setPosition({ -top_rail_x, -0.5f * (k_table_height + k_rail_width) });
	m_scene->addActor(rail.clone());

	// Side rails
	rail.setHeight(side_rail_length);
	rail.setWidth(k_rail_width);
	rail.setPosition({0.5f * (k_table_width + k_rail_width), 0});
	m_scene->addActor(rail.clone());
	rail.setPosition({ -0.5f * (k_table_width + k_rail_width), 0});
	m_scene->addActor(rail.clone());

	// TODO adding pockets
	// TODO extract pocket dimensions out to constant
	// TODO pocket colour?
	Box pocket({ 0,0 }, k_pocket_width, k_pocket_width, 0);
	pocket.setStatic(true);
	pocket.setTrigger(true);
	pocket.setTags(EPoolTags::pocket);
	
	// Add each pocket
	pocket.setPosition({ 0, 0.5f * (k_table_height + k_pocket_width) });
	m_pockets.push_back(BoxPtr((Box*)pocket.clone()));
	pocket.setPosition({ 0,-0.5f * (k_table_height + k_pocket_width) });
	m_pockets.push_back(BoxPtr((Box*)pocket.clone()));
	pocket.setOrientation(glm::quarter_pi<float>());
	pocket.setPosition({ -0.5f * k_table_width, 0.5f * k_table_height });
	m_pockets.push_back(BoxPtr((Box*)pocket.clone()));
	pocket.setPosition({ 0.5f * k_table_width, -0.5f * k_table_height });
	m_pockets.push_back(BoxPtr((Box*)pocket.clone()));
	pocket.setOrientation(-glm::quarter_pi<float>());
	pocket.setPosition({ 0.5f * k_table_width, 0.5f * k_table_height });
	m_pockets.push_back(BoxPtr((Box*)pocket.clone()));
	pocket.setPosition({ -0.5f * k_table_width, -0.5f * k_table_height });
	m_pockets.push_back(BoxPtr((Box*)pocket.clone()));

	for (BoxPtr b : m_pockets) {
		m_scene->addActor(std::static_pointer_cast<PhysicsObject>(b));
	}

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

	// TODO reset player suit
	m_player[0].startNewGame();
	m_player[1].startNewGame();
	m_break = true;
	m_state = shot;
	m_firstHit = none;
	m_sunkThisRound = std::queue<PoolBall*>();
	m_message = "Break";
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
		// Change state to moving
		m_state = moving;
	}
}

bool PoolGame::isLegalCuePosition(glm::vec2 pos)
{
	// Is position within legal bounds?
	// Not worrying about break or not, just let it be placed anywhere
	const float legalExtentX = 0.5f * k_table_width - PoolBall::k_radius;
	const float legalExtentY = 0.5f * k_table_height - PoolBall::k_radius;
	if (abs(pos.x) > legalExtentX || abs(pos.y) > legalExtentY) {
		return false;
	}
	else {
		// Would it collide with other balls or the pockets?
		Sphere testSphere(pos, PoolBall::k_radius, { 0,0 });
		if (std::any_of(m_balls.begin() + 1, m_balls.end(),		// Ignore cue ball at start
			[&](PoolBallPtr ball)
		{
			// If ball's not sunk, and would collide, they will collide
			return !ball->isSunk() && (bool)(testSphere.checkSphereCollision(ball->getSphere()));
		})) {
			// If any ball would collide, illegal position
			return false;
		}
		if (std::any_of(m_pockets.begin(), m_pockets.end(),
			[&](BoxPtr box)
		{
			//  Return whether box would collide
			return (bool)(testSphere.checkBoxCollision(box.get()));
		})) {
			// If any pocket would collide, illegal position
			return false;
		}
	}
	return true;
}
