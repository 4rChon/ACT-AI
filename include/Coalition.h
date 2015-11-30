#pragma once
#include <BWAPI.h>
#include <string>

#include "Composition.h"
#include "TaskType.h"
#include "Agent.h"
#include "Agentset.h"

class Coalition
{
private:	
	Agentset agentSet;
	BWAPI::Unitset unitSet;
	Composition currentComp;
	Composition targetComp;
	TaskType currentTask;
	bool active;
	int age;
	int taskID;
	int coalitionID;
	double cost;
	double profit;
public:
	//constructors and destructors;
	Coalition();
	//Target Composition, Task ID, and Task Type
	Coalition(Composition targetComp, int taskID, TaskType taskType);
	~Coalition();

	//setters
	void setUnitSet(BWAPI::Unitset unitSet);
	void setActive(bool active);	

	//getters
	bool isActive() const;
	std::string getCurrentTaskString() const;
	TaskType getCurrentTaskType() const;
	BWAPI::Unitset getUnitSet() const;
	Agentset getAgentSet() const;
	Composition getCurrentComp() const;
	Composition getTargetComp() const;
	int getAge() const;
	int getID() const;
	double getCost();
	double getProfit();

	//-
	void addUnit(BWAPI::Unit unit);
	void addAgent(Agent* agent);	
	void removeUnit(BWAPI::Unit unit);
	void removeAgent(Agent* agent);
	void updateFreeAgents();
};