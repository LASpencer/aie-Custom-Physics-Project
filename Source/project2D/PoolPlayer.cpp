#include "PoolPlayer.h"
#include "PoolGame.h"

PoolPlayer::PoolPlayer() : m_score(0), m_penalty(false), m_suit(none)
{
}

void PoolPlayer::startNewGame()
{
	m_suit = none;
	m_penalty = false;
}
