#pragma once
#include "Task.h"
#include "MapHelper.h"

class Attack : public Task
{
public:
	//constructors and destructors
	Attack(Zone* target);

	//-
	void assign();
	void act();
	void update();
	void fail();
	void succeed();
};