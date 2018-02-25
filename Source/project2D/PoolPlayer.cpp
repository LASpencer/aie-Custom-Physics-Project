#include "PoolPlayer.h"
#include "PoolGame.h"

PoolPlayer::PoolPlayer() : m_score(0), m_penalty(false), m_firstPenaltyShot(false), m_suit(none)
{
}

void PoolPlayer::givePenalty()
{
	m_penalty = true;
	m_firstPenaltyShot = true;
}

void PoolPlayer::useFirstShot()
{
	m_firstPenaltyShot = false;
}

void PoolPlayer::resetPenalty()
{
	m_penalty = false;
	m_firstPenaltyShot = false;
}

void PoolPlayer::startNewGame()
{
	m_suit = none;
	m_penalty = false;
	m_firstPenaltyShot = false;
}
