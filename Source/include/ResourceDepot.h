#pragma once
#include "BWTA.h"
#include "Agent.h"
#include "Worker.h"

typedef BWAPI::SetContainer < Worker*, std::hash<void*>> Workerset;

class Expand;

class ResourceDepot : public Agent
{
protected:
	BWTA::BaseLocation* baseLocation;		//Pointer to BWTA::BaseLocation type
	Workerset workers;						//SetContainer of workers

	int mineralMiners;						//number of mineral miners
	int gasMiners;							//number of gas miners

	//saturation 
	double mineralSaturation;
	double gasSaturation;

	int refineryCount;						//number of refineries built on vespene geysers
public:
	///constructors and destructors
	ResourceDepot();
	ResourceDepot(BWAPI::Unit unit);
	~ResourceDepot();

	///getters
	BWTA::BaseLocation* getBaseLocation()			{ return baseLocation; }		//Get pointer to BWTA::BaseLocation type
	const Workerset& getWorkers()					{ return workers; }				//Get SetContainer of workers
	int getMineralMiners()							{ return mineralMiners; }		//Get number of mineral miners
	int getGasMiners()								{ return gasMiners; }			//Get number of gas miners	
	int getRefineryCount()							{ return refineryCount; }		//Get number of refineries built on vespene geysers

	void updateFreeActions();

	//Get saturation
	double getMineralSaturation()					{ return mineralSaturation; }
	double getGasSaturation()						{ return gasSaturation; }

	///-	
	virtual void act();			//Compute Actions
	void updateSaturation();	//Update saturation according to worker - resource node ratio
	void updateRefineryCount();	//Update number of available refineries

	///helpers
	bool isMineralSaturated();
	bool isGasSaturated();

	void addMineralWorker(Worker* const& worker);	//Add a mineral worker to the workerset and increment mineralMiners
	void addGasWorker(Worker* const& worker);		//Add a gas worker to the workerset and increment mineralMiners
	void removeWorker(Worker* const& worker);		//remove a worker from the workerset and decrement the corresponding miner count
	bool addGeyser(Worker* const& worker);			//Order the construction of a refinery on a vespene geyser node
};