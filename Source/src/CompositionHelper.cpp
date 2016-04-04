#pragma once
#define BOOST_FILESYSTEM_VERSION 3
#define BOOST_FILESYSTEM_NO_DEPRECATED

#include "CompositionHelper.h"
#include "UtilHelper.h"
#include "ArmyHelper.h"
#include <sstream>
#include <string>
#include <iostream>
#include <istream>
#include <fstream>
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


	Composition getComposition(TaskType taskType)
	{
		//return best composition for tasktype
		std::vector<UsedComposition> candidateSet;
		for each(auto usedComposition in compositionSet)
		{
			if (usedComposition.taskType == taskType)
				candidateSet.push_back(usedComposition);
		}				

		if (candidateSet.size() > 0)
		{
			auto scoutedUnits = ArmyHelper::getScoutedUnits();
			scoutedUnits.debugInfo();
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
		
		if(candidateSet.size() == 0)
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
			case EXP:
			{
				c.addType(util::getSelf()->getRace().getWorker(), 1);
				return c;
			}
			case SCO:
			{
				c.addType(util::getSelf()->getRace().getWorker(), 1);
				return c;
			}
			default:
				return c;
			}
		}
		auto bestComposition = *candidateSet.begin();
		for each(auto candidate in candidateSet)
		{
			if (candidate.fitness > bestComposition.fitness)
				bestComposition = candidate;
		}
		if (taskType == ATT)
		{
			std::cout << "Countering with...\n";
			bestComposition.composition.debugInfo();
		}
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
			if (unitType.getRace() != util::getSelf()->getRace() 
				|| unitType.isHero() 
				|| unitType.isBuilding() 
				|| unitType.isWorker() 
				|| unitType.supplyProvided() > 0
				|| unitType == BWAPI::UnitTypes::Terran_Vulture_Spider_Mine
				|| unitType == BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode)
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
			counterComposition.addType(unitType->first, unitType->second);
		/*counterComposition.debugInfo();*/
		return counterComposition;
	}


	const std::vector<UsedComposition>& getCompositionSet()
	{
		return compositionSet;
	}

	//const std::vector<UsedComposition>& getWorkingSet()
	//{
	//	return workingSet;
	//}

	void loadCompositions()
	{
		auto enemy = util::getEnemy();// util::getEnemy();

		auto directory = "compositions\\" + enemy->getRace().getName() + "\\";
		fs::path path = fs::path(directory);
		std::vector<fs::path> compFiles;
		util::getFiles(path, ".comp", compFiles);
		
		std::cout << "Loading Compositions...  (" << compFiles.size() << ")\n";
		for each(auto compPath in compFiles)
		{
			std::ifstream compStream;

			Composition c;
			
			//std::cout << "path: " << compPath.string() << "\n";
			compStream.open(directory + compPath.string(), std::ios::binary | std::ios::in);

			int taskType;
			int activationFrame;
			double fitness;

			compStream.read((char*)&taskType, sizeof(taskType));
			compStream.read((char*)&activationFrame, sizeof(activationFrame));
			compStream.read((char*)&fitness, sizeof(fitness));			

			int typeCount;
			compStream.read((char*)&typeCount, sizeof(typeCount));

			for (int i = 0; i < typeCount; i++)
			{				
				int unitType;
				int typeCount;
				compStream.read((char*)&unitType, sizeof(unitType));
				compStream.read((char*)&typeCount, sizeof(typeCount));
				c.addType(unitType, typeCount);
			}
			
			compStream.close();

			/*std::cout << "TaskType: " << taskType << "\n";
			std::cout << "Average Activation Time: " << activationFrame << "\n";
			std::cout << "Average Fitness: " << fitness << "\n";
			c.debugInfo();*/

			UsedComposition usedComposition{
				c,
				TaskType(taskType),
				activationFrame,
				fitness,
				1
			};
			compositionSet.push_back(usedComposition);
		}
		std::cout << "Compositions Loaded...\n";
	}

	void saveComposition(Coalition* coalition)
	{
		Composition composition;

		for each(auto& unit in coalition->getTargetComp().getUnitMap())
			composition.addType(unit.first, (int)((double)unit.second / coalition->getUnitMultiplier()));

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
			/*std::cout << "Saving...\n";
			usedComposition.composition.debugInfo();*/
			bool unique = true;
			for each(auto uniqueComposition in uniqueCompositions)
			{
				/*std::cout << "\tComparing...\n";*/
				uniqueComposition.composition.debugInfo();
				if (usedComposition.composition == uniqueComposition.composition)
				{
					/*std::cout << "\tComposition is not unique.\n";*/
					unique = false;
					uniqueComposition.fitness += usedComposition.fitness;
					uniqueComposition.activationFrame += usedComposition.activationFrame;
					uniqueComposition.useCount += usedComposition.useCount;
					break;
				}
			}

			if (unique)
			{
				/*std::cout << "\tComposition is unique.\n";*/
				uniqueCompositions.push_back(usedComposition);
			}
		}

		for each(auto uniqueComposition in uniqueCompositions)
		{
			std::ofstream compStream;
			
			auto directory = "compositions\\" + util::getEnemy()->getRace().getName() + "\\";
			auto fileName = std::to_string(uniqueComposition.activationFrame) + "_" + std::to_string(uniqueComposition.taskType);

			compStream.open(directory + fileName + ".comp", std::ios::binary | std::ios::out);
			
			int taskType = uniqueComposition.taskType;
			int activationFrame = uniqueComposition.activationFrame / uniqueComposition.useCount;			
			double fitness = uniqueComposition.fitness / uniqueComposition.useCount;

			compStream.write((char*)&taskType, sizeof(taskType));
			compStream.write((char*)&activationFrame, sizeof(activationFrame));
			compStream.write((char*)&fitness, sizeof(fitness));

			int typeCount = uniqueComposition.composition.getTypes().size();
			compStream.write((char*)&typeCount, sizeof(typeCount));

			for each(auto unitType in uniqueComposition.composition.getTypes())
			{
				int typeID = unitType.getID();				
				int typeCount = (int)((double)(uniqueComposition.composition[unitType]));
				compStream.write((char*)&typeID, sizeof(typeID));
				compStream.write((char*)&typeCount, sizeof(typeCount));
			}
			compStream.close();

			std::ofstream readableStream;

			readableStream.open(directory + "\\human\\" + fileName + ".hcomp", std::ios::out);

			readableStream << "TaskType: " << taskType << "\n";
			readableStream << "Average Activation Time: " << activationFrame << "\n";
			readableStream << "Average Fitness: " << fitness << "\n";

			for each(auto unitType in uniqueComposition.composition.getTypes())
			{
				int typeCount = (int)((double)(uniqueComposition.composition[unitType]));
				readableStream << unitType.c_str() << " : " << typeCount << "\n";
			}
			readableStream.close();
		}
	}
};