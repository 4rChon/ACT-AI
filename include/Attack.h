#pragma once
#include "Task.h"
#include "MapHelper.h"

class Attack : public Task
{
private:
	MapHelper::Zone* target;
	bool scouting;
public:
	//constructors and destructors
	Attack(MapHelper::Zone* target);

	//-
	void assign();
	void act();
	void update();
};