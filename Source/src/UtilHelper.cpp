#include "UtilHelper.h"
#include "CompositionHelper.h"
#include "Composition.h"
#include "MapHelper.h"
#include "Zone.h"
#include <string>
#include <vector>
#include <math.h>
#include <iostream>
#include <random>
#include <chrono>

namespace util
{			
	namespace
	{
		static std::string selfName;
		static BWAPI::Player self;
		static BWAPI::Player enemy;
		static BWAPI::Playerset enemySet;
		static BWAPI::TilePosition playerStartLocation;
	}

	void initialiseUtil()
	{
		game::setSelf();
		game::setStartLocation();
		game::setEnemy();
		std::cout << selfName << " (" << self->getRace().getName() << ") vs " << enemy->getName() << " (" << enemy->getRace().getName() << ")\n";
	}	

	namespace calc
	{
		double normaliseValues(std::vector<double> valueArr, std::vector<double> coeffArr)
		{
			double total = 0.0;
			for (std::size_t i = 0; i < valueArr.size(); i++)
				total += 1 / (1 + std::exp(-coeffArr[i] * valueArr[i]));

			return ((2 / valueArr.size()) * total) - 1;
		}

		double normaliseDistance(BWAPI::Position pos1, BWAPI::Position pos2)
		{
			double dist = sqrt(pow(pos1.x - pos2.x, 2) + pow(pos1.y - pos2.y, 2));
			double mapDist = sqrt(pow(BWAPI::Broodwar->mapWidth(), 2) + pow(BWAPI::Broodwar->mapHeight(), 2));
			return (mapDist - dist) / mapDist;
		}

		double getRandom(int min, int max)
		{
			std::mt19937_64 rng;
			uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
			std::seed_seq ss{ uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed >> 32) };
			rng.seed(ss);
			std::uniform_real_distribution<double> unif(0, 1);
			return unif(rng);
		}

		BWAPI::UnitType getRandomType(Composition composition)
		{
			int randomIndex = rand() % composition.getTypes().size();
			return composition.getTypes()[randomIndex];
		}

		BWAPI::UnitType getRandomType(BWAPI::UnitType macroType = BWAPI::UnitTypes::AllUnits)
		{
			if (!BWAPI::UnitTypes::allMacroTypes().contains(macroType))
				return macroType;

			std::vector<BWAPI::UnitType> unitTypeBag;

			for each(auto unitType in BWAPI::UnitTypes::allUnitTypes())
			{
				if (unitType.getRace() != util::game::getSelf()->getRace()
					|| unitType.isHero()
					|| unitType == BWAPI::UnitTypes::Terran_Vulture_Spider_Mine
					|| unitType == BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode
					|| unitType.isWorker())
					continue;

				if (macroType == BWAPI::UnitTypes::Men && unitType.isBuilding())
					continue;

				if (macroType == BWAPI::UnitTypes::Buildings && !unitType.isBuilding())
					continue;

				unitTypeBag.push_back(unitType);
			}

			int randomIndex = rand() % unitTypeBag.size();
			return unitTypeBag[randomIndex];
		}
	}

	namespace game
	{
		void setSelf()
		{
			if (!BWAPI::Broodwar->isReplay())
			{
				self = BWAPI::Broodwar->self();
				selfName = self->getName();
			}
			else
			{
				self = nullptr;
				selfName = getSelfName();
				auto playerSet = BWAPI::Broodwar->getPlayers();
				for each(auto player in playerSet)
				{
					if (player->getName().compare(selfName) == 0)
					{
						self = player;
						break;
					}
				}
			}
		}

		void setStartLocation()
		{
			playerStartLocation = self->getStartLocation();
		}

		void setEnemy()
		{
			if (!BWAPI::Broodwar->isReplay())
			{
				enemy = BWAPI::Broodwar->enemy();
			}
			else
			{
				auto playerSet = BWAPI::Broodwar->getPlayers();
				enemySet.clear();
				for each(auto player in playerSet)
				{
					if (player->getName().compare(selfName) != 0 && !player->isNeutral())
						enemySet.insert(player);
				}

				enemy = (*enemySet.begin());
			}
		}		

		BWAPI::Playerset& getEnemies()
		{
			return enemySet;
		}

		BWAPI::Player getEnemy()
		{
			return enemy;
		}

		BWAPI::Player getSelf()
		{
			return self;
		}

		std::string getSelfName()
		{
			std::ifstream nameFile;
			nameFile.open("ai-data//Player.txt");
			std::string selfName;
			getline(nameFile, selfName);
			nameFile.close();

			return selfName;
		}

		BWAPI::TilePosition getStartLocation()
		{
			return playerStartLocation;
		}

		bool canMakeUnit(BWAPI::UnitType unitType)
		{
			bool hasUnitTypeRequirement = true;
			bool hasResearched = !self->hasResearched(unitType.requiredTech());
			bool hasCreator = self->hasUnitTypeRequirement(unitType.whatBuilds().first, unitType.whatBuilds().second);

			for each(auto& requirement in unitType.requiredUnits())
			{
				if (!self->hasUnitTypeRequirement(requirement.first, requirement.second))
					hasUnitTypeRequirement = false;
			}


			return hasUnitTypeRequirement && hasResearched && hasCreator;
		}

		bool canResearch(BWAPI::TechType techType)
		{
			bool hasRequiredUnit = self->hasUnitTypeRequirement(techType.requiredUnit());
			bool hasResearcher = self->hasUnitTypeRequirement(techType.whatResearches());

			return hasRequiredUnit && hasResearcher;
		}

		int getFrameBracket(int frame, int bracket)
		{
			return int(std::floor((frame) / bracket) * bracket);
		}		
	}

	namespace data
	{
		void getFiles(const fs::path& root, const std::string& ext, std::vector<fs::path>& ret)
		{
			if (!fs::exists(root) || !fs::is_directory(root)) return;

			fs::recursive_directory_iterator it(root);
			fs::recursive_directory_iterator endit;

			while (it != endit)
			{
				if (fs::is_regular_file(*it) && it->path().extension() == ext)
					ret.push_back(it->path().filename());
				++it;
			}
		}

		void serializeComposition(CompositionHelper::UsedComposition usedComposition)
		{
			std::ofstream compStream;

			auto directory = "ai-data\\" + game::getEnemy()->getRace().getName() + "\\compositions\\";
			int avgActivationFrame = usedComposition.activationFrame;
			if (usedComposition.useCount > 0)
				avgActivationFrame = (int)((double)usedComposition.activationFrame / usedComposition.useCount);
			int frameBracket = game::getFrameBracket(avgActivationFrame, 7200);
			auto fileName = std::to_string(frameBracket) + "_" + std::to_string(usedComposition.taskType);

			compStream.open(directory + fileName + ".comp", std::ios::binary | std::ios::out);

			int taskType = usedComposition.taskType;
			
			double fitness = usedComposition.fitness;
			if (usedComposition.useCount > 0)
				fitness = usedComposition.fitness / usedComposition.useCount;

			int useCount = usedComposition.useCount;

			compStream.write((char*)&taskType, sizeof(taskType));
			compStream.write((char*)&avgActivationFrame, sizeof(avgActivationFrame));
			compStream.write((char*)&fitness, sizeof(fitness));

			int totalTypeCount = 0;			
			for each(auto unitType in usedComposition.composition.getTypes())
			{
				if (usedComposition.composition[unitType] > 0)
					totalTypeCount++;
			}

			compStream.write((char*)&totalTypeCount, sizeof(totalTypeCount));

			for each(auto unitType in usedComposition.composition.getTypes())
			{
				if (usedComposition.composition[unitType] > 0)
				{
					int typeID = unitType.getID();
					int unitTypeCount = (int)((double)(usedComposition.composition[unitType]));
					compStream.write((char*)&typeID, sizeof(typeID));
					compStream.write((char*)&unitTypeCount, sizeof(unitTypeCount));
				}
			}
			compStream.write((char*)&useCount, sizeof(useCount));

			compStream.close();

			std::ofstream readableStream;

			readableStream.open(directory + "\\human\\" + fileName + ".txt", std::ios::out);

			readableStream << "TaskType: " << taskType << "\n";
			readableStream << "Average Activation Time: " << avgActivationFrame << "\n";
			readableStream << "Average Fitness: " << fitness << "\n";
			readableStream << "UseCount " << useCount << "\n";

			for each(auto unitType in usedComposition.composition.getTypes())
			{				
				int unitTypeCount = (int)((double)(usedComposition.composition[unitType]));
				readableStream << unitType.c_str() << " : " << unitTypeCount << "\n";
			}

			readableStream.close();
		}

		CompositionHelper::UsedComposition deserializeComposition(std::string target)
		{
			std::ifstream compStream;
			compStream.open(target, std::ios::binary | std::ios::in);

			Composition c;
			int taskType;
			int activationFrame;
			double fitness;
			int useCount;

			compStream.read((char*)&taskType, sizeof(taskType));
			compStream.read((char*)&activationFrame, sizeof(activationFrame));
			compStream.read((char*)&fitness, sizeof(fitness));			

			int totalTypeCount;
			compStream.read((char*)&totalTypeCount, sizeof(totalTypeCount));

			for (int i = 0; i < totalTypeCount; i++)
			{
				int unitType;
				int unitTypeCount;
				compStream.read((char*)&unitType, sizeof(unitType));
				compStream.read((char*)&unitTypeCount, sizeof(unitTypeCount));
				c.addType(unitType, unitTypeCount);
			}

			compStream.read((char*)&useCount, sizeof(useCount));

			compStream.close();

			CompositionHelper::UsedComposition usedComposition{
				c,
				TaskType(taskType),
				activationFrame,
				fitness,
				useCount
			};

			return usedComposition;
		}		

		void serializeMapData(MapHelper::Field regionField)
		{
			std::ofstream dataStream;

			auto directory = "ai-data\\" + game::getEnemy()->getRace().getName() + "\\maps\\";
			auto fileName = BWAPI::Broodwar->mapHash();
			auto target = directory + fileName;

			dataStream.open(target, std::ios::binary | std::ios::out);			

			for (auto &zone : regionField)
			{				
				int zoneID = zone->getID();
				int defenseCount = zone->getTimesDefended();

				dataStream.write((char*)&zoneID, sizeof(zoneID));
				dataStream.write((char*)&defenseCount, sizeof(defenseCount));

				//std::cout << "ZoneID : " << zoneID << " | dCount : " << defenseCount << "\n";
			}			

			dataStream.close();			
		}

		std::map<int, int> deserializeMapData()
		{	
			auto directory = "ai-data\\" + game::getEnemy()->getRace().getName() + "\\maps\\";
			auto fileName = BWAPI::Broodwar->mapHash();
			auto target = directory + fileName;

			std::map<int, int> zoneDefenseMap;

			if (!fs::exists(target))
				return zoneDefenseMap;

			std::ifstream dataStream;
			dataStream.open(target, std::ios::binary | std::ios::in);
			
			int regionCount = BWAPI::Broodwar->getAllRegions().size();

			for (int i = 0; i < regionCount; i++)
			{				
				int zoneID;
				int defenseCount;

				dataStream.read((char*)&zoneID, sizeof(zoneID));
				dataStream.read((char*)&defenseCount, sizeof(defenseCount));

				zoneDefenseMap[zoneID] = defenseCount;
			}

			dataStream.close();

			return zoneDefenseMap;
		}
	}
}