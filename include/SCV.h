#pragma once
#include "Worker.h"


class SCV : public Worker
{	
public:
	//constructor
	SCV(BWAPI::Unit unit);
			
	//-
	void act();

	//commands
	bool repair(BWAPI::Unit unit);
	bool repair();
};