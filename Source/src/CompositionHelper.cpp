#pragma once
#define BOOST_FILESYSTEM_VERSION 3
#define BOOST_FILESYSTEM_NO_DEPRECATED

#include "CompositionHelper.h"
#include "CoalitionHelper.h"
#include "UtilHelper.h"
#include "ArmyHelper.h"
#include <sstream>
#include <string>
#include <iostream>
#include <istream>
#include <fstream>
#include <stdio.h> //ceil
#include <math.h> //ceil
#include <boost/filesystem.hpp>

namespace fs = ::boost::filesystem;

namespace CompositionHelper
{
	namespace
	{
		static std::vector<UsedComposition> compositionSet;
		static std::vector<UsedComposition> workingSet;
	}

	void initialiseHelper()
	{
		loadCompositions();
	}


	Composition getComposition(Task* task)
	{ 
		TaskType taskType = task->getType();
		//TO DO: consider time proximity
		//return best composition for tasktype
		std::vector<UsedComposition> candidateSet;
		for each(auto usedComposition in compositionSet)
		{
			if (usedComposition.taskType == taskType)
			{
				if(util::game::getFrameBracket(usedComposition.activationFrame, 7200) == util::game::getFrameBracket(BWAPI::Broodwar->getFrameCount(), 7200))
					candidateSet.push_back(usedComposition);
			}
		}				

		if (candidateSet.size() > 0)
		{
			auto scoutedUnits = ArmyHelper::getScoutedUnits();
			//scoutedUnits.debugInfo();
			for each(auto unitType in scoutedUnits.getTypes())
			{
				auto counters = getCounters(unitType);
				for (auto it = candidateSet.begin(); it != candidateSet.end();)
				{
					bool canCounter = false;
					//if usedComposition contains at least one counter...
					for each(auto counterType in counters.getTypes())
					{
						if ((*it).composition[counterType] > 0)
						{
							canCounter = true;
							break;
						}
					}

					if (!canCounter)
					{
						auto nextIt = candidateSet.erase(it);
						it = nextIt;
					}
					else
						it++;
				}
			}
		}
		else
		{
			Composition c;
			switch (taskType)
			{
			case ATT:
			{
				c.addType(BWAPI::UnitTypes::Terran_Marine);
				c.addType(BWAPI::UnitTypes::Terran_Medic);
				return c;
			}
			case DEF:
			{
				for (auto& agent : AgentHelper::getAgentset())
				{
					if (agent->getUnit()->exists() && agent->isFree() && !agent->getUnit()->getType().isBuilding() && !agent->getUnit()->getType().isSpell() && !agent->getUnit()->getType().isWorker())
						c.addType(agent->getUnit()->getType());
				}
				return c;
			}
			case EXP:
			{
				c.addType(util::game::getSelf()->getRace().getWorker(), 1);
				return c;
			}
			case SCO:
			{
				c.addType(util::game::getSelf()->getRace().getWorker(), 1);
				return c;
			}
			default:
			{
				c.addType(BWAPI::UnitTypes::Terran_Marine);
				c.addType(BWAPI::UnitTypes::Terran_Medic);
				return c;
			}
			}
		}
		auto bestComposition = *candidateSet.begin();
		for each(auto candidate in candidateSet)
		{
			if (candidate.fitness > bestComposition.fitness)
				bestComposition = candidate;
		}

		//if (taskType == ATT)
		//{
		//	/*std::cout << "Countering with...\n";
		//	bestComposition.composition.debugInfo();*/
		//}
		return bestComposition.composition;
	}

	Composition getCounters(BWAPI::UnitType enemyUnitType)
	{
		auto allUnitTypes = BWAPI::UnitTypes::allUnitTypes();
			
		std::unordered_map<BWAPI::UnitType, int> unitTypeCounterAmount; //LF new name pls

		auto groundDamageType = enemyUnitType.groundWeapon().damageType();
		auto airDamageType = enemyUnitType.airWeapon().damageType();
		auto sizeType = enemyUnitType.size();
		bool flyer = enemyUnitType.isFlyer();

		for each (auto unitType in allUnitTypes)
		{	
			if (unitType.getRace() != util::game::getSelf()->getRace() || unitType.isHero() || unitType.isBuilding() || unitType.isWorker())
				continue;

			//if enemy unit flies and friendly unit can damage it...
			if (flyer)
			{
				if (unitType.airWeapon().damageAmount() > 0)
				{
					unitTypeCounterAmount[unitType]++;

					//if friendly unit flies and enemy unit can't damage it...
					if (!(enemyUnitType.airWeapon().damageAmount() > 0))
						unitTypeCounterAmount[unitType]++;

					//if enemy unit damage type is concussive and friendly unit size is small... 
					if (airDamageType == BWAPI::DamageTypes::Concussive && unitType.size() == BWAPI::UnitSizeTypes::Small)
						unitTypeCounterAmount[unitType]++;

					//if enemy unit damage type is explosive and friendly unit size is large... 
					else if (airDamageType == BWAPI::DamageTypes::Explosive && unitType.size() == BWAPI::UnitSizeTypes::Large)
						unitTypeCounterAmount[unitType]++;

					//if enemy unit size is small and friendly damage type is explosive...
					if (sizeType == BWAPI::UnitSizeTypes::Small && unitType.airWeapon().damageType() == BWAPI::DamageTypes::Explosive)
						unitTypeCounterAmount[unitType]++;

					//if enemy unit size is medium or large and friendly damage type is concussive...
					else if ((sizeType == BWAPI::UnitSizeTypes::Medium || sizeType == BWAPI::UnitSizeTypes::Large) && unitType.airWeapon().damageType() == BWAPI::DamageTypes::Concussive)
						unitTypeCounterAmount[unitType]++;
				}
				else
					continue;
			}
					

			//if enemy unit is ground unit and friendly unit can damage it...
			if (!flyer && unitType.groundWeapon().damageAmount() > 0)
			{
				unitTypeCounterAmount[unitType]++;

				//if friendly unit flies and enemy unit can't damage it...
				if (unitType.isFlyer() && !(enemyUnitType.airWeapon().damageAmount() > 0))
					unitTypeCounterAmount[unitType]++;

				//if enemy unit damage type is concussive and friendly unit size is small... 
				if (groundDamageType == BWAPI::DamageTypes::Concussive && unitType.size() == BWAPI::UnitSizeTypes::Small)
					unitTypeCounterAmount[unitType]++;

				//if enemy unit damage type is explosive and friendly unit size is large... 
				else if (groundDamageType == BWAPI::DamageTypes::Explosive && unitType.size() == BWAPI::UnitSizeTypes::Large)
					unitTypeCounterAmount[unitType]++;

				//if enemy unit size is small and friendly damage type is explosive...
				if (sizeType == BWAPI::UnitSizeTypes::Small && unitType.groundWeapon().damageType() == BWAPI::DamageTypes::Explosive)
					unitTypeCounterAmount[unitType]++;

				//if enemy unit size is medium or large and friendly damage type is concussive...
				else if ((sizeType == BWAPI::UnitSizeTypes::Medium || sizeType == BWAPI::UnitSizeTypes::Large) && unitType.groundWeapon().damageType() == BWAPI::DamageTypes::Concussive)
					unitTypeCounterAmount[unitType]++;
			}
			else
				continue;
		}
		
		/*std::cout << "Unit to Counter: \n";		
		std::cout << "\t" << enemyUnitType.getName() << "\n";*/
		/*std::cout << "\t\t" << enemyUnitType.size().c_str() << "\n";
		std::cout << "\t\t" << enemyUnitType.groundWeapon().damageType().c_str() << "\n";
		std::cout << "\t\t" << enemyUnitType.airWeapon().damageType().c_str() << "\n";*/

		/*std::cout << "Counters: \n";*/
		Composition counterComposition;
		for (auto unitType = unitTypeCounterAmount.begin(); unitType != unitTypeCounterAmount.end(); unitType++)
		{
			if (unitType->first == BWAPI::UnitTypes::Spell_Scanner_Sweep)
				counterComposition.addType(BWAPI::UnitTypes::Terran_Comsat_Station, unitType->second);
			else if (unitType->first == BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode)
				counterComposition.addType(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode, unitType->second);
			else if(unitType->first == BWAPI::UnitTypes::Terran_Vulture_Spider_Mine)
				counterComposition.addType(BWAPI::UnitTypes::Terran_Vulture, unitType->second);
			else
				counterComposition.addType(unitType->first, unitType->second);
		}
		/*counterComposition.debugInfo();*/
		return counterComposition;
	}


	const std::vector<UsedComposition>& getCompositionSet()
	{
		return compositionSet;
	}

	void loadCompositions()
	{
		auto enemy = util::game::getEnemy();

		auto directory = "ai-data\\" + enemy->getRace().getName() + "\\compositions\\";
		fs::path path = fs::path(directory);
		std::vector<fs::path> compFiles;
		util::data::getFiles(path, ".comp", compFiles);
		
		std::cout << "Loading Compositions...  (" << compFiles.size() << ")\n";

		for each(auto compPath in compFiles)
		{
			UsedComposition usedComposition = util::data::deserializeComposition(directory + compPath.string());			
			compositionSet.push_back(usedComposition);
		}

		std::cout << "Compositions Loaded...\n";
	}

	void saveComposition(Coalition* coalition)
	{
		if (coalition->getTask()->getType() != ATT)
			return;

		Composition composition;

		for each(auto& unit in coalition->getTargetComp().getUnitMap())
		{
			if(unit.second > 0)
				composition.addType(unit.first, (int)std::ceil((double)unit.second / coalition->getUnitMultiplier()));
		}

		if (composition.getTypes().size() == 0)
			return;

		UsedComposition usedComposition{
			composition,
			coalition->getTask()->getType(),
			coalition->getAge(),			
			coalition->getFitness(),
			1
		};

		workingSet.push_back(usedComposition);
	}

	void saveComposition(UsedComposition usedComposition)
	{
		workingSet.push_back(usedComposition);
	}

	void saveCompositions()
	{
		std::vector<UsedComposition> uniqueCompositions;
		for each(auto usedComposition in workingSet)
		{
			bool unique = true;
			for (auto& uniqueComposition : uniqueCompositions)
			{
				if (usedComposition.composition == uniqueComposition.composition && util::game::getFrameBracket(usedComposition.activationFrame, 7200) == util::game::getFrameBracket(uniqueComposition.activationFrame, 7200))
				{
					unique = false;
					uniqueComposition.fitness += usedComposition.fitness;
					uniqueComposition.activationFrame += usedComposition.activationFrame;
					uniqueComposition.useCount += usedComposition.useCount;
					break;
				}
			}			

			if (unique)
				uniqueCompositions.push_back(usedComposition);
		}

		for each(auto uniqueComposition in uniqueCompositions)
			util::data::serializeComposition(uniqueComposition);
	}
};