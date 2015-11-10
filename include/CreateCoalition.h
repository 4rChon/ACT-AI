#pragma once
#include "Task.h"
#include "Composition.h"
#include "Coalition.h"

class CreateCoalition : public Task
{
private:
	Coalition* taskCoalition;
public:
	//constructors
	CreateCoalition(Composition targetComposition, Task* task);

	//getters
	bool getCoalitionState();
	double getCost();

	//-
	void assign();
	void act();
	void update();
};