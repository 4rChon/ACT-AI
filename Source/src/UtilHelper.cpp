#include "UtilHelper.h"
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
		setSelf();
		setEnemy();
		std::cout << selfName << " (" << self->getRace().getName() << ") vs " << enemy->getName() << " (" << enemy->getRace().getName() << ")\n";
	}

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

	double normaliseValues(std::vector<double> valueArr, std::vector<double> coeffArr)
	{
		double total = 0.0;
		for (std::size_t i = 0; i < valueArr.size(); i++)
			total += 1 / (1 + std::exp(-coeffArr[i] * valueArr[i]));

		return ((2 / valueArr.size()) * total) - 1;
	}

	void getFiles(const fs::path& root, const std::string& ext, std::vector<fs::path>& ret)
	{
		if (!fs::exists(root) || !fs::is_directory(root)) return;

		fs::recursive_directory_iterator it(root);
		fs::recursive_directory_iterator endit;

		while (it != endit)
		{
			if(fs::is_regular_file(*it) && it->path().extension() == ext)
				ret.push_back(it->path().filename());
			++it;
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
}