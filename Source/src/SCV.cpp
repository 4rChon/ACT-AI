#include "SCV.h"
#include "AgentHelper.h"
#include "EconHelper.h"
#include "DesireHelper.h"

SCV::SCV(BWAPI::Unit unit)
{
	this->unit = unit;
	unitID = unit->getID();
}

void SCV::updateFreeActions()
{

}

void SCV::act()
{	
	if (!exists())
		return;

	updateActions();

	if (free)
		updateFreeActions();		
	else
		updateBoundActions();
}