#pragma once
#include "Agent.h"
#include "BWTA.h"

class ResourceDepot;

class Worker : public Agent
{
private:	
	bool gasMiner;									//True if worker is mining gas	
	ResourceDepot* miningBase;						//Pointer to current mining base
	int reservedMinerals;							//Minerals reserved for construction
	int reservedGas;								//Gas reserved for construction

	///Private Helpers
	void reserveResources(int minerals, int gas);	//Reserve minerals and gas for construction
	void releaseResources();						//Release minerals and gas

	bool repair(BWAPI::Unit damagedUnit);
	bool repair(BWAPI::Region region);

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
	void setMiningBase(ResourceDepot* const& miningBase, bool gas);	//Set current mining base to miningBase and gasMiner to gas
	void unsetMiningBase();											//Unset current mining base

	///getters
	bool isMiningGas() const					{ return gasMiner; }	//True if gasMiner is true
	ResourceDepot* getMiningBase() const		{ return miningBase; }	//Get current mining base - nullptr if no mining base

	///-
	virtual void act();	//Compute actions

	///commands
	bool build(BWAPI::UnitType building, BWAPI::TilePosition * desiredPosition = nullptr);		//Specific build command for worker - construct building at desiredPosition.
	bool expand();																				//Specific expand command for worker - expand.
	
	///Public Helpers
	void debugInfo() const;		//Print unitID and unitType to console
};