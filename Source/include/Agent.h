#pragma once
#include "UtilHelper.h"
#include "MapHelper.h"
#include <BWAPI.h>
#include <unordered_map>

class Task;
class CreateUnit;
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

	MapHelper::Zone* target;

	bool isAttacking;

	int engageDuration;
	int lastEngaged;
	bool isEngaged;	
	int lastKillCount;
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
	Coalition* getCoalition() const;
	int getTaskID() const;
	Task * getTask() const;
	BWAPI::Unit getUnit() const;
	double getPrice() const;
	bool isFree() const;

	void updateCoalitionStatus();

	//-
	virtual void act();
	virtual bool pollCoalitions();

	//commands
	virtual bool move(BWAPI::Position target);
	virtual bool attack(BWAPI::PositionOrUnit target);
	virtual bool defend(BWAPI::Position target);
	virtual bool expand();
	virtual bool gather(BWAPI::Unit target);
	virtual bool build(BWAPI::UnitType building, BWAPI::TilePosition * desiredPosition = nullptr);
	virtual bool buildAddon(BWAPI::UnitType addon);
	virtual bool train(BWAPI::UnitType unitType);
	virtual bool morph(BWAPI::UnitType unitType);
	virtual bool upgrade(BWAPI::UpgradeType upgradeType);
	virtual bool research(BWAPI::TechType techType);
	virtual bool useAbility(BWAPI::TechType ability, BWAPI::PositionOrUnit target = nullptr);

	//helpers
	void bind();
	void unbind();
	virtual void debugInfo() const;
};