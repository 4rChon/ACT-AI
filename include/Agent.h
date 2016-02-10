#pragma once
#include <BWAPI.h>

class Agent
{
protected:
	BWAPI::Unit unit;

	int unitID;
	int coalitionID;	
	int taskID;

public:
	//constructors	and destructors
	Agent();
	Agent(BWAPI::Unit unit);

	virtual ~Agent();

	//setters	
	void setCoalitionID(int id);
	void setTaskID(int id);
	void setUnit(BWAPI::Unit unit);

	//getters
	int getID() const;
	int getCoalitionID() const;
	int getTaskID() const;

	BWAPI::Unit getUnit() const;

	double getPrice() const;

	//-
	virtual void act();
	virtual void debugInfo();
};