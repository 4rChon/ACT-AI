#pragma once
#include "ResourceDepot.h"

class CommandCenter : public ResourceDepot
{
public:
	//constructor
	CommandCenter(BWAPI::Unit unit);

	//-
	void act();
};