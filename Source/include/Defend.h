#pragma once
#include "Task.h"
#include "MapHelper.h"

class Defend: public Task
{
private:
	BWAPI::Unit unit;
public:
	//constructors and destructors
	Defend(BWAPI::Unit unit);

	//-
	void assign();
	void act();
	void update();
	void succeed();
	void fail();
};