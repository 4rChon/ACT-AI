#pragma once
#include "Task.h"
#include "MapHelper.h"

class Attack : public Task
{
private:
	MapHelper::Zone* target;
	bool scouting;
	bool scoutingFinished;
public:
	//constructors and destructors
	Attack(MapHelper::Zone* target);

	void createCoalition();

	void scout();

	//-
	void assign();
	void act();
	void update();
	void succeed();
	void fail();
};