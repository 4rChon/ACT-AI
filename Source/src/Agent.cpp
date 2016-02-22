#include "..\include\Agent.h"
#include "..\include\EconHelper.h"
#include "..\include\CoalitionHelper.h"
#include "..\include\Coalition.h"
#include "..\include\Task.h"
#include "..\include\CreateUnit.h"

Agent::Agent()
{
	unit = nullptr;
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
	//std::cout << "~Agent\n";
	/*AgentHelper::removeAgent(this);*/	
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
	std::cout << unitID << " : Binding agent\n";
	free = false;
}

void Agent::unbind()
{
	std::cout << unitID << " : Unbinding Agent\n";
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

Coalition* Agent::getCoalition() const
{
	return coalition;
}

int Agent::getTaskID() const
{
	return taskID;
}

Task* Agent::getTask() const
{
	return task;
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
	if (unit->getType().canProduce())
		train(*unit->getType().buildsWhat().begin());
	//temp contents
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

bool Agent::build(BWAPI::UnitType building, BWAPI::TilePosition * desiredPosition)
{
	std::cout << "Agent Build\n";
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
	if (unit->canTrain(unitType) && EconHelper::haveMoney(unitType) && EconHelper::haveSupply(unitType) && unit->getTrainingQueue().size() < 1)
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

bool Agent::research(BWAPI::TechType techType)
{
	if (unit->canResearch(techType))
		return unit->research(techType);
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