#include "..\include\Agent.h"
#include "..\include\EconHelper.h"

Agent::Agent()
{
	this->unit = nullptr;
	this->unitID = -1;
	this->coalitionID = -1;
	this->taskID = -1;
}

Agent::Agent(BWAPI::Unit unit)
{
	this->unit = unit;
	this->unitID = unit->getID();
	this->coalitionID = -1;
	this->taskID = -1;
}

Agent::~Agent()
{
	std::cout << "~Agent\n";
}

void Agent::setCoalitionID(int id)
{
	coalitionID = id;	
}

void Agent::setTaskID(int id)
{
	taskID = id;
}

void Agent::setUnit(BWAPI::Unit unit)
{
	this->unit = unit;
}

int Agent::getID() const
{
	return unitID;
}

int Agent::getCoalitionID() const
{
	return coalitionID;
}

int Agent::getTaskID() const
{
	return taskID;
}

BWAPI::Unit Agent::getUnit() const
{
	return unit;
}

double Agent::getPrice() const
{
	return unit->getType().mineralPrice() + (unit->getType().gasPrice() * 1.5);
}

void Agent::act()
{
	//do stuff
}

bool Agent::move(BWAPI::Position target)
{
	if (unit->canMove())
		return unit->move(target);
	return false;
}

bool Agent::attack(BWAPI::PositionOrUnit target)
{
	if (unit->canAttack(target))
		return unit->attack(target);
	return false;
}

bool Agent::gather(BWAPI::Unit target)
{
	if (unit->canGather(target))
		return unit->gather(target);
	return false;
}

bool Agent::buildAddon(BWAPI::UnitType addon)
{
	if (unit->canBuildAddon(addon) && EconHelper::haveMoney(addon))
		return unit->buildAddon(addon);	
	return false;
}

bool Agent::train(BWAPI::UnitType unitType)
{
	if (unit->canTrain(unitType) && EconHelper::haveMoney(unitType) && EconHelper::haveSupply(unitType))
		return this->unit->train(unitType);
	return false;
}

bool Agent::morph(BWAPI::UnitType unitType)
{
	if (unit->canMorph(unitType) && EconHelper::haveMoney(unitType))
		return(unit->morph(unitType));	
	return false;
}

bool Agent::upgrade(BWAPI::UpgradeType upgradeType)
{
	if (unit->canUpgrade(upgradeType) && EconHelper::haveMoney(upgradeType))
		return unit->upgrade(upgradeType);
	return false;
}

void Agent::debugInfo() const
{
	std::cout << "\tID  : " << unitID << "\n";
	std::cout << "\tUnit: " << unit->getType() << "\n";
}