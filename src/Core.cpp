#include "Core.h"
#include "Worker.h"
#include "Producer.h"
#include "AgentManager.h"

using namespace BWAPI;
using namespace Filter;

void updateTaskTree(Task* task)
{	
	if (task->getSubTasks().size() > 0)
		for (auto t : task->getSubTasks())
			updateTaskTree(t);
	task->assign();
	task->update();
}

void initSatisifed()
{
	std::cout << "Initialising Satisfied Types\n";
	for each(BWAPI::UnitType t in BWAPI::UnitTypes::allUnitTypes())
		g_isUnlocked[t] = false;
}

void updateSatisfied()
{
	for (auto unit : g_TotalCount)
		if (unit.second > 0)
			for (auto buildsThis : unit.first.buildsWhat())
				for (auto requiresThis : buildsThis.requiredUnits())
				{
					if (g_TotalCount[requiresThis.first] == 0)
					{
						g_isUnlocked[buildsThis] = false;
						break;
					}
					g_isUnlocked[buildsThis] = true;
				}

	if (Broodwar->self()->getRace() == Races::Zerg)
		g_isUnlocked[UnitTypes::Zerg_Larva] = true;

	AgentManager::getInstance()->updateUnlocked();
}

void Core::onStart()
{	
	std::cout << "\n------------ MATCH STARTED --------------\n";	

	initSatisifed();
	updateSatisfied();

	AgentManager::getInstance();

	drawGui = false;

	threatField = new ThreatField(Broodwar->getAllRegions());	
	
	defend = new Defend(threatField->getZone(140));
	attack = nullptr;

	Broodwar->sendText("gl hf");
	Broodwar << "The map is " << Broodwar->mapName() << "!" << std::endl;


	Broodwar->enableFlag(Flag::UserInput);
	
	// Uncomment the following line and the bot will know about everything through the fog of war (cheat).
	//Broodwar->enableFlag(Flag::CompleteMapInformation);

	Broodwar->setCommandOptimizationLevel(1);

	if (Broodwar->isReplay())
	{
		Broodwar << "The following players are in this replay:" << std::endl;

		Playerset players = Broodwar->getPlayers();
		for (auto p : players)
			if (!p->isObserver())
				Broodwar << p->getName() << ", playing as " << p->getRace() << std::endl;

	}
	else
		if (Broodwar->enemy())
			Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;

}

void Core::onEnd(bool isWinner)
{
	delete threatField;
	delete attack;
	if (isWinner)
	{
	}

	std::cout << "\n------------  MATCH ENDED  --------------\n\n";

}

void Core::onFrame()
{	
	//display text with information at the top of the screen
	drawTextInfo();

	if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
		return;	

	if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
		return;

	if (drawGui)
		drawRegions();
	
	std::unordered_set<Agent*>::iterator agent = AgentManager::getInstance()->getAgentset().begin();
	while (agent != AgentManager::getInstance()->getAgentset().end())
	{		
		if (!(*agent)->getUnit()->exists())
		{			
			agent = AgentManager::getInstance()->removeAgent(agent);
			continue;
		}

		if ((*agent)->getUnit()->isLockedDown() || (*agent)->getUnit()->isMaelstrommed() || (*agent)->getUnit()->isStasised())
		{
			++agent;
			continue;
		}

		if ((*agent)->getUnit()->isLoaded() || !(*agent)->getUnit()->isPowered() || (*agent)->getUnit()->isStuck())
		{
			++agent;
			continue;
		}

		if (!(*agent)->getUnit()->isCompleted() || (*agent)->getUnit()->isConstructing())
		{
			++agent;
			continue;
		}
		
		if ((*agent)->isFree())
		{			
			std::unordered_set<Coalition*>::iterator coalition = g_OpenCoalitions.begin();
			while (coalition != g_OpenCoalitions.end())
			{
				(*coalition)->addAgent((*agent)); //probability parameter
				if ((*coalition)->isActive())
				{
					coalition = g_OpenCoalitions.erase(coalition);
					break;
				}
				++coalition;
			}
			(*agent)->act();
		}
		
		threatField->getZone((*agent)->getUnit()->getRegion()->getID())->updateZone();

		++agent;
	}

	//AgentManager::getInstance()->act();
	
	for (auto coalition : g_Coalitions)
		coalition->updateFreeAgents();	


	if (attack != nullptr)
	{
		if (attack->isComplete())
		{
			delete attack;
			attack = nullptr;
			defend = new Defend(threatField->getZone(135));
		}
		else
			updateTaskTree(attack);
	}		

	if (defend != nullptr)
	{
		if (defend->isComplete())
		{
			delete defend;
			defend = nullptr;
			attack = new Attack(threatField->getZone(20));

		}
		else updateTaskTree(defend);
	}
}

void Core::onSendText(std::string text)
{
	if (text == "/draw gui")
	{
		drawGui = !drawGui;
		return;
	}

	Broodwar->sendText("%s", text.c_str());
}

