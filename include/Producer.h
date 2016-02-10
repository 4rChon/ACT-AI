#pragma once
#include "Agent.h"

class Producer : public Agent
{
public:
	Producer();
	Producer(BWAPI::Unit unit);

	~Producer();

	//getters

	//-
	virtual void act();

	//commands
	virtual bool train(BWAPI::UnitType unitType);

	//helpers
};