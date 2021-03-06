#include "Agent.h"
#include "EconHelper.h"
#include "CoalitionHelper.h"
#include "Coalition.h"
#include "DesireHelper.h"
#include "Task.h"
#include "CreateUnit.h"
#include "ArmyHelper.h"
#include "TaskHelper.h"
#include "Defend.h"
#include "Zone.h"

Agent::Agent()
	: unit(nullptr)
	, unitID(-1)
	, task(nullptr)
	, taskID(-1)
	, coalition(nullptr)
	, coalitionID(-1)
	, free(true)
	, target(nullptr)
	, unitTarget(nullptr)
	, engageDuration(0)
	, lastEngaged(0)
	, isEngaged(false)
	, lastKillCount(0)
	, boredom(0)
{
}

Agent::Agent(BWAPI::Unit unit)
	: unit(unit)
	, task(nullptr)
	, coalition(nullptr)
	, unitID(unit->getID())
	, coalitionID(-1)
	, taskID(-1)
	, free(true)
	, target(nullptr)
	, unitTarget(nullptr)
	, engageDuration(0)
	, lastEngaged(0)
	, isEngaged(false)
	, lastKillCount(0)
	, lastCommand(unit->getLastCommand())
	, boredom(0)
{
}

Agent::~Agent()
{
}

void Agent::setCoalition(Coalition* const& coalition)
{
	coalitionID = coalition->getID();
	this->coalition = coalition;
}

void Agent::setTask(Task* const& task)
{
	taskID = task->getID();
	this->task = task;
	target = task->getTarget();
}

void Agent::bind()
{
	free = false;
	if (unit->isLoaded())
		unit->getTransport()->unload(unit);
}

void Agent::unbind()
{
	coalitionID = -1;
	coalition = nullptr;
	taskID = -1;
	task = nullptr;
	target = nullptr;
	free = true;
}

void Agent::micro()
{	
	auto attackUnits = coalition->getUnitSet();
	for (auto &unit : coalition->getUnitSet())
	{
		if (unit->getType().groundWeapon() == BWAPI::WeaponTypes::None || unit->getType().airWeapon() == BWAPI::WeaponTypes::None)
			attackUnits.erase(unit);
	}

	BWAPI::Position coalitionCenter = attackUnits.getPosition();

	BWAPI::Position targetCenter = target->getRegion()->getCenter();
	int coalitionSize = coalition->getUnitSet().size();

	static bool centering = false;
	static bool attacking = false;	

	if (unit->getDistance(coalitionCenter) > (10 + (coalitionSize * 5) * (unit->getType().width() / 8)))
	{		
		if (!centering && unit->hasPath(coalitionCenter) && BWAPI::Broodwar->getUnitsOnTile(BWAPI::TilePosition(coalitionCenter), BWAPI::Filter::IsBuilding).size() == 0)
		{
			centering = true;
			attacking = false;
			if (!attack(coalitionCenter))
				move(coalitionCenter);
		}
	}	
	else if(!attacking)
	{
		attacking = true;
		centering = false;
		if (unit->getType() == BWAPI::UnitTypes::Terran_Medic)
		{
			auto injuredUnit = unit->getClosestUnit(
				!BWAPI::Filter::IsBeingHealed
				&& BWAPI::Filter::IsAlly
				&& BWAPI::Filter::HP_Percent < 100
				&& !BWAPI::Filter::IsMechanical,
				coalitionSize * 3
				);

			if (!useAbility(BWAPI::TechTypes::Healing, injuredUnit))
				move(coalitionCenter);
		}
		else
		{
			if (unit->getType().groundWeapon() != BWAPI::WeaponTypes::None)
			{
				auto inRange = unit->getUnitsInWeaponRange(unit->getType().groundWeapon(), BWAPI::Filter::IsEnemy);
				if (inRange.size() != 0)
				{
					unitTarget = *inRange.begin();
					int targetPriority = 1;
					for (auto &enemyUnit : inRange)
					{
						int friendliesInRange = enemyUnit->getUnitsInRadius(enemyUnit->getType().sightRange(), BWAPI::Filter::IsOwned).size();
						if (friendliesInRange > targetPriority)
							unitTarget = enemyUnit;
					}
				}
			}
			else if (unit->getType().airWeapon() != BWAPI::WeaponTypes::None)
			{
				auto inRange = unit->getUnitsInWeaponRange(unit->getType().airWeapon(), BWAPI::Filter::IsEnemy);
				if (inRange.size() != 0)
				{
					unitTarget = *inRange.begin();
					int targetPriority = 1;
					for (auto &enemyUnit : inRange)
					{
						int friendliesInRange = enemyUnit->getUnitsInRadius(enemyUnit->getType().sightRange(), BWAPI::Filter::IsOwned).size();
						if (friendliesInRange > targetPriority)
						{
							unitTarget = enemyUnit;
							targetPriority = friendliesInRange;
						}
					}
				}
			}
			if (unitTarget != nullptr)
			{
				if (unitTarget->exists())
					attack(unitTarget);
				else
					attack(targetCenter);
			}			
			else
				attack(targetCenter);
		}
	}

	if (unit->isIdle())
		attack(targetCenter);
}

