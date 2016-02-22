#pragma once
#include <BWAPI.h>
#include <string>
#include "Composition.h"
#include "Agent.h"
#include "AgentHelper.h"

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
	struct Attributes
	{
		double groundDPS;
		double airDPS;
		int totalHealth;
		double avgGroundRange;
		double avgAirRange;
		double maxGroundRange;
		double maxAirRange;
		double avgSpeed;
		bool detection;
	} attributes;
public:
	//constructors and destructors;
	Coalition();	
	Coalition(Composition targetComp, Task* task);
	
	~Coalition();

	//getters	
	int getAge() const;
	int getID() const;
	double getCost();
	double getProfit() const;
	bool isActive() const;
	Composition getCurrentComp() const;
	Composition getTargetComp() const;
	BWAPI::Unitset getUnitSet() const;
	Agentset getAgentSet() const;
	Attributes &getAttributes();
	
	//-
	void initAttributes();
	void outAttributes();
	bool addAgent(Agent* agent);
	void addUnit(BWAPI::Unit unit);	
	void removeAgent(Agent* agent);
	void removeUnit(BWAPI::Unit unit);		
};