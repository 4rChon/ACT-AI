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

	//commands
	virtual bool move(BWAPI::Position target);
	virtual bool attack(BWAPI::PositionOrUnit target);
	virtual bool gather(BWAPI::Unit target);
	virtual bool buildAddon(BWAPI::UnitType addon);
	virtual bool train(BWAPI::UnitType unitType);
	virtual bool morph(BWAPI::UnitType unitType);
	virtual bool upgrade(BWAPI::UpgradeType upgradeType);

	//helpers
	virtual void debugInfo() const;
};