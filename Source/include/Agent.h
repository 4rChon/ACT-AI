#pragma once
#include <BWAPI.h>

class Agent
{
private:
	BWAPI::Unit unit;
	//Coalition* coalition;
public:
	//constructors
	Agent(BWAPI::Unit unit);

	//setters
	void setUnit(BWAPI::Unit unit);
	//void setCoalition(Coalition* coalition);

	//getters
	BWAPI::Unit getUnit() const;
	//Coalition* getCoalition() const;

	//-
	//void leaveCoalition();
	
};