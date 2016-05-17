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
	//Agents assigned to coalition
	Agentset agentSet;

	//Units from the agentset assigned to coalition
	BWAPI::Unitset unitSet;

	//Current unit composition
	Composition currentComp;

	//Target unit composition
	Composition targetComp;

	//Current assigned task - nullptr if no task
	Task* task;

	//Current assigned taskID - -1 if no task
	int taskID;

	//True if currentComp >= targetComp
	bool active;

	bool focusFire;

	//Set to first frame of coalition's existence
	int creationFrame;

	//Set to first frame of coalition's activation
	int activationFrame;
	
	//coalitioniD - obtained from CoalitionHelper
	const int coalitionID;

	//Current coalition money and time cost
	double cost;

	//Current coalition profit
	double profit;

	//Amount of time spent engaging the enemy
	int engageDuration;

	//Number of total kills for all agents
	int killCount;

	//Unit multiplier used on composition
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
	int getID() const { return coalitionID; }

	//Get the cost of the coalition so far.
	double getCost();

	//Get the profit of the coalition so far.
	double getProfit();

	//Get the fitness of the coalition so far.
	double getFitness();

	//Get the unit multiplier used on the composition.
	double getUnitMultiplier() { return unitMultiplier; }

	//Returns true if coalition is active.
	bool isActive() const { return active; }

	void activate();

	//Get current assigned task.
	Task* getTask() const { return task; }

	//Get the current unit composition assigned to the coalition.
	Composition getCurrentComp() const { return currentComp; }

	//Get the target composition required for the coalition to activate.
	Composition getTargetComp() const { return targetComp; }

	//Get the units assigned to the coalition.
	BWAPI::Unitset &getUnitSet() { return unitSet; }

	//Get the agents assigned to the coalition.
	Agentset &getAgentSet() { return agentSet; }

	///helpers	
	//Add an agent to the coalition.
	bool addAgent(Agent* agent);	

	//Remove an agent from the coalition.
	void removeAgent(Agent* agent);

	//Increase engagement count.
	void addEngagement() { engageDuration++; }

	//Increase kill count.
	void addKillCount(int killCount = 1) { this->killCount += killCount; }

	//Output coalition attributes.
	void printAttributes();

	//Write human readable information about the coalition to a file.
	void logCoalition();
};