void Agent::updateEngagement()
{
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
}

void Agent::updateKillCount()
{
	if (unit->getType().groundWeapon() != BWAPI::WeaponTypes::None || unit->getType().groundWeapon() != BWAPI::WeaponTypes::None)
		coalition->addKillCount(unit->getKillCount() - lastKillCount);
	lastKillCount = unit->getKillCount();
}

void Agent::updateBoredom()
{
	if (!isEngaged)
	{
		boredom++;
		if (boredom > 300)
			coalition->removeAgent(this);
		return;
	}

	boredom = 0;	
}

void Agent::updateBoundActions()
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
		micro();

	auto inRange = unit->getUnitsInRadius(unit->getType().sightRange(), BWAPI::Filter::IsNeutral && !BWAPI::Filter::IsInvincible);

	for (auto &neutralUnit : inRange)
	{
		if (attack(neutralUnit))
			break;
	}

	updateEngagement(); 
	updateKillCount();
	if(task->getType() == ATT)
		updateBoredom();
}

void Agent::updateFreeActions()
{
	pollCoalitions();

	if (!free)
		return;

	if (unit->getType().canBuildAddon())
	{
		BWAPI::SetContainer<BWAPI::UnitType> addonList;

		for (auto &unitType : unit->getType().buildsWhat())
			if (unitType.isAddon())
				addonList.insert(unitType);

		if (addonList.size() > 1)
		{
			for (auto &task : TaskHelper::getAllTasks())
			{
				if (task->getType() == CRU)
				{
					auto unitType = task->getUnitType();
					if (unitType.isAddon() && addonList.count(unitType) != 0)
					{
						if (buildAddon(unitType))
							return;
					}
				}
			}
		}
		else
			if(buildAddon(DesireHelper::getMostDesirableAddon(unit->getType())))
				return;
	}

	if (unit->getType().canProduce())
	{
		if(train(DesireHelper::getMostDesirableUnit(unit->getType())))
			return;
	}

	//if (unit->canUpgrade())
	//{
	//	upgrade(DesireHelper::getMostDesirableUpgrade(unit->getType()));
	//	for (auto upgradeType : unit->getType().upgradesWhat())
	//	{
	//		if (upgrade(upgradeType))
	//			return;
	//	}
	//}

	//if (unit->canResearch())
	//{
	//	research(DesireHelper::getMostDesirableResearch(unit->getType()));
	//	for (auto researchType : unit->getType().researchesWhat())
	//	{
	//		if (research(researchType))
	//			return;
	//	}
	//}	

	if(unit->isIdle())
		defend();	
}

bool Agent::exists()
{
	if (!this)
		return false;
	if (!unit)
		return false;
	if (unit->getID() <= 0)
		return false;

	if (!unit->exists())
		return false;

	return true;
}

void Agent::updateActions()
{
	if (unit->getType().isBuilding())
		unit->setRallyPoint(unit->getClosestUnit(BWAPI::Filter::IsResourceDepot && BWAPI::Filter::IsOwned));
	
	if (unit->isUnderAttack() && !MapHelper::getZone(unit->getRegion())->isDefending())
	{
		if (task)
		{
			if (task->getType() == EXP)
			{
				Defend* defend = new Defend(unit);
				TaskHelper::addTask(defend, true);
			}
		}
		else if ((unit->getType().isBuilding() || unit->getUnitsInRadius(unit->getType().sightRange(), (BWAPI::Filter::IsBuilding || BWAPI::Filter::IsBeingConstructed) && BWAPI::Filter::IsOwned).size() > 0))
		{
			Defend* defend = new Defend(unit);
			TaskHelper::addTask(defend, true);
		}
	}	
}

