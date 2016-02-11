#pragma once
#include <BWTA.h>
#include "Task.h"
#include "MapHelper.h"

class Expand : public Task
{
public:
	//constructors and destructors
	Expand();

	//-
	void assign();
	void act();
	void update();
};