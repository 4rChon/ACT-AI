#pragma once
#include "BWTA.h"
#include "Agent.h"
#include "Worker.h"

typedef BWAPI::SetContainer < Worker*, std::hash<void*>> Workerset;

class Expand;

class ResourceDepot : public Agent
{
protected:
	//Pointer to BWTA::BaseLocation type
	BWTA::BaseLocation* baseLocation;
	//SetContainer of workers
	Workerset workers;

	//number of mineral miners
	int mineralMiners;

	//number of gas miners
	int gasMiners;

	//saturation 
	double mineralSaturation;
	double gasSaturation;

	//number of refineries built on vespene geysers
	int refineryCount;
public:
	///constructors and destructors
	ResourceDepot();
	ResourceDepot(BWAPI::Unit unit);
	~ResourceDepot();

	///getters
	//Get pointer to BWTA::BaseLocation type
	BWTA::BaseLocation* getBaseLocation() { return baseLocation; }

	//Get SetContainer of workers
	Workerset& getWorkers() { return workers; }

	//Get number of mineral miners
	int getMineralMiners() { return mineralMiners; }

	//Get number of gas miners
	int getGasMiners() { return gasMiners; }

	//Get number of refineries built on vespene geysers
	int getRefineryCount() { return refineryCount; }

	void updateFreeActions();

	//Get saturation
	double getMineralSaturation() { return mineralSaturation; }
	double getGasSaturation() { return gasSaturation; }

	///-
	//Compute Actions
	virtual void act();

	//Update saturation according to worker - resource node ratio
	void updateSaturation();

	//Update number of available refineries
	void updateRefineryCount();

	///helpers
	bool isMineralSaturated();
	bool isGasSaturated();

	//Add a mineral worker to the workerset and increment mineralMiners
	void addMineralWorker(Worker* worker);

	//Add a gas worker to the workerset and increment mineralMiners
	void addGasWorker(Worker* worker);

	//remove a worker from the workerset and decrement the corresponding miner count
	void removeWorker(Worker* worker);

	//Order the construction of a refinery on a vespene geyser node
	bool addGeyser(Worker* worker);
};