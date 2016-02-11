#pragma once
#include "UtilHelper.h"
#include <BWAPI.h>
#include <unordered_map>

class Task;
class Coalition;

typedef std::unordered_map<BWAPI::UnitCommandType, double> CommandMap;

class Agent
{
protected:
	BWAPI::Unit unit;

	Coalition* coalition;
	Task* task;

	int unitID;
	int coalitionID;	
	int taskID;

	bool free;
	bool movingToBuild;

	CommandMap commandMap;

	void initialiseCommandMap();

public:
	//constructors	and destructors
	Agent();
	Agent(BWAPI::Unit unit);
	virtual ~Agent();	

	//setters	
	void setCoalition(Coalition* coalition);
	void setTask(Task* task);
	void setUnit(BWAPI::Unit unit);

	//getters
	int getID() const;
	int getCoalitionID() const;
	int getTaskID() const;
	BWAPI::Unit getUnit() const;
	double getPrice() const;
	bool isMovingToBuild() const;
	bool isFree() const;

	//-
	virtual void act();
	virtual bool pollCoalitions();

	//commands
	virtual bool move(BWAPI::Position target);
	virtual bool attack(BWAPI::PositionOrUnit target);
	virtual bool expand();
	virtual bool gather(BWAPI::Unit target);
	virtual bool build(BWAPI::UnitType building, BWAPI::TilePosition* desiredPosition);
	virtual bool buildAddon(BWAPI::UnitType addon);
	virtual bool train(BWAPI::UnitType unitType);
	virtual bool morph(BWAPI::UnitType unitType);
	virtual bool upgrade(BWAPI::UpgradeType upgradeType);
	virtual bool useAbility(BWAPI::TechType ability, BWAPI::PositionOrUnit target = nullptr);

	//helpers
	void bind();
	void unbind();
	virtual void debugInfo() const;
};