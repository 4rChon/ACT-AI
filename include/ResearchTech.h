#pragma once
#include "Task.h"

class ResearchTech : public Task
{
private:
	BWAPI::TechType techType;
	bool satisfying;
	bool satisfied;
public:
	//constructors
	ResearchTech(BWAPI::TechType techType);
	void createCoalition();

	//-
	void assign(); //obtain a coalition capable of researching tech
	void satisfyRequirements();
	void act(); //order coalition to research tech
	void update(); //update process
};