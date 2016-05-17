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
	
	BWAPI::Unit unit;		//pointer to unit represented by agent - nullptr if no unit
	Coalition* coalition;	//pointer to agent's current coalition - nullptr if no coalition
	Task* task;				//pointer to agent's current task - nullptr if no task
	int unitID;				//unitID - -1 if no unit
	int coalitionID;		//coalitionID - -1 if no coalition
	int taskID;				//taskID - -1 if no task
	bool free;				//true if agent coalition is currently active, false otherwise
	Zone* target;			//agent's current target zone - nullptr if no target
	BWAPI::Unit unitTarget;
	int engageDuration;		//number of frames that the agent has been engaged in combat
	int lastEngaged;		//last frame that agent was engaged in combat
	bool isEngaged;			//true if agent is currently engaged in combat
	int lastKillCount;		//number of recorded kills

	/*CommandMap commandMap;
	void initialiseCommandMap();*/

	///Private Helpers
	void updateEngagement();	//Update Engagement Time for coalition
	void updateKillCount();		//Update Kill Count for coalition
	void updateActions();
	void updateBoundActions();	//Update Coalition relevant information and micro if available
	virtual void updateFreeActions();	//Update Actions when not bound to a coalition

	bool exists();

public:
	///constructors	and destructors
	Agent();
	Agent(BWAPI::Unit unit);
	virtual ~Agent();
	
	///setters	
	void setCoalition(Coalition* coalition);	//Assign agent coalition and coalitionID
	void setTask(Task* task);					//Assign agent task and taskID
	void setUnit(BWAPI::Unit unit) { this->unit = unit; unitID = unit->getID(); }

	///getters
	int getID() const				{ return unitID; }
	BWAPI::Unit getUnit() const		{ return unit; };
	int getCoalitionID() const		{ return coalitionID; }
	Coalition* getCoalition() const { return coalition; }
	int getTaskID() const			{ return taskID; }
	Task * getTask() const			{ return task; }
	double getPrice() const			{ return unit->getType().mineralPrice() + (unit->getType().gasPrice() * 1.5); }
	bool isFree() const				{ return free; }	//True if agent is bound to a coalition and coalition is active

	///-	
	virtual void act(); //Compute agent actions
	virtual void micro();

	virtual bool pollCoalitions();	//Poll open coalitions to join

	///commands
	//Generic move command to target. Returns false if API fails to pass command.
	virtual bool move(BWAPI::Position target);

	//Generic move or attack move command to target. Returns false if API fails to pass command.
	virtual bool attack(BWAPI::PositionOrUnit target);	

	//Generic attack-move to target. Returns false if API fails to pass command. ----Changed Functionality
	virtual bool defend();

	//Expand - Returns false if not a worker.
	virtual bool expand() { return false; }

	//Gather from target - Returns false if not a worker.
	virtual bool gather(BWAPI::Unit target);

	//Build building at desiredPosition - Returns false if not a worker.
	virtual bool build(BWAPI::UnitType building, BWAPI::TilePosition * desiredPosition = nullptr) { return false; }

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