#pragma once
#include "Task.h"
#include "MapHelper.h"

class Attack : public Task
{
private:
	MapHelper::Zone* target;
public:
	//constructors and destructors
	Attack(MapHelper::Zone* target);

	void createCoalition();
	//-
	void assign();
	void act();
	void update();
};