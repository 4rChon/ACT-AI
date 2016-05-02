#pragma once
#include "UtilHelper.h"
#include "MapHelper.h"
#include "Zone.h"
#include <BWAPI.h>
#include <unordered_map>

class Task;
class CreateUnit;
class Coalition;

typedef std::unordered_map<BWAPI::UnitCommandType, double> CommandMap;

class Agent
{
protected:

	BWAPI::UnitCommand lastCommand;

	//pointer to unit represented by agent - nullptr if no unit
	BWAPI::Unit unit;

	//pointer to agent's current coalition - nullptr if no coalition
	Coalition* coalition;

	//pointer to agent's current task - nullptr if no task
	Task* task;

	//unitID - -1 if no unit
	int unitID;

	//coalitionID - -1 if no coalition
	int coalitionID;

	//taskID - -1 if no task
	int taskID;

	//true if agent coalition is currently active, false otherwise
	bool free;

	//agent's current target zone - nullptr if no target
	Zone* target;

	BWAPI::Unit unitTarget;
	//number of frames that the agent has been engaged in combat
	int engageDuration;

	//last frame that agent was engaged in combat
	int lastEngaged;

	//true if agent is currently engaged in combat
	bool isEngaged;

	//number of recorded kills
	int lastKillCount;

	/*CommandMap commandMap;
	void initialiseCommandMap();*/

	///Private Helpers
	//Update Engagement Time for coalition
	void updateEngagement();

	//Update Kill Count for coalition
	void updateKillCount();

	//Update Actions when not bound to a coalition
	virtual void updateFreeActions();

	bool exists();

	void updateActions();

	//Update Coalition relevant information and micro if available
	void updateBoundActions();

public:
	///constructors	and destructors
	Agent();
	Agent(BWAPI::Unit unit);
	virtual ~Agent();
	///setters	
	//Assign agent coalition and coalitionID
	void setCoalition(Coalition* coalition);

	//Assign agent task and taskID
	void setTask(Task* task);

	//Assign agent unit and unitID
	void setUnit(BWAPI::Unit unit);

	///getters
	//Get unitID - -1 if no unit
	int getID() const;
	//Get unit - nullptr if no unit
	BWAPI::Unit getUnit() const;

	//Get coalitionID - -1 if no coalition
	int getCoalitionID() const;

	//Get assigned coalition - nullptr if no coalition
	Coalition* getCoalition() const;

	//Get taskID - -1 if no task
	int getTaskID() const;

	//Get task - nullptr if no task
	Task * getTask() const;

	//Get unit cost - mineralCost + (gasCost*1.5)
	double getPrice() const;

	//True if agent is bound to a coalition and coalition is active
	bool isFree() const;
	
	virtual void micro();	

	///-	
	//Compute agent actions
	virtual void act();

	//Poll open coalitions to join
	virtual bool pollCoalitions();

	///commands
	//Generic move command to target. Returns false if API fails to pass command.
	virtual bool move(BWAPI::Position target);

	//Generic move or attack move command to target. Returns false if API fails to pass command.
	virtual bool attack(BWAPI::PositionOrUnit target);	

	//Generic attack-move to target. Returns false if API fails to pass command. ----Changed Functionality
	virtual bool defend();

	//Expand - Returns false if not a worker.
	virtual bool expand();

	//Gather from target - Returns false if not a worker.
	virtual bool gather(BWAPI::Unit target);

	//Build building at desiredPosition - Returns false if not a worker.
	virtual bool build(BWAPI::UnitType building, BWAPI::TilePosition * desiredPosition = nullptr);

	//Build addon - Returns false if not a terran building that can build an addon.
	virtual bool buildAddon(BWAPI::UnitType addon);

	//Train unitType - Returns false if API fails to pass command.
	virtual bool train(BWAPI::UnitType unitType);

	//Morph into unitType - returns false if API fails to pass command.
	virtual bool morph(BWAPI::UnitType unitType);

	//Upgrade upgradeType - returns false if API fails to pass command.
	virtual bool upgrade(BWAPI::UpgradeType upgradeType);

	//Research techType - returns false if API fails to pass command.
	virtual bool research(BWAPI::TechType techType);

	//Generic useAbility - use ability on target if available
	virtual bool useAbility(BWAPI::TechType ability, BWAPI::PositionOrUnit target = nullptr);

	///Public Helpers
	//Set free to false if agent is currently assigned to a coalition. Does nothing if no coalition assigned.
	void bind();

	//Unbinds agent from any current coalition and set free to true.
	void unbind();

	//Print agent ID and type to console
	virtual void debugInfo() const;
};