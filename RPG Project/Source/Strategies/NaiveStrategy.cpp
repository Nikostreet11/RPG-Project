/*
 * NaiveStrategy.cpp
 *
 *  Created on: Jul 6, 2019
 *      Author: nicop
 */

#include "NaiveStrategy.hpp"

#include "..\Actions\Action.hpp"
#include "..\Entities\Character.hpp"

std::shared_ptr<Strategy> NaiveStrategy::INSTANCE = nullptr;

// Static constructor
std::shared_ptr<Strategy> NaiveStrategy::getInstance()
{
	if (!INSTANCE)
	{
		INSTANCE = std::shared_ptr<Strategy>(new NaiveStrategy());
	}

	return INSTANCE;
}

// Destructor
NaiveStrategy::~NaiveStrategy()
{
}

// Functions
std::shared_ptr<Action> NaiveStrategy::chooseAction(
		std::vector<std::shared_ptr<Action>> actions)
{
	Randomizer& rand = Randomizer::getInstance();

	size_t index;

	do
	{
		index = static_cast<unsigned>(rand.getBetween(0, actions.size()));
	}
	while (actions[index]->isFriendly());

	return actions[index];
}

std::shared_ptr<Character> NaiveStrategy::chooseTarget(
		std::vector<std::shared_ptr<Character>> allies,
		std::vector<std::shared_ptr<Character>> enemies)
{
	Randomizer& rand = Randomizer::getInstance();
	size_t index;

	do
	{
		index = static_cast<unsigned>(rand.getBetween(0, enemies.size()));
	}
	while (!enemies[index]->isAlive());

	return enemies[index];
}

// Constructor / Destructor
NaiveStrategy::NaiveStrategy()
{
}
