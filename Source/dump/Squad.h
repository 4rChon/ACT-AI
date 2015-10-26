#pragma once
#include "ThreatField.h"
#include "Zone.h"
#include "_Unit.h"
#include <vector>
#include <BWAPI.h>

class Squad : public BWAPI::AIModule
{
private:
	std::vector<_Unit> unitSet;
	Zone *target;
	bool active;

public:
	Squad(BWAPI::Unitset unitset);
	Squad(std::vector<_Unit> unitset);
	~Squad();
	//bool act(std::vector<Order> orderSet)
	Zone* getZone();
	std::vector<_Unit> getUnitSet();

	void setActive(bool activity);
	void setZone(Zone* z);
	void setUnitSet(BWAPI::Unitset unitSet);
	void setUnitSet(std::vector<_Unit> unitSet);
	void attack(std::vector<Zone*> field);
	void patrol(std::vector<Zone*> field);
	void move(std::vector<Zone*> field);
	void holdPosition(std::vector<Zone*> field);
};
