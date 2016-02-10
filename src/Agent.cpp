#include "..\include\Agent.h"
#include "..\include\EconHelper.h"
#include "..\include\Coalition.h"
#include "..\include\Task.h"

Agent::Agent()
{
	this->unit = nullptr;
	unitID = -1;
	coalitionID = -1;
	taskID = -1;

	free = true;

	task = nullptr;
	coalition = nullptr;

	initialiseCommandMap();
}

Agent::Agent(BWAPI::Unit unit)
{
	this->unit = unit;
	unitID = unit->getID();
	coalitionID = -1;
	taskID = -1;

	free = true;

	task = nullptr;
	coalition = nullptr;

	initialiseCommandMap();
}

Agent::~Agent()
{
	std::cout << "~Agent\n";
	delete task;	
	delete coalition;
}

void Agent::initialiseCommandMap()
{
	for (auto &commandType : BWAPI::UnitCommandTypes::allUnitCommandTypes())
	{		
		commandMap.insert(std::pair<BWAPI::UnitCommandType, double>(commandType, 0.0));
	}
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

void Agent::bindCheck()
{
	if (coalitionID != -1)
		if (coalition->isActive())
			free = false;
}

void Agent::unbind()
{
	coalitionID = -1;
	coalition = nullptr;
	taskID = -1;
	task = nullptr;
	free = true;
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

bool Agent::isFree() const
{
	return free;
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

bool Agent::useAbility(BWAPI::TechType ability, BWAPI::PositionOrUnit target)
{
	if (unit->canUseTech(ability, target))
		return unit->useTech(ability, target);
	return false;
}

void Agent::debugInfo() const
{
	std::cout << "\tID  : " << unitID << "\n";
	std::cout << "\tUnit: " << unit->getType() << "\n";
}