void Core::onReceiveText(BWAPI::Player player, std::string text)
{
	Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void Core::onPlayerLeft(BWAPI::Player player)
{
	Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void Core::onNukeDetect(BWAPI::Position target)
{
	if (target)
		Broodwar << "Nuclear Launch Detected at " << target << std::endl;
	else
		Broodwar->sendText("Where's the nuke?");

}

void Core::onUnitDiscover(BWAPI::Unit unit)
{
}

void Core::onUnitEvade(BWAPI::Unit unit)
{
}

void Core::onUnitShow(BWAPI::Unit unit)
{

}

void Core::onUnitHide(BWAPI::Unit unit)
{
}

void Core::onUnitCreate(BWAPI::Unit unit)
{	
}

void Core::onUnitDestroy(BWAPI::Unit unit)
{	
	if (unit->getPlayer() == Broodwar->self())
	{
		g_TotalCount[unit->getType()]--;
		AgentManager::getInstance()->removeAgent(unit);
	}
	updateSatisfied();
}

void Core::onUnitMorph(BWAPI::Unit unit)
{
	if (Broodwar->isReplay())
		if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral())
		{
			int seconds = Broodwar->getFrameCount() / 24;
			int minutes = seconds / 60;
			seconds %= 60;
			Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
		}
}

void Core::onUnitRenegade(BWAPI::Unit unit)
{
}

void Core::onSaveGame(std::string gameName)
{
	Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void Core::onUnitComplete(BWAPI::Unit unit)
{		
	if (unit->getPlayer() == Broodwar->self())
	{
		AgentManager::getInstance()->addAgent(unit);
		g_TotalCount[unit->getType()]++;
		std::cout << g_TotalCount[unit->getType()];
	}
	updateSatisfied();
	g_Supply = (1 / (double)((BWAPI::Broodwar->self()->supplyTotal() - BWAPI::Broodwar->self()->supplyUsed()) + 1));
}

void Core::drawRegions()
{
	for (int i = 0; i < threatField->getSize(); i++)
	{
		Color c = Color(255, 0, 0);
		if (threatField->getZone(i)->getRegion()->getUnits(Filter::IsOwned).size() > 0)
			c = Color(0, 255, 0);
		Broodwar->drawCircle(CoordinateType::Map, threatField->getZone(i)->getRegion()->getCenter().x, threatField->getZone(i)->getRegion()->getCenter().y, 2, c, true);

		std::ostringstream oss;
		oss << "getRegion() ID: " << threatField->getZone(i)->getRegion()->getID()
			<< "\nZone ID: " << i
			<< "\nEnemy Score: " << threatField->getZone(i)->getEnemyScore()
			<< "\nFriend Score: " << threatField->getZone(i)->getFriendScore()
			<< "\nResource Score: " << threatField->getZone(i)->getResourceScore()
			//<< "\nConfidence Value " << threatField->getZone(i)->getConfidence()
			<< "\nLast Visit " << threatField->getZone(i)->getLastVisit()
			<< "\nPosition: " << "(" << threatField->getZone(i)->getRegion()->getCenter().x << ", " << threatField->getZone(i)->getRegion()->getCenter().y << ")"
			<< "\nDefense Priority: " << threatField->getZone(i)->getRegion()->getDefensePriority()
			<< std::endl;
		std::string s = oss.str();

		Broodwar->drawTextMap(
			threatField->getZone(i)->getRegion()->getCenter().x, threatField->getZone(i)->getRegion()->getCenter().y,
			s.c_str()
			);
	}
}

void Core::drawTextInfo()
{
	Broodwar->drawTextScreen(200, 0, "FPS: %d", Broodwar->getFPS());
	Broodwar->drawTextScreen(200, 10, "Average FPS: %f", Broodwar->getAverageFPS());
	Broodwar->drawTextScreen(200, 30, "Active Tasks: %d", g_Tasks.size());
	Broodwar->drawTextScreen(200, 40, "Coalition Count: %d", g_Coalitions.size());
	Broodwar->drawTextScreen(200, 50, "Open Coalition Count: %d", g_OpenCoalitions.size());
	Broodwar->drawTextScreen(200, 60, "Agent Count: %d", AgentManager::getInstance()->getAgentset().size());
	Broodwar->drawTextScreen(200, 70, "Free Agent Count: %d", AgentManager::getInstance()->getFreeCount());
	Broodwar->drawTextScreen(200, 80, "Supply Desire: %.5f", g_Supply);
	Broodwar->drawTextScreen(200, 90, "Mineral Reserve: %.5f", g_MinReserve);
	Broodwar->drawTextScreen(200, 100, "Gas Reserve: %.5f", g_GasReserve);

	for (auto coalition : g_Coalitions)
		if (coalition->isActive())
			for (auto unit : coalition->getUnitSet())
			{
				if (!unit->exists())
					coalition->removeUnit(unit);

				Broodwar->drawTextMap(unit->getPosition(), coalition->getCurrentTaskString().c_str());
			}
}