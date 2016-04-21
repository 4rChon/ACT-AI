#include "UtilHelper.h"
#include "CompositionHelper.h"
#include "Composition.h"
#include <string>
#include <vector>

namespace util
{			
	namespace
	{
		static std::string selfName;
		static BWAPI::Player self;
		static BWAPI::Player enemy;
		static BWAPI::Playerset enemySet;
	}

	void initialiseUtil()
	{
		game::setSelf();
		game::setEnemy();
		std::cout << selfName << " (" << self->getRace().getName() << ") vs " << enemy->getName() << " (" << enemy->getRace().getName() << ")\n";
		data::loadMapData();
	}	

	double normaliseValues(std::vector<double> valueArr, std::vector<double> coeffArr)
	{
		double total = 0.0;
		for (std::size_t i = 0; i < valueArr.size(); i++)
			total += 1 / (1 + std::exp(-coeffArr[i] * valueArr[i]));

		return ((2 / valueArr.size()) * total) - 1;
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
			nameFile.open("compositions\\Player.txt");
			std::string selfName;
			getline(nameFile, selfName);
			nameFile.close();

			return selfName;
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

			auto directory = "compositions\\" + game::getEnemy()->getRace().getName() + "\\";
			int avgActivationFrame = (int)((double)usedComposition.activationFrame / usedComposition.useCount);
			int frameBracket = game::getFrameBracket(avgActivationFrame, 7200);
			auto fileName = std::to_string(frameBracket) + "_" + std::to_string(usedComposition.taskType);

			compStream.open(directory + fileName + ".comp", std::ios::binary | std::ios::out);

			int taskType = usedComposition.taskType;
			
			double fitness = usedComposition.fitness / usedComposition.useCount;
			int useCount = usedComposition.useCount;

			compStream.write((char*)&taskType, sizeof(taskType));
			compStream.write((char*)&avgActivationFrame, sizeof(avgActivationFrame));
			compStream.write((char*)&fitness, sizeof(fitness));

			int typeCount = 0;			
			for each(auto unitType in usedComposition.composition.getTypes())
			{
				if (usedComposition.composition[unitType] > 0)
					typeCount++;
			}

			compStream.write((char*)&typeCount, sizeof(typeCount));

			for each(auto unitType in usedComposition.composition.getTypes())
			{
				if (usedComposition.composition[unitType] > 0)
				{
					int typeID = unitType.getID();
					int typeCount = (int)((double)(usedComposition.composition[unitType]));
					compStream.write((char*)&typeID, sizeof(typeID));
					compStream.write((char*)&typeCount, sizeof(typeCount));
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
				int typeCount = (int)((double)(usedComposition.composition[unitType]));
				readableStream << unitType.c_str() << " : " << typeCount << "\n";
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

		void loadMapData()
		{
			BWAPI::Broodwar->mapHash();
		}
	}
}