#pragma once
#include <BWAPI.h>
#include <string>

#include "Composition.h"
//#include "TaskType.h"
#include "Agent.h"
#include "Agentset.h"

class Task;

class Coalition
{
private:
	Agentset agentSet;
	BWAPI::Unitset unitSet; //-
	Composition currentComp;
	Composition targetComp;
	Task* task;
	//TaskType currentTask;
	bool active;
	int creationFrame;
	int taskID;	
	int coalitionID;
	double cost;
	double profit;
public:
	//constructors and destructors;
	Coalition();	
	Coalition(Composition targetComp, Task* task);
	
	~Coalition();

	//setters
	void setActive(bool active);

	//getters
	int getAge() const;
	int getID() const;
	double getCost();
	double getProfit() const;
	bool isActive() const;
	
	//-
	void addUnit(BWAPI::Unit unit);
	void addAgent(Agent* agent);
	void removeUnit(BWAPI::Unit unit);
	void removeAgent(Agent* agent);

	////getters
	BWAPI::Unitset getUnitSet() const;
	Agentset getAgentSet() const;
	//Composition getCurrentComp() const;
	//Composition getTargetComp() const;
};