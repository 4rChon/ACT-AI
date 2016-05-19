#include "ArmyHelper.h"
#include "Attack.h"
#include "CreateUnit.h"
#include "TaskHelper.h"
#include "MapHelper.h"
#include "EconHelper.h"
#include "CoalitionHelper.h"
#include "Scout.h"
#include <BWAPI.h>
#include <BWTA.h>

typedef std::pair<int, int> UnitCostPair;

namespace ArmyHelper
{
	namespace
	{
		struct compareUnit
		{
			bool operator()(const UnitCostPair a, const UnitCostPair b)
			{
				if (BWAPI::UnitType(a.second).buildScore() == BWAPI::UnitType(b.second).buildScore())
					return a.first > b.first;
				return BWAPI::UnitType(a.second).buildScore() > BWAPI::UnitType(b.second).buildScore();
			}
		};

		static bool attacking;
		static bool defending;
		static bool scouting;
		static double productionRatio;
		static Zone* scoutTarget;
		static Zone* enemyStart;		
		static std::map<UnitCostPair, Zone*, compareUnit> targetPriorityList;
		static std::unordered_map<int, BWAPI::UnitType> scoutedUnits;
	}

	void initialiseHelper()
	{
		attacking = false;
		scouting = false;
		defending = true;		
		scoutTarget = nullptr;
		if (BWTA::getStartLocations().size() == 2)
		{
			if ((*BWTA::getStartLocations().begin()) == BWTA::getStartLocation(util::game::getSelf()))
				enemyStart = MapHelper::getZone(BWAPI::Broodwar->getRegionAt((*(BWTA::getStartLocations().begin()++))->getPosition()));
			else
				enemyStart = MapHelper::getZone(BWAPI::Broodwar->getRegionAt((*BWTA::getStartLocations().begin())->getPosition()));
		}
		else
			enemyStart = nullptr;
	}

	void updateArmyMovement()
	{
		if(!scouting)
			scout();
		//if(!attacking)
		attack();		
	}

	void scout()
	{
		for each (auto &bases in BWTA::getStartLocations())
		{
			scoutTarget = MapHelper::getZone(BWAPI::Broodwar->getRegionAt(bases->getPosition()));
			if (BWAPI::Broodwar->getFrameCount() - scoutTarget->getLastVisited() > 1500)
			{
				if (scouting)
					return;

				Scout* scout = new Scout(scoutTarget);
				TaskHelper::addTask(scout, true);
				scouting = true;
			}
		}

		for each (auto &bases in BWTA::getBaseLocations())
		{
			scoutTarget = MapHelper::getZone(BWAPI::Broodwar->getRegionAt(bases->getPosition()));
			if (BWAPI::Broodwar->getFrameCount() - scoutTarget->getLastVisited() > 7500)
			{
				if (scouting)
					return;

				Scout* scout = new Scout(scoutTarget);
				TaskHelper::addTask(scout, true);
				scouting = true;
			}
		}
	}

	void attack()
	{		
		if (targetPriorityList.size() > 0)
		{
			//attacking = true;
			for (auto &target : targetPriorityList)
			{		
				Taskset& attackTasks = TaskHelper::getAttackTasks();
				if (attackTasks.size() >= 3)
					return;

				bool sameRegion = false;
				for (auto &task : attackTasks)
				{
					if (target.second->getSuperRegion()->getCenter() == task->getTarget()->getSuperRegion()->getCenter())
					{
						sameRegion = true;
						break;
					}
				}

				if (!sameRegion)
				{
					Attack* attack = new Attack(target.second);
					TaskHelper::addTask(attack, true);
				}
			}
		}
	}

	void defend()
	{
		attacking = false;
		defending = true;
	}

	void stopScouting()
	{
		scouting = !scouting;
	}

	bool isAttacking()
	{
		return attacking;
	}

	bool isDefending()
	{
		return defending;
	}

	void addTargetPriority(BWAPI::Unit unit)
	{
		auto target = std::pair<UnitCostPair, Zone*>(UnitCostPair(unit->getID(), unit->getType()), MapHelper::getZone(unit->getRegion()));
		targetPriorityList.insert(target);
	}

	void removeTargetPriority(BWAPI::Unit unit)
	{
		for (auto& target : targetPriorityList)
		{
			if (target.first.first == unit->getID())
				targetPriorityList.erase(target.first);				
		}
	}

	void updateTargetPriority()
	{
		for (auto &target : targetPriorityList)
		{
			if (target.second->getLastVisited() < 5 && !BWAPI::Broodwar->getUnit(target.first.first)->exists())
				targetPriorityList.erase(target.first);
		}
	}

	void clearZoneTargets(Zone* const&zone)
	{
		for (auto &target : targetPriorityList)
		{
			if (target.second == zone)
				targetPriorityList.erase(target.first);
		}
	}

	void addScoutedUnit(int unitID, BWAPI::UnitType unitType)
	{
		scoutedUnits.insert(std::pair<int, BWAPI::UnitType>(unitID, unitType));
	}

	void removeScoutedUnit(int unitID)
	{
		scoutedUnits.erase(unitID);
	}

	Composition getScoutedUnits()
	{
		Composition scoutedUnitComposition;
		for (auto unitType = scoutedUnits.begin(); unitType != scoutedUnits.end(); unitType++)
			scoutedUnitComposition.addType(unitType->second);
		return scoutedUnitComposition;
	}

	void printPriorityList(int count)
	{
		int i = 0;
		for (auto& target : targetPriorityList)
		{
			BWAPI::Broodwar->drawTextScreen(125, 250 + (10 * ++i), "%d : %s - %d", target.first.first, ((BWAPI::UnitType)target.first.second).getName().c_str(), target.second->getID());
			if (i == count)
				break;
		}
	}
}
