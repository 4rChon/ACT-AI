#pragma once
#include <BWAPI.h>
#include <string>

#include "Composition.h"
#include "TaskType.h"

class Coalition
{
private:
	BWAPI::Unitset unitSet;
	Composition currentComp;
	Composition targetComp;
	TaskType currentTask;
	bool active;
public:
	//constructors
	Coalition();
	Coalition(Composition targetComp, TaskType taskType);

	//setters
	void setUnitSet(BWAPI::Unitset unitSet);
	void setActive(bool active);

	//getters
	bool isActive() const;
	std::string getCurrentTaskString() const;
	TaskType getCurrentTaskType() const;
	BWAPI::Unitset getUnitSet() const;
	Composition getCurrentComp() const;
	Composition getTargetComp() const;

	//-
	void addUnit(BWAPI::Unit unit);
	void removeUnit(BWAPI::Unit unit);
};