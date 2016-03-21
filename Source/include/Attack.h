#pragma once
#include "Task.h"
#include "MapHelper.h"

class Attack : public Task
{
public:
	//constructors and destructors
	Attack(MapHelper::Zone* target);

	//-
	void createCoalition();
	void assign();
	void act();
	void update();
	void succeed();
};