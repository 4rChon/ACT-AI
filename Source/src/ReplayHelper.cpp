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

	void evolveComposition(CompositionHelper::UsedComposition usedComp)
	{
		std::cout << "\n==Evolving a composition==\n\n";
		std::cout << "Composition Before Evolution\n";
		usedComp.composition.printDebugInfo();
		enemyComposition = Composition(util::game::getEnemy()->getUnits());
		std::cout << "Loaded Enemy Composition\n";
		//respond to units with counters
		if (usedComp.useCount > 1)
		{
			if (usedComp.fitness < 0.5)
			{
				for each(auto unitType in enemyComposition.getTypes())
				{
					if (unitType.isBuilding() || unitType == BWAPI::UnitTypes::Zerg_Larva)
						continue;
					//std::cout << "Responding to unit: " << unitType.c_str() << "\n";
					auto counters = CompositionHelper::getCounters(unitType);

					if (usedComp.composition.getIntersection(counters).getTypes().size() > 0)
					{
						//add random unit type from counter list
						BWAPI::UnitType randomType = util::getRandomType(counters);
						usedComp.composition.addType(randomType);
					}
					else
					{
						int evolveType = rand() % 100;

						//increase amount for existing unit type
						if (evolveType <= 33)
						{
							auto intersection = usedComp.composition.getIntersection(counters).getUnitMap();
							BWAPI::UnitType randomType = util::getRandomType(intersection);
							usedComp.composition.addType(randomType);
						}

						//add a new counter unit type
						else if (evolveType > 33 && evolveType <= 66)
						{
							BWAPI::UnitType randomType = util::getRandomType(counters);
							usedComp.composition.addType(randomType);
						}
					}
				}

				std::cout << "\n==Composition Evolved==\n\n";
				std::cout << "Composition After Evolution\n";
				usedComp.composition.printDebugInfo();
			}			

			////possible mutation
			//int mutateProbability = rand() % 100;
			//if (mutateProbability < 5)
			//	composition = mutateComposition(composition);
		}
		else
		{			
			std::cout << "Removing Unit\n";
			BWAPI::UnitType randomType = util::getRandomType(usedComp.composition);
			usedComp.composition.removeType(randomType);

			std::cout << "\n==Composition Evolved==\n\n";
			std::cout << "Composition After Evolution\n";
			usedComp.composition.printDebugInfo();

			if (usedComp.composition.getCost() == 0)
				usedComp.composition = mutateComposition(usedComp.composition);
		}

		usedComp.useCount = 1;

		CompositionHelper::saveComposition(usedComp);
	}

	Composition mutateComposition(Composition composition)
	{
		std::cout << "\n==Mutating Composition==\n\n";
		std::vector<BWAPI::UnitType> possibleTypes;

		BWAPI::UnitType randomUnit = util::getRandomType(BWAPI::UnitTypes::Men);
		composition.addType(randomUnit);
		std::cout << "\n\n==Composition After Mutation==\n";
		composition.printDebugInfo();

		return composition;
	}
}