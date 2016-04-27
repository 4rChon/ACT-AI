#pragma once
#include "Agent.h"
#include "BWTA.h"

class ResourceDepot;

class Worker : public Agent
{
protected:
	//True if worker is mining gas
	bool gasMiner;

	//Pointer to current mining base
	ResourceDepot* miningBase;

	//Minerals reserved for construction
	int reservedMinerals;

	//Gas reserved for construction
	int reservedGas;

	///Private Helpers
	//Reserve minerals and gas for construction
	void reserveResources(int minerals, int gas);

	//Release minerals and gas
	void releaseResources();

	bool repair(BWAPI::Unit damagedUnit);

	bool repair();

	bool defend(BWAPI::PositionOrUnit target);
	bool defend();

	void updateFreeActions();

public:
	///constructors and desctructors
	Worker();
	Worker(BWAPI::Unit unit);
	~Worker();

	///setters
	//Set current mining base to miningBase and gasMiner to gas
	void setMiningBase(ResourceDepot* miningBase, bool gas);

	//Unset current mining base
	void unsetMiningBase();	

	///getters
	//True if gasMiner is true
	bool isMiningGas() const;

	//Get current mining base - nullptr if no mining base
	ResourceDepot* getMiningBase() const;

	///-
	//Compute actions
	virtual void act();	

	///commands
	//Specific build command for worker - construct building at desiredPosition.
	bool build(BWAPI::UnitType building, BWAPI::TilePosition * desiredPosition = nullptr);
	
	//Specific expand command for worker - expand.
	bool expand();	
	
	///Public Helpers
	//Print unitID and unitType to console
	void debugInfo() const;
};