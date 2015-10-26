#pragma once
#include "Task.h"
#include "Zone.h"

class Attack : public Task
{
private:
	Zone* target;
public:
	//constructors
	Attack(Zone* target);

	//-
	void assign();
	void act();
	void update();
};