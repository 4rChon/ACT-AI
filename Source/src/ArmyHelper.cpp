#include "ArmyHelper.h"
#include "Attack.h"
#include "TaskHelper.h"
#include "MapHelper.h"
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
			bool operator()(const std::pair<int, int> a, const std::pair<int, int> b)
			{
				if (BWAPI::UnitType(a.second).buildScore() == BWAPI::UnitType(b.second).buildScore())
					return a.first > b.first;
				return BWAPI::UnitType(a.second).buildScore() > BWAPI::UnitType(b.second).buildScore();
			}
		};

		static bool attacking;
		static bool defending;
		static bool scouting;
		static MapHelper::Zone* attackTarget;
		static MapHelper::Zone* defendTarget;
		static MapHelper::Zone* scoutTarget;
		static MapHelper::Zone* enemyStart;
		static std::map<UnitCostPair, MapHelper::Zone*, compareUnit> targetPriorityList;
	}

	void initialiseHelper()
	{
		attacking = false;
		scouting = false;
		defending = true;		
		attackTarget = nullptr;
		defendTarget = MapHelper::getZone(BWAPI::Broodwar->getRegionAt(BWTA::getNearestChokepoint(BWTA::getStartLocation(BWAPI::Broodwar->self())->getPosition())->getCenter()));
		scoutTarget = nullptr;
		if (BWTA::getStartLocations().size() == 2)
		{
			if ((*BWTA::getStartLocations().begin()) == BWTA::getStartLocation(BWAPI::Broodwar->self()))
				enemyStart = MapHelper::getZone(BWAPI::Broodwar->getRegionAt((*(BWTA::getStartLocations().begin()++))->getPosition()));
			else
				enemyStart = MapHelper::getZone(BWAPI::Broodwar->getRegionAt((*BWTA::getStartLocations().begin())->getPosition()));
		}
		else
			enemyStart = nullptr;
	}

	void updateArmyMovement()
	{
		scout();
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
		if (targetPriorityList.size() != 0 && !attacking)
		{
			attacking = true;
			defending = false;
			Attack* attack = new Attack(attackTarget);
			TaskHelper::addTask(attack, true);
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
		auto target = std::pair<UnitCostPair, MapHelper::Zone*>(UnitCostPair(unit->getID(), unit->getType().buildScore()), MapHelper::getZone(unit->getRegion()));
		targetPriorityList.insert(target);

		attackTarget = (*targetPriorityList.begin()).second;
	}

	void removeTargetPriority(BWAPI::Unit unit)
	{
		targetPriorityList.erase(UnitCostPair(unit->getID(), unit->getType().buildScore()));
		attackTarget = (*targetPriorityList.begin()).second;
	}

}
