#include "ReplayHelper.h"
#include "UtilHelper.h"
#include <BWAPI.h>
#include <BWTA.h>

namespace ReplayHelper
{
	namespace
	{
		Composition enemyComposition;
		std::vector<CompositionHelper::UsedComposition> flaggedCompositions;
	}

	void initialiseHelper()
	{
		loadFlaggedCompositions();
	}

	void getEnemyUnitsAtFrame(int frame)
	{
		if (BWAPI::Broodwar->getFrameCount() == frame)
			enemyComposition = Composition(util::getEnemy()->getUnits());
	}

	void loadFlaggedCompositions()
	{
		auto compositions = CompositionHelper::getCompositionSet();
		for each(auto composition in compositions)
		{
			if (composition.fitness < 20 && composition.taskType == ATT)
				flaggedCompositions.push_back(composition);
		}
	}

	void evolveFlaggedCompositions()
	{		
		for each(auto composition in flaggedCompositions)
		{
			if (composition.activationFrame == BWAPI::Broodwar->getFrameCount())
				evolveComposition(composition);
		}
	}

	void evolveComposition(CompositionHelper::UsedComposition composition)
	{
		std::cout << "\n==Evolving a composition==\n\n";
		std::cout << "Composition Before Evolution\n";
		composition.composition.debugInfo();
		enemyComposition = Composition(util::getEnemy()->getUnits());
		std::cout << "Loaded Enemy Composition\n";
		//respond to units with counters
		for each(auto unitType in enemyComposition.getTypes())
		{
			if (unitType.isBuilding() || unitType == BWAPI::UnitTypes::Zerg_Larva)
				continue;
			//std::cout << "Responding to unit: " << unitType.c_str() << "\n";
			auto counters = CompositionHelper::getCounters(unitType);
			auto counterTypes = counters.getTypes();
			bool canCounter = false;
			for each(auto counterType in counterTypes)
			{
				if (composition.composition[counterType] > 0)
					canCounter = true;
			}

			if (canCounter == false)
			{
				int randomIndex = rand() % counterTypes.size();
				composition.composition.addType(counterTypes[randomIndex]);
			}
			else
			{
				int evolveType = rand() % 100;
				if (evolveType <= 25)
				{
					auto intersection = composition.composition.getIntersection(counters).getTypes();
					int randomIndex = rand() % intersection.size();
					composition.composition.addType(intersection[randomIndex]);					
				}
				else if (evolveType > 25 && evolveType <= 50)
				{
					int randomIndex = rand() % counterTypes.size();
					composition.composition.addType(counterTypes[randomIndex]);
				}
			}
		}
		std::cout << "\n==Composition Evolved==\n\n";
		std::cout << "Composition After Evolution\n";
		composition.composition.debugInfo();
		//possible mutation
		int mutateProbability = rand() % 100;
		if (mutateProbability < 10)
		{
			std::cout << "\n==Mutating Composition==\n\n";
			std::vector<BWAPI::UnitType> possibleTypes;
			for each(auto unitType in BWAPI::UnitTypes::allUnitTypes())
			{
				if (unitType.getRace() != util::getSelf()->getRace()
					|| unitType.isHero()
					|| unitType.isBuilding()
					|| unitType.isWorker()
					|| unitType.supplyProvided() > 0
					|| unitType == BWAPI::UnitTypes::Terran_Vulture_Spider_Mine
					|| unitType == BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode)
					continue;
				possibleTypes.push_back(unitType);
			}

			int randomIndex = rand() % possibleTypes.size();
			composition.composition.addType(possibleTypes[randomIndex]);
			std::cout << "Composition After Mutation\n";
			composition.composition.debugInfo();
		}

		CompositionHelper::saveComposition(composition);
	}	
}