#pragma once
#include "Task.h"
#include "Zone.h"

class Attack : public Task
{
private:
	Zone* target;
public:
	//constructors and destructors
	Attack(Zone* target);
	~Attack();

	//-
	void assign();
	void act();
	void update();
};