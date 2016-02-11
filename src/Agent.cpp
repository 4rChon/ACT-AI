#include "..\include\Agent.h"
#include "..\include\EconHelper.h"
#include "..\include\CoalitionHelper.h"
#include "..\include\Coalition.h"
#include "..\include\Task.h"

Agent::Agent()
{
	this->unit = nullptr;
	unitID = -1;
	coalitionID = -1;
	taskID = -1;

	free = true;
	movingToBuild = false;

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
	unbind();
}

void Agent::initialiseCommandMap()
{
	for (auto &commandType : BWAPI::UnitCommandTypes::allUnitCommandTypes())
	{		
		commandMap.insert(std::pair<BWAPI::UnitCommandType, double>(commandType, 0.0));
	}
}

void Agent::setCoalition(Coalition* coalition)
{
	coalitionID = coalition->getID();	
	this->coalition = coalition;
}

void Agent::setTask(Task* task)
{
	taskID = task->getID();
	this->task = task;
}

void Agent::setUnit(BWAPI::Unit unit)
{
	this->unit = unit;
}

void Agent::bind()
{
	std::cout << "Binding agent " << unitID << "\n";
	free = false;
}

void Agent::unbind()
{
	if (coalitionID != -1)
		coalition->removeAgent(this);

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

bool Agent::isMovingToBuild() const
{
	return movingToBuild;
}

bool Agent::isFree() const
{
	return free;
}

void Agent::act()
{
	//do stuff
}

bool Agent::pollCoalitions()
{
	for (auto &coalition : CoalitionHelper::getCoalitionset())
		if (!coalition->isActive())
			if (coalition->addAgent(this))
				return true;
	return false;
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

bool Agent::expand()
{
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