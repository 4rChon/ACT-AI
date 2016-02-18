#pragma once
#include "BWTA.h"
#include "Agent.h"
#include "Worker.h"

typedef BWAPI::SetContainer < Worker*, std::hash<void*>> Workerset;

class Expand;

class ResourceDepot : public Agent
{
protected:
	BWTA::BaseLocation* baseLocation;
	Workerset workers;
	int mineralMiners;
	int gasMiners;
	double expandDesire;
public:
	//constructors and destructors
	ResourceDepot();
	ResourceDepot(BWAPI::Unit unit);

	~ResourceDepot();

	//getters
	BWTA::BaseLocation* getBaseLocation();
	Workerset& getWorkers();
	int getMineralMiners();
	int getGasMiners();

	//-
	virtual void act();

	void updateExpandDesire();

	//helpers
	bool isMineralSaturated();
	bool isGasSaturated();
	void addMineralWorker(Worker* worker);
	void addGasWorker(Worker* worker);
	void removeWorker(Worker* worker);
};