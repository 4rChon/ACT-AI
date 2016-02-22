#pragma once
#include <BWTA.h>
#include "Task.h"
#include "MapHelper.h"

class Expand : public Task
{
private:
	Agent* builder;
public:
	//constructors and destructors
	Expand();
	~Expand();

	void createCoalition();

	//-
	void assign();
	void act();
	void update();
};