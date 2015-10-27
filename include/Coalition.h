#pragma once
#include <BWAPI.h>
#include <string>

#include "Composition.h"
#include "TaskType.h"
#include "Agent.h"

class Coalition
{
private:	
	std::unordered_set<Agent*> agentSet;
	BWAPI::Unitset unitSet;
	Composition currentComp;
	Composition targetComp;
	TaskType currentTask;
	bool active;
	int age;
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
	std::unordered_set<Agent*> getAgentSet() const;
	Composition getCurrentComp() const;
	Composition getTargetComp() const;
	int getAge() const;

	//-
	void addUnit(BWAPI::Unit unit);
	void addAgent(Agent* agent);	
	void removeUnit(BWAPI::Unit unit);
	void removeAgent(Agent* agent);
	void updateFreeAgents();
	void Coalition::disband();
};