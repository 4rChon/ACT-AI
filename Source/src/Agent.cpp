#include "Agent.h"
#include "EconHelper.h"
#include "CoalitionHelper.h"
#include "Coalition.h"
#include "DesireHelper.h"
#include "Task.h"
#include "CreateUnit.h"
#include "ArmyHelper.h"

Agent::Agent()
{
	unit = nullptr;
	task = nullptr;
	coalition = nullptr;

	unitID = -1;
	coalitionID = -1;
	taskID = -1;

	free = true;

	target = nullptr;

	isAttacking = false;

	engageDuration = 0;
	lastEngaged = 0;
	isEngaged = false;
	lastKillCount = 0;

	/*initialiseCommandMap();*/
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

	target = nullptr;

	isAttacking = false;

	engageDuration = 0;
	lastEngaged = 0;
	isEngaged = false;
	lastKillCount = 0;

	/*initialiseCommandMap();*/
}

Agent::~Agent()
{
	//std::cout << "~Agent\n";
	/*AgentHelper::removeAgent(this);*/
}

//void Agent::initialiseCommandMap()
//{
//	for (auto &commandType : BWAPI::UnitCommandTypes::allUnitCommandTypes())
//		commandMap.insert(std::pair<BWAPI::UnitCommandType, double>(commandType, 0.0));
//}

void Agent::setCoalition(Coalition* coalition)
{
	coalitionID = coalition->getID();
	this->coalition = coalition;
}

void Agent::setTask(Task* task)
{
	taskID = task->getID();
	this->task = task;
	target = task->getTarget();
}

void Agent::setUnit(BWAPI::Unit unit)
{
	this->unit = unit;
}

void Agent::bind()
{
	//std::cout << unitID << " : Binding agent\n";
	free = false;
}

void Agent::unbind()
{
	//std::cout << unitID << " : Unbinding Agent\n";
	coalitionID = -1;
	coalition = nullptr;
	taskID = -1;
	task = nullptr;
	target = nullptr;
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

bool Agent::isFree() const
{
	return free;
}

void Agent::updateCoalitionStatus()
{
	if (!task)
	{
		unbind();
		return;
	}

	if (unit->isStuck())
	{
		coalition->removeAgent(this);
		return;
	}

	if (task->getType() == ATT)
	{
		BWAPI::Position coalitionCenter = coalition->getUnitSet().getPosition();
		int coalitionSize = coalition->getUnitSet().size();

		if (unit->getDistance(coalitionCenter) > (coalitionSize * 5 * (unit->getType().width() / 8)))
		{
			if (unit->hasPath(coalitionCenter))
			{
				if (!attack(coalitionCenter))
					move(coalitionCenter);
				isAttacking = false;
			}
		}
		else if (unit->getType() == BWAPI::UnitTypes::Terran_Medic)
		{
			auto injuredUnit = unit->getClosestUnit(
				!BWAPI::Filter::IsBeingHealed
				&& BWAPI::Filter::IsAlly
				&& BWAPI::Filter::HP_Percent < 100
				&& BWAPI::Filter::ArmorUpgrade == BWAPI::UpgradeTypes::Terran_Infantry_Armor,
				coalitionSize * 3
				);

			if (!useAbility(BWAPI::TechTypes::Healing, injuredUnit))
				attack(target->getRegion()->getCenter());
		}
		else if (!isAttacking)
		{
			attack(target->getRegion()->getCenter());
			isAttacking = true;
		}
	}

	int framesSinceLastEngage = BWAPI::Broodwar->getFrameCount() - lastEngaged;

	if (framesSinceLastEngage > 24 * 10)
	{
		isEngaged = false;
	}

	if (!isEngaged && (unit->isUnderAttack() || unit->isAttacking()))
	{
		lastEngaged = BWAPI::Broodwar->getFrameCount();
		engageDuration++;
		coalition->addEngagement();
		isEngaged = true;
	}

	if (unit->canAttack())
		coalition->addKillCount(unit->getKillCount() - lastKillCount);
	lastKillCount = unit->getKillCount();
}

void Agent::act()
{
	if (unit->getID() == 0)
		return;

	if (unit->isUnderAttack())
	{
		auto allBullets = BWAPI::Broodwar->getBullets();
		for (auto bullet : allBullets)
		{
			if (bullet->getType() == BWAPI::BulletTypes::Subterranean_Spines)
				ArmyHelper::scan(bullet->getPosition());
		}
	}

	if (free)
	{
		if (unit->getType().canProduce())
		{
			train(DesireHelper::getMostDesirableUnit(unit->getType()));
			return;
		}

		if (unit->canUpgrade())
		{
			for (auto upgradeType : unit->getType().upgradesWhat())
			{
				if (upgrade(upgradeType))
					return;
			}
		}
		if (unit->canResearch())
		{
			for (auto researchType : unit->getType().researchesWhat())
			{
				if (research(researchType))
					return;
			}
		}

		defend(BWAPI::Position(util::getSelf()->getStartLocation()));
	}
	else
		updateCoalitionStatus();
}

//add swarm equation
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

bool Agent::defend(BWAPI::Position target)
{
	if (unit->getType() == BWAPI::UnitTypes::Terran_Bunker)
	{
		int spaceRemaining = unit->getSpaceRemaining();
		auto closestUnit = unit->getClosestUnit(
			BWAPI::Filter::ArmorUpgrade == BWAPI::UpgradeTypes::Terran_Infantry_Armor
			&& !BWAPI::Filter::IsWorker
			&& BWAPI::Filter::SpaceRequired <= spaceRemaining
			&& BWAPI::Filter::IsIdle
			);

		if (closestUnit != nullptr)
		{
			if (AgentHelper::getAgent(closestUnit->getID())->isFree())
			{
				std::cout << "Found a nearby bunker\n";
				return unit->load(closestUnit);
			}
		}
		return false;
	}
	else if (unit->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode)
	{
		if (move(target))
			return useAbility(BWAPI::TechTypes::Tank_Siege_Mode, target);
	}

	if (unit->isIdle())
		return attack(target);
	else
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
	if (BWAPI::Broodwar->canMake(addon, unit) && EconHelper::haveMoney(addon))
		return unit->buildAddon(addon);
	return false;
}

bool Agent::train(BWAPI::UnitType unitType)
{
	if (BWAPI::Broodwar->canMake(unitType, unit) && EconHelper::haveMoney(unitType) && unit->getTrainingQueue().size() < 1)
		return unit->train(unitType);
	return false;
}

bool Agent::morph(BWAPI::UnitType unitType)
{
	if (BWAPI::Broodwar->canMake(unitType, unit) && EconHelper::haveMoney(unitType))
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