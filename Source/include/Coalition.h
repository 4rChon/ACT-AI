#pragma once
#include <BWAPI.h>
#include <string>
#include "Composition.h"
#include "Agent.h"
#include "AgentHelper.h"
#include <vector>

class Task;

class Coalition
{
private:
	Agentset agentSet;
	BWAPI::Unitset unitSet;
	Composition currentComp;
	Composition targetComp;
	Task* task;
	bool active;
	int creationFrame;
	int activationFrame;
	int taskID;	
	int coalitionID;	
	double cost;
	double profit;
	int engageDuration;
	int killCount;
	double unitMultiplier;

	//Add a unit to the coalition.
	void addUnit(BWAPI::Unit unit);
	//Remove a unit from the coalition.
	void removeUnit(BWAPI::Unit unit);

public:
	///constructors and destructors;
	Coalition();	
	Coalition(Composition targetComp, Task* task);
	
	~Coalition();	

	///getters	
	//Get frame count for coalition creation. If coalition is active, get activation time for coalition.
	int getAge() const;
	//Get a unique coalition ID.
	int getID() const;
	//Get the cost of the coalition so far.
	double getCost();
	//Get the profit of the coalition so far.
	double getProfit();
	//Get the fitness of the coalition so far.
	double getFitness();
	//Get the unit multiplier used on the composition.
	double getUnitMultiplier();
	//Returns true if coalition is active.
	bool isActive() const;
	//Get current assigned task.
	Task * getTask() const;
	//Get the current unit composition assigned to the coalition.
	Composition getCurrentComp() const;
	//Get the target composition required for the coalition to activate.
	Composition getTargetComp() const;
	//Get the units assigned to the coalition.
	BWAPI::Unitset &getUnitSet();
	//Get the agents assigned to the coalition.
	Agentset &getAgentSet();
	
	///helpers	
	//Add an agent to the coalition.
	bool addAgent(Agent* agent);	
	//Remove an agent from the coalition.
	void removeAgent(Agent* agent);
	//Increase engagement count.
	void addEngagement();
	//Increase kill count.
	void addKillCount(int killCount = 1);
	//Output coalition attributes.
	void outAttributes();
	//Write human readable information about the coalition to a file.
	void logCoalition();
};