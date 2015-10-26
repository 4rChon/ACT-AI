#include "Core.h"

using namespace BWAPI;
using namespace Filter;

void updateTaskTree(Task* task)
{	
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

}

void Core::onStart()
{
	std::cout << "\n------------ MATCH STARTED --------------\n";
	initSatisifed();
	updateSatisfied();

	drawGui = false;

	threatField = new ThreatField(Broodwar->getAllRegions());
	attack = new Attack(threatField->getZone(10));

	//g_Tasks.push_back(attack);
	//attackComp.addType(Broodwar->self()->getRace().getWorker(), 1);	

	//init wrappers

	Broodwar->sendText("gl hf");

	// Print the map name.
	Broodwar << "The map is " << Broodwar->mapName() << "!" << std::endl;

	// Enable the UserInput flag, which allows us to control the bot and type messages.
	Broodwar->enableFlag(Flag::UserInput);
	// Uncomment the following line and the bot will know about everything through the fog of war (cheat).
	//Broodwar->enableFlag(Flag::CompleteMapInformation);

	// Set the command optimization level so that common commands can be grouped
	// and reduce the bot's APM (Actions Per Minute).
	Broodwar->setCommandOptimizationLevel(1);

	// Check if this is a replay
	if (Broodwar->isReplay())
	{

		// Announce the players in the replay
		Broodwar << "The following players are in this replay:" << std::endl;

		// Iterate all the players in the game using a std:: iterator
		Playerset players = Broodwar->getPlayers();
		for (auto p : players)
		{
			// Only print the player if they are not an observer
			if (!p->isObserver())
				Broodwar << p->getName() << ", playing as " << p->getRace() << std::endl;
		}

	}
	else // if this is not a replay
	{
		// Retrieve you and your enemy's races. enemy() will just return the first enemy.
		// If you wish to deal with multiple enemies then you must use enemies().
		if (Broodwar->enemy()) // First make sure there is an enemy
			Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;
	}
}

void Core::onEnd(bool isWinner)
{
	delete threatField;
	// Called when the game ends*/
	if (isWinner)
	{
		// Log your win here!
	}

	std::cout << "\n------------  MATCH ENDED  --------------\n\n";

}

void Core::onFrame()
{	
	// Called once every game frame
	if (drawGui)
		drawRegions();
	// Display the game frame rate as text in the upper left area of the screen
	Broodwar->drawTextScreen(200, 0, "FPS: %d", Broodwar->getFPS());
	Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS());
	Broodwar->drawTextScreen(200, 40, "Active Tasks: %d", g_Tasks.size());
	for (auto coalition : g_Coalitions)
		for (auto unit : coalition->getUnitSet())
			Broodwar->drawTextMap(unit->getPosition(), coalition->getCurrentTaskString().c_str());

	// Return if the game is a replay or is paused
	if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
		return;

	// Prevent spamming by only running our onFrame once every number of latency frames.
	// Latency frames are the number of frames before commands are processed.
	if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
		return;

	// Iterate through all the units that we own
	std::unordered_set<Agent*>::iterator agent = g_Agents.begin();
	while (agent != g_Agents.end())
	{
		if (g_FreeAgents.find(*agent) == g_FreeAgents.end())
		{
			g_FreeAgents.insert(*agent);
			(*agent)->getCoalition()->removeUnit((*agent)->getUnit());
		}
			
		// Ignore the unit if it no longer exists
		if (!(*agent)->getUnit()->exists())
		{
			agent = g_Agents.erase(agent);
			continue;
		}

		// Ignore the unit if it has one of the following status ailments
		if ((*agent)->getUnit()->isLockedDown() || (*agent)->getUnit()->isMaelstrommed() || (*agent)->getUnit()->isStasised())
		{
			++agent;
			continue;
		}

		// Ignore the unit if it is in one of the following states
		if ((*agent)->getUnit()->isLoaded() || !(*agent)->getUnit()->isPowered() || (*agent)->getUnit()->isStuck())
		{
			++agent;
			continue;
		}

		// Ignore the unit if it is incomplete or busy constructing
		if (!(*agent)->getUnit()->isCompleted() || (*agent)->getUnit()->isConstructing())
		{
			++agent;
			continue;
		}

		if ((*agent)->getUnit()->isIdle() && g_FreeAgents.count((*agent)))
		{
			std::cout << (*agent)->getUnit()->getType().c_str() << " is joining a coalition.....\n";
			for (auto coalition : g_OpenCoalitions)
			{
				(*agent)->setCoalition(coalition);
				if(coalition->isActive())
					g_OpenCoalitions.erase(coalition);
				break;
			}
		}

		threatField->getZone((*agent)->getUnit()->getRegion()->getID())->updateZone();
		++agent;
	}

//	std::cout << "iter-   --------     ------     ----------------\n";
	updateTaskTree(attack);
//	tempBool++;
	//} // closure: unit iterator
}

void Core::onSendText(std::string text)
{
	if (text == "/draw gui")
	{
		drawGui = !drawGui;
		return;
	}
	// Send the text to the game if it is not being processed.	
	Broodwar->sendText("%s", text.c_str());
	// Make sure to use %s and pass the text as a parameter,
	// otherwise you may run into problems when you use the %(percent) character!
}

void Core::onReceiveText(BWAPI::Player player, std::string text)
{
	// Parse the received text
	Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void Core::onPlayerLeft(BWAPI::Player player)
{
	// Interact verbally with the other players in the game by
	// announcing that the other player has left.
	Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void Core::onNukeDetect(BWAPI::Position target)
{
	// Check if the target is a valid position
	if (target)
	{
		// if so, print the location of the nuclear strike target
		Broodwar << "Nuclear Launch Detected at " << target << std::endl;
	}
	else
	{
		// Otherwise, ask other players where the nuke is!
		Broodwar->sendText("Where's the nuke?");
	}

	// You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
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
	updateSatisfied();
}

void Core::onUnitMorph(BWAPI::Unit unit)
{
	if (Broodwar->isReplay())
	{
		// if we are in a replay, then we will print out the build order of the structures
		if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral())
		{
			int seconds = Broodwar->getFrameCount() / 24;
			int minutes = seconds / 60;
			seconds %= 60;
			Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
		}
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
		Agent* agent = new Agent(unit);
		g_Agents.insert(agent);
		g_FreeAgents.insert(agent);
		g_TotalCount[unit->getType()]++;
	}
	updateSatisfied();
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