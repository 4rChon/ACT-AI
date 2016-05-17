#pragma once
#include "Agent.h"
#include "BWTA.h"


class ComsatStation : public Agent
{
protected:
	int lastScanned;
	BWAPI::Position lastScannedPos;

	void updateFreeActions();

public:
	///constructors and desctructors
	ComsatStation(BWAPI::Unit unit);
	~ComsatStation();

	///Compute actions
	virtual void act();

	///commands
	bool scan(BWAPI::Position target);
	

};