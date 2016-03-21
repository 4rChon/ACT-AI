#pragma once
#define BOOST_FILESYSTEM_VERSION 3
#define BOOST_FILESYSTEM_NO_DEPRECATED

#include "CompositionHelper.h"
#include "UtilHelper.h"
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

		if (candidateSet.size() == 0)
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
				c.addType(BWAPI::Broodwar->self()->getRace().getWorker(), 1);
				return c;
			}
			case SCO:
			{
				c.addType(BWAPI::Broodwar->self()->getRace().getWorker(), 1);
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
		return bestComposition.composition;
	}

	const std::vector<UsedComposition>& getCompositionSet()
	{
		return compositionSet;
	}

	const std::vector<UsedComposition>& getWorkingSet()
	{
		return workingSet;
	}

	void loadCompositions()
	{		
		auto directory = "compositions\\" + BWAPI::Broodwar->enemy()->getRace().getName() + "\\";
		fs::path path = fs::path(directory);
		std::vector<fs::path> compFiles;
		util::getFiles(path, ".comp", compFiles);
		
		std::cout << "Loading Compositions...  (" << compFiles.size() << ")\n";
		for each(auto compPath in compFiles)
		{
			std::ifstream compStream;

			Composition c;
			
			std::cout << "path: " << compPath.string() << "\n";
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

			std::cout << "TaskType: " << taskType << "\n";
			std::cout << "Average Activation Time: " << activationFrame << "\n";
			std::cout << "Average Fitness: " << fitness << "\n";
			c.debugInfo();
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

	void saveCompositions()
	{
		std::vector<UsedComposition> uniqueCompositions;
		for each(auto usedComposition in workingSet)
		{
			bool unique = true;
			for each(auto uniqueComposition in uniqueCompositions)
			{
				if (usedComposition.composition == uniqueComposition.composition)
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
		{
			std::ofstream compStream;
			
			auto directory = "compositions\\" + BWAPI::Broodwar->enemy()->getRace().getName() + "\\";
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

			readableStream.open(directory + "human_" + fileName + ".hcomp", std::ios::out);

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