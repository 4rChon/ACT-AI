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
	Agentset agentSet;					//Agents assigned to coalition							
	BWAPI::Unitset unitSet;				//Units from the agentset assigned to coalition	
	Composition currentComp;			//Current unit composition	
	Composition targetComp;				//Target unit composition	
	Task* task;							//Current assigned task - nullptr if no task	
	int const taskID;					//Current assigned taskID - -1 if no task	
	bool active;						//True if currentComp >= targetComp
	bool focusFire;
	int const creationFrame;			//Set to first frame of coalition's existence	
	int activationFrame;				//Set to first frame of coalition's activation	
	int const coalitionID;				//coalitioniD - obtained from CoalitionHelper	
	double cost;						//Current coalition money and time cost	
	double profit;						//Current coalition profit	
	int engageDuration;					//Amount of time spent engaging the enemy
	int killCount;						//Number of total kills for all agents	
	double unitMultiplier;				//Unit multiplier used on composition	
	void addUnit(BWAPI::Unit unit);		//Add a unit to the coalition.	
	void removeUnit(BWAPI::Unit unit);	//Remove a unit from the coalition.

public:
	///constructors and destructors;
	Coalition();
	Coalition(Composition targetComp, Task* const& task);
	~Coalition();

	///getters		
	int getAge() const;							//Get frame count for coalition creation. If coalition is active, get activation time for coalition.	
	int getActivationFrame() const		{ std::cout << "Creation Frame " << creationFrame << "\n"; return activationFrame; }
	int getID() const					{ return coalitionID; }	//Get a unique coalition ID.	
	double getCost();							//Get the cost of the coalition so far.	
	double getProfit();							//Get the profit of the coalition so far.	
	double getFitness();						//Get the fitness of the coalition so far.	
	double getUnitMultiplier() const	{ return unitMultiplier; }	//Get the unit multiplier used on the composition.	
	bool isActive() const				{ return active; }			//Returns true if coalition is active.
	void activate();	
	Task* getTask() const				{ return task; }		//Get current assigned task.	
	Composition getCurrentComp() const	{ return currentComp; }	//Get the current unit composition assigned to the coalition.	
	Composition getTargetComp() const	{ return targetComp; }	//Get the target composition required for the coalition to activate.	
	BWAPI::Unitset &getUnitSet()		{ return unitSet; }		//Get the units assigned to the coalition.	
	Agentset &getAgentSet()				{ return agentSet; }	//Get the agents assigned to the coalition.
	
	///helpers		
	bool addAgent(Agent* const& agent);			//Add an agent to the coalition.	
	void removeAgent(Agent* const& agent);		//Remove an agent from the coalition.	
	void addEngagement()					{ engageDuration++; }	//Increase engagement count.	
	void addKillCount(int killCount = 1)	{ this->killCount += killCount; }	//Increase kill count.	
	void printAttributes();						//Output coalition attributes.	
	void logCoalition();						//Write human readable information about the coalition to a file.
};