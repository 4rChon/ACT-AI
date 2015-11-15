#pragma once
#include "Task.h"
#include "Zone.h"

class Defend : public Task
{
private:
	Zone* target;
public:
	Defend(Zone* target);

	void assign();
	void act();
	void update();
};