void Agent::act()
{
	if (!exists())
		return;

	updateActions();

	if (unit->getType() == BWAPI::UnitTypes::Terran_Supply_Depot)
		return;

	if (free)
		updateFreeActions();
	else
		updateBoundActions();
}

bool Agent::pollCoalitions()
{
	for (auto &candidateCoalition : CoalitionHelper::getCoalitionset())
	{
		if (!candidateCoalition->isActive())
		{
			double agentFitness = 0.0;
			if(unit->getType().maxHitPoints() != 0)
				agentFitness += unit->getHitPoints() / unit->getType().maxHitPoints();

			Zone* candidateTarget = nullptr;

			if(candidateCoalition->getTask() != nullptr)
				candidateTarget = candidateCoalition->getTask()->getTarget();

			if (candidateTarget != nullptr)
				agentFitness += util::calc::normaliseDistance(unit->getPosition(), candidateTarget->getRegion()->getCenter());

			agentFitness /= 2;
			if (candidateCoalition->addAgent(this) && util::calc::getRandom(0, 1) < agentFitness)
				return true;
		}
	}
	return false;
}

bool Agent::move(BWAPI::Position target)
{	
	if (unit->canMove())
	{
		if (lastCommand.getType() == BWAPI::UnitCommandTypes::Move && lastCommand.getTargetPosition() == target)
			return false;
		return unit->move(target);
	}
	return false;
}

bool Agent::attack(BWAPI::PositionOrUnit target)
{	
	if (unit->canAttack(target))
	{
		if (target.isPosition())
		{
			if (lastCommand.getType() == BWAPI::UnitCommandTypes::Attack_Move && lastCommand.getTargetPosition() == target.getPosition())
				return false;
		}

		if (target.isUnit())
		{
			if (lastCommand.getType() == BWAPI::UnitCommandTypes::Attack_Unit && lastCommand.getTarget() == target.getUnit())
				return false;
		}

		return unit->attack(target);
	}
	return false;
}

bool Agent::defend()
{	
	BWAPI::Position targetPosition = unit->getPosition();
	Zone *targetZone = DesireHelper::getUnitDefenseTarget();	

	auto closestResourceDepot = unit->getClosestUnit(BWAPI::Filter::IsResourceDepot && BWAPI::Filter::IsOwned);
	
	if(closestResourceDepot)
		targetPosition = BWAPI::Position(closestResourceDepot->getPosition());

	if (targetZone)
		targetPosition = targetZone->getRegion()->getCenter();
	
	if (!targetPosition)
		targetPosition = BWAPI::Position(unit->getClosestUnit(BWAPI::Filter::IsBuilding && BWAPI::Filter::IsOwned)->getPosition());

	if (unit->getType() == BWAPI::UnitTypes::Terran_Bunker)
	{
		int spaceRemaining = unit->getSpaceRemaining();
		if (spaceRemaining == 0)
			return false;

		auto closestUnit = unit->getClosestUnit(
			BWAPI::Filter::IsOwned
			&& BWAPI::Filter::ArmorUpgrade == BWAPI::UpgradeTypes::Terran_Infantry_Armor
			&& !BWAPI::Filter::IsWorker
			&& BWAPI::Filter::SpaceRequired <= spaceRemaining
			&& BWAPI::Filter::IsIdle
			);

		if (closestUnit != nullptr)
		{
			if (AgentHelper::getAgent(closestUnit->getID())->isFree() && closestUnit->getType().groundWeapon() != BWAPI::WeaponTypes::None)							
				return unit->load(closestUnit);			
		}
		return false;
	}
	else if (unit->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode)
	{
		if (move(targetPosition))					
			return useAbility(BWAPI::TechTypes::Tank_Siege_Mode, targetPosition);		
	}

	

	if (MapHelper::getZone(unit->getRegion()) != MapHelper::getZone(BWAPI::Broodwar->getRegionAt(targetPosition)))			
		return attack(targetPosition);
	else
		return false;
}

bool Agent::gather(BWAPI::Unit target)
{
	if (unit->canGather(target))
	{
		if (lastCommand.getType() == BWAPI::UnitCommandTypes::Gather && lastCommand.getUnit() == target)
			return false;
		return unit->gather(target);
	}
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