#include "Agent.h"
//#include "Globals.h"

Agent::Agent(BWAPI::Unit unit)
{
	this->unit = unit;
}

void Agent::setUnit(BWAPI::Unit unit)
{
	this->unit = unit;
}

//void Agent::setCoalition(Coalition* coalition)
//{
//	if (!coalition->isActive())
//		for (auto type : coalition->getTargetComp().getTypes())
//			if (this->unit->getType() == type)
//			{
//				this->coalition = coalition;
//				this->coalition->addUnit(this->unit);
//				g_FreeAgents.erase(this);
//				return;
//			}
//}

BWAPI::Unit Agent::getUnit() const
{
	return this->unit;
}

//Coalition* Agent::getCoalition() const
//{
//	return this->coalition;
//}
//
//void Agent::leaveCoalition()
//{
//	if (this->coalition != nullptr)
//		this->coalition->removeUnit(this->unit);	
//}