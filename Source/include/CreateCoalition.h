#pragma once
#include "Task.h"
#include "Composition.h"
#include "Coalition.h"

class CreateCoalition : public Task
{
private:
	Coalition* taskCoalition;
public:
	//constructors and destructors
	CreateCoalition(Composition targetComposition, Task* const& task);	

	//getters
	double getCost();

	//-
	void assign();
	void act();
	void update();
};