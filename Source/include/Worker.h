#pragma once
#include "Agent.h"
#include "BWTA.h"

class ResourceDepot;

class Worker : public Agent
{
protected:
	bool gasMiner;
	ResourceDepot* miningBase;
	bool reservedResources;
public:
	//constructors and desctructors
	Worker();
	Worker(BWAPI::Unit unit);

	~Worker();

	//setters
	void setMiningBase(ResourceDepot* miningBase, bool gas);
	void unsetMiningBase();	

	//getters
	bool isMiningGas() const;
	ResourceDepot* getMiningBase() const;

	//-
	virtual void act();	

	//commands
	bool build(BWAPI::UnitType building, BWAPI::TilePosition * desiredPosition = nullptr);
	bool expand();
	
	//helpers
	void debugInfo() const;
};