#include "swarmCAT.h"
#include "AgentHelper.h"
#include "ArmyHelper.h"
#include "Attack.h"
#include "CoalitionHelper.h"
#include "CompositionHelper.h"
#include "CreateUnit.h"
#include "Composition.h"
#include "DesireHelper.h"
#include "EconHelper.h"
#include "MapHelper.h"
#include "ResourceDepot.h"
#include "ResearchTech.h"
#include "ReplayHelper.h"
#include "Scout.h"
#include "TaskHelper.h"
#include "UtilHelper.h"
#include "Zone.h"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <cmath> /* ceil */

using namespace BWAPI;
using namespace Filter;

bool analyzed = false;
auto currentTime = std::chrono::high_resolution_clock::now();

DWORD WINAPI AnalyzeThread()
{
	BWTA::analyze();

	analyzed = true;
	return 0;
}

void SwarmCAT::onStart()
{	

	std::cout << " ---------------- MATCH START ----------------\n";
	Broodwar << "The map is " << Broodwar->mapName() << "!" << std::endl;

	Broodwar->setLocalSpeed(0);
	//Broodwar->setGUI(false);
	Broodwar->enableFlag(Flag::UserInput);

	//Broodwar->enableFlag(Flag::CompleteMapInformation);

	Broodwar->setCommandOptimizationLevel(1);	

	util::initialiseUtil();
	CompositionHelper::initialiseHelper();
	
	if (Broodwar->isReplay())
	{
		Broodwar << "The following players are in this replay:" << std::endl;

		Playerset players = Broodwar->getPlayers();
		for (auto &p : players)
			if (!p->isObserver())
				Broodwar << p->getName() << ", playing as " << p->getRace() << std::endl;

		ReplayHelper::initialiseHelper();
	}
	else
	{
		if (Broodwar->enemy())
			Broodwar << "The matchup is " << util::game::getSelf()->getRace() << " vs " << util::game::getEnemy()->getRace() << std::endl;

		BWTA::readMap();
		AnalyzeThread();
		
		CoalitionHelper::initialiseHelper();
		MapHelper::initialiseHelper();
		AgentHelper::initialiseHelper();
		EconHelper::initialiseHelper();
		DesireHelper::initialiseHelper();
		TaskHelper::initialiseHelper();
		ArmyHelper::initialiseHelper();
		
	}
}

void SwarmCAT::onEnd(bool isWinner)
{
	CompositionHelper::saveCompositions();
	if (!BWAPI::Broodwar->isReplay())
	{
		MapHelper::saveMapData();
		util::eval::logCompositionFitness();
		util::eval::logResult(isWinner);
	}

	/*TaskHelper::deleteTaskTree(TaskHelper::getRootTasks());
	for(auto &coalition : CoalitionHelper::getCoalitionset())
		CoalitionHelper::removeCoalition(coalition);*/
	std::cout << " ---------------- MATCH END ----------------\n";
}

void SwarmCAT::onFrame()
{
	if (!Broodwar->isReplay())
	{
		if (BWAPI::Broodwar->getFrameCount() % (24 * 60) == 0)
		{
			auto player = util::game::getSelf();
			util::eval::logIncome((player->gatheredMinerals() + player->gatheredGas()) - (player->spentGas() + player->spentMinerals()));
		}
		//std::cout << "---FrameStart---\n";	
		//drawDebugText();
		//Cancel unfinished structures if not being constructed by an scv.
		if (util::game::getSelf()->incompleteUnitCount(BWAPI::UnitTypes::Buildings))
		{			
			auto unfinishedUnits = BWAPI::Broodwar->getUnitsInRectangle(Position(0, 0), Position(BWAPI::Broodwar->mapWidth() * TILE_SIZE, BWAPI::Broodwar->mapHeight() * TILE_SIZE), BWAPI::Filter::IsBuilding && !BWAPI::Filter::IsBeingConstructed && !BWAPI::Filter::IsCompleted);
			unfinishedUnits.cancelConstruction();
		}

		currentTime = std::chrono::high_resolution_clock::now();

		if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
			return;

		if (analyzed)
			drawTerrainData();

		if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
			return;

		auto &a = AgentHelper::getLastServiced();
		while (a != AgentHelper::getAgentset().end())
		{
			auto u = (*a)->getUnit();

			if (!u->exists())
			{
				AgentHelper::setLastServiced(++a);
				continue;
			}

			MapHelper::getZone(BWAPI::Broodwar->getRegionAt(u->getPosition()))->updateZone();

			if (u->isLockedDown() || u->isMaelstrommed() || u->isStasised())
			{
				AgentHelper::setLastServiced(++a);
				continue;
			}

			if (u->isLoaded() || !u->isPowered() || u->isStuck())
			{
				AgentHelper::setLastServiced(++a);
				continue;
			}

			if (!u->isCompleted() || u->isConstructing())
			{
				AgentHelper::setLastServiced(++a);
				continue;
			}

			(*a)->act();			

			AgentHelper::setLastServiced(++a);

			auto diff = std::chrono::high_resolution_clock::now() - currentTime;

			if (diff.count() > 0.015)
				break;
		}

		DesireHelper::updateDesireMaps();
		TaskHelper::updateRootTasks();
		EconHelper::updateEconomy();
		ArmyHelper::updateArmyMovement();		

		if (Broodwar->getFrameCount() % (24 * 30) == 0)
			DesireHelper::updateSupplyDesire();
		//std::cout << "---FrameEnd---\n";
	}
	else
	{
		ReplayHelper::evolveFlaggedCompositions();
	}
}

void SwarmCAT::onSendText(std::string text)
{
	Broodwar->sendText("%s", text.c_str());
	if (text.compare(0, 7, "Terran_") == 0)
	{
		for each(auto unitType in BWAPI::UnitTypes::allUnitTypes())
		{
			if (unitType.getName().compare(text) == 0)
			{
				CreateUnit* createUnit = new CreateUnit(unitType, 1);
				//createUnit->setDebug(true);
				TaskHelper::addTask(createUnit, true);
				break;
			}
		}
	}
	if (text.compare(0, 17, "print composition") == 0)
	{
		//if (text.length() <= 18)
		//	return;

		std::string number = text.substr(18);
		char * pEnd;
		int id = (int)std::strtol(number.c_str(), &pEnd, 10);
		std::cout << "Printing Composition " << id << "\n";		
		auto task = TaskHelper::getTask(id);
		if (task)
			std::cout << task->getCoalition()->getTargetComp().toString() << "\n";
		else
			std::cout << "No composition found\n";
	}
}

void SwarmCAT::onReceiveText(BWAPI::Player player, std::string text)
{
	Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void SwarmCAT::onPlayerLeft(BWAPI::Player player)
{
	Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void SwarmCAT::onNukeDetect(BWAPI::Position target)
{

	if (target)
		Broodwar << "Nuclear Launch Detected at " << target << std::endl;
	else
		Broodwar->sendText("Where's the nuke?");
}

void SwarmCAT::onUnitDiscover(BWAPI::Unit unit)
{
	if (!Broodwar->isReplay())
	{
		if (unit->getType().isBuilding() && unit->getPlayer() == util::game::getEnemy())
			ArmyHelper::addTargetPriority(unit);
	}
}

void SwarmCAT::onUnitEvade(BWAPI::Unit unit)
{
}

void SwarmCAT::onUnitShow(BWAPI::Unit unit)
{
	if (!Broodwar->isReplay())
	{
		if (unit->getPlayer() == util::game::getEnemy())
			ArmyHelper::addScoutedUnit(unit->getID(), unit->getType());
	}
}

void SwarmCAT::onUnitHide(BWAPI::Unit unit)
{	
}

void SwarmCAT::onUnitCreate(BWAPI::Unit unit)
{
	if (!Broodwar->isReplay())
	{
		if (unit->getPlayer() == util::game::getSelf() && !(unit->getType().supplyProvided() == 16 || unit->getType().isBuilding()))
			DesireHelper::updateSupplyDesire(unit->getType());

		if (Broodwar->isReplay())
		{
			// if we are in a replay, then we will print out the build order of the structures
			if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral())
			{
				int seconds = Broodwar->getFrameCount() / 24;
				int minutes = seconds / 60;
				seconds %= 60;
				Broodwar->sendText("%.2d:%.2d: %s creates a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
			}
		}
	}
}

void SwarmCAT::onUnitDestroy(BWAPI::Unit unit)
{	
	if (!Broodwar->isReplay())
	{
		if (unit->getPlayer() == util::game::getSelf() && !unit->isBeingConstructed())
		{
			if (unit->getType() == BWAPI::UnitTypes::Terran_Bunker)
			{
				int bunkerCount = 0;
				const MapHelper::Field& neighbourhood = MapHelper::getZone(unit->getRegion())->getNeighbourhood();
				for (const auto &zone : neighbourhood)
					bunkerCount += zone->getRegion()->getUnits(BWAPI::Filter::IsOwned && BWAPI::Filter::GetType == BWAPI::UnitTypes::Terran_Bunker).size();

				if (bunkerCount == 0)
				{
					for (const auto &zone : neighbourhood)
						zone->setBunkerDefense(false);
				}
			}

			if (unit->getType() == BWAPI::UnitTypes::Terran_Missile_Turret)
			{
				int turretCount = 0;
				for (auto &zone : MapHelper::getZone(unit->getRegion())->getNeighbourhood())
					turretCount += zone->getRegion()->getUnits(BWAPI::Filter::IsOwned && BWAPI::Filter::GetType == BWAPI::UnitTypes::Terran_Missile_Turret).size();

				if (turretCount == 0)
				{
					for (auto &zone : MapHelper::getZone(unit->getRegion())->getNeighbourhood())
						zone->setTurretDefense(false);
				}
			}

			DesireHelper::updateSupplyDesire(unit->getType(), true);
			AgentHelper::removeAgent(unit->getID());
			
		}

		if (unit->getPlayer() == util::game::getEnemy())
		{
			if (unit->getType().isBuilding())
				ArmyHelper::removeTargetPriority(unit);
			else
				ArmyHelper::removeScoutedUnit(unit->getID());
		}
	}
}

void SwarmCAT::onUnitMorph(BWAPI::Unit unit)
{
	if (!Broodwar->isReplay())
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
}

void SwarmCAT::onUnitRenegade(BWAPI::Unit unit)
{
}

void SwarmCAT::onSaveGame(std::string gameName)
{
	Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void SwarmCAT::onUnitComplete(BWAPI::Unit unit)
{
	if (!Broodwar->isReplay())
	{
		if (unit->getPlayer() == util::game::getSelf())
		{			
			if (unit->getType().isResourceDepot())
			{
				DesireHelper::updateSupplyDesire(unit->getType());
				int unitCount = util::game::getSelf()->allUnitCount(unit->getType());
				if( unitCount > 1)
					util::eval::logExpansion(unitCount -1);
			}
			AgentHelper::createAgent(unit);

			auto unitZone = MapHelper::getZone(unit->getRegion()); 			

			if (unit->getType() == BWAPI::UnitTypes::Terran_Bunker)
			{
				for (auto &zone : unitZone->getNeighbourhood())
					zone->setBunkerDefense(true);
			}

			if (unit->getType() == BWAPI::UnitTypes::Terran_Missile_Turret)
			{
				for (auto &zone : unitZone->getNeighbourhood())
					zone->setTurretDefense(true);
			}

			DesireHelper::setDefendDesire(unitZone, unitZone->getEnemyScore());
		}
	}
}

void SwarmCAT::drawDebugText()
{
	Broodwar->drawTextScreen(445, 20, "+ %.0f", EconHelper::getMineralIncome());
	Broodwar->drawTextScreen(510, 20, "+ %.0f", EconHelper::getGasIncome());
	Broodwar->drawTextScreen(445, 30, "%d", util::game::getSelf()->minerals() - EconHelper::getMinerals());
	Broodwar->drawTextScreen(510, 30, "%d", util::game::getSelf()->gas() - EconHelper::getGas());
	Broodwar->drawTextScreen(200, 0, "FPS: %d", Broodwar->getFPS());
	Broodwar->drawTextScreen(200, 10, "Average FPS: %f", Broodwar->getAverageFPS());
	Broodwar->drawTextScreen(200, 40, "Agent Count: %d", AgentHelper::getAgentset().size());	
	Broodwar->drawTextScreen(200, 60, "Root Tasks: ");

	int i = 1;
	for (auto &task : TaskHelper::getRootTasks())
		Broodwar->drawTextScreen(200, 60 + (10 * i++), "%d : %s", task->getID(), task->getName().c_str());
	
	Broodwar->drawTextScreen(350, 50, "All Tasks: ");
	i = 1;
	for (auto &task : TaskHelper::getAllTasks())
		Broodwar->drawTextScreen(350, 50 + (10 * i++), "%d : %s", task->getID(), task->getName().c_str());

	i = 1;
	for (auto &coalition : CoalitionHelper::getCoalitionset())
	{
		Broodwar->drawTextScreen(5, 100 + (10 * i++), "%d : %.2f | %.2f - %s", coalition->getID(), coalition->getCost(), coalition->getProfit(), coalition->getTask()->getName().c_str());
		for (auto &unitType : coalition->getTargetComp().getUnitMap())
			Broodwar->drawTextScreen(10, 100 + (10 * i++), "%s : %d/%d", unitType.first.c_str(), coalition->getCurrentComp().getUnitMap()[unitType.first], unitType.second);
		if(coalition->isActive() && coalition->getTask()->getType() == ATT)
			Broodwar->drawCircleMap(coalition->getUnitSet().getPosition(), 10 + (coalition->getUnitSet().size() * 5), BWAPI::Colors::Red);		
	}

	for (auto &a : AgentHelper::getAgentset())
	{
		auto u = a->getUnit();
		if (u->exists())
		{
			BWAPI::Color unitCircleColor;

			if (a->isFree())
				unitCircleColor = BWAPI::Colors::Green;
			else
				unitCircleColor = BWAPI::Colors::Blue;

			Broodwar->drawCircleMap(
				u->getPosition(),
				u->getType().dimensionLeft(),
				unitCircleColor
				);
			
			Broodwar->drawTextMap(
				u->getPosition(),
				"%d", a->getBoredom()
				);
		}
	}

	ArmyHelper::printPriorityList(3);

	for (auto &rd : AgentHelper::getResourceDepots())
	{
		for (auto &mineral : rd->getBaseLocation()->getMinerals())
			Broodwar->drawLineMap(rd->getBaseLocation()->getPosition(), mineral->getPosition(), Colors::Green);
		bool gasSaturated = false;
		bool minSaturated = false;
		minSaturated = rd->isMineralSaturated();			
		gasSaturated = rd->isGasSaturated();
		Broodwar->drawCircleMap(rd->getUnit()->getPosition().x - 22, rd->getUnit()->getPosition().y - 37, 2, BWAPI::Colors::Teal, minSaturated);
		Broodwar->drawCircleMap(rd->getUnit()->getPosition().x - 22, rd->getUnit()->getPosition().y - 21, 2, BWAPI::Colors::Green, gasSaturated);
		Broodwar->drawTextMap(rd->getUnit()->getPosition().x - 15, rd->getUnit()->getPosition().y - 44, "%d / %.0f", rd->getMineralMiners(), rd->getBaseLocation()->getMinerals().size() * 2.5);
		Broodwar->drawTextMap(rd->getUnit()->getPosition().x - 15, rd->getUnit()->getPosition().y - 28, "%d / %.0f", rd->getGasMiners(), rd->getRefineryCount() * 3.0);
	}

	for (auto &expansion : DesireHelper::getExpansionDesireMap())
		Broodwar->drawTextMap(expansion.first->getPosition(), "Expansion Desire: %f", expansion.second);
	
	Broodwar->drawTextScreen(10, 10, "Worker Count: %d", util::game::getSelf()->allUnitCount(util::game::getSelf()->getRace().getWorker()));
	Broodwar->drawTextScreen(10, 20, "Expand Desire: %.2f", DesireHelper::getExpandDesire());
	Broodwar->drawTextScreen(10, 30, "Supply Desire: %.2f", DesireHelper::getSupplyDesire());

	/*draw zones*/
	auto unitDefenseDesireMap = DesireHelper::getUnitDefenseDesireMap();
	auto bunkerDefenseDesireMap = DesireHelper::getBunkerDefenseDesireMap();
	auto turretDefenseDesireMap = DesireHelper::getTurretDefenseDesireMap();
	for (auto &zone : MapHelper::getRegionField())
	{
		//Broodwar->drawTextMap(zone->getRegion()->getCenter(), "ZoneID : %d\nDefend Count : %d\nUnit Defense Desire : %.2f\nStatic Defense Desire : %.2f", zone->getID(), zone->getTimesDefended(), unitDefenseDesireMap[zone], staticDefenseDesireMap[zone]);
		Broodwar->drawCircleMap(zone->getRegion()->getCenter(), 15, Color((int)bunkerDefenseDesireMap[zone] * 10, 0, 0), true);
		Broodwar->drawCircleMap(zone->getRegion()->getCenter(), 10, Color((int)turretDefenseDesireMap[zone] * 10, 0, 0), true);
		Broodwar->drawCircleMap(zone->getRegion()->getCenter(), 5, Color((int)unitDefenseDesireMap[zone] * 10, 0, 0), true);
		
		if(zone->isDefending())
			Broodwar->drawCircleMap(zone->getRegion()->getCenter(), 2, Colors::Red, true);
		else
			Broodwar->drawCircleMap(zone->getRegion()->getCenter(), 2, Colors::Green, true);
		
	}
}

void SwarmCAT::drawTerrainData()
{
	//we will iterate through all the base locations, and draw their outlines.
	for (const auto &baseLocation : BWTA::getBaseLocations()) {
		TilePosition p = baseLocation->getTilePosition();

		//draw outline of center location
		Position leftTop(p.x * TILE_SIZE, p.y * TILE_SIZE);
		Position rightBottom(leftTop.x + 4 * TILE_SIZE, leftTop.y + 3 * TILE_SIZE);
		Broodwar->drawBoxMap(leftTop, rightBottom, Colors::Blue);

		//draw a circle at each mineral patch
		for (const auto &mineral : baseLocation->getStaticMinerals()) {
			Broodwar->drawCircleMap(mineral->getInitialPosition(), 30, Colors::Cyan);
		}

		//draw the outlines of Vespene geysers
		for (const auto &geyser : baseLocation->getGeysers()) {
			TilePosition p1 = geyser->getInitialTilePosition();
			Position leftTop1(p1.x * TILE_SIZE, p1.y * TILE_SIZE);
			Position rightBottom1(leftTop1.x + 4 * TILE_SIZE, leftTop1.y + 2 * TILE_SIZE);
			Broodwar->drawBoxMap(leftTop1, rightBottom1, Colors::Orange);
		}

		//if this is an island expansion, draw a yellow circle around the base location
		if (baseLocation->isIsland()) {
			Broodwar->drawCircleMap(baseLocation->getPosition(), 80, Colors::Yellow);
		}
	}

	//we will iterate through all the regions and ...
	for (const auto & region : BWTA::getRegions()) {
		// draw the polygon outline of it in green
		BWTA::Polygon p = region->getPolygon();
		for (size_t j = 0; j < p.size(); ++j) {
			Position point1 = p[j];
			Position point2 = p[(j + 1) % p.size()];
			Broodwar->drawLineMap(point1, point2, Colors::Green);
		}
		// visualize the chokepoints with red lines
		for (auto const& chokepoint : region->getChokepoints()) {
			Position point1 = chokepoint->getSides().first;
			Position point2 = chokepoint->getSides().second;
			Broodwar->drawLineMap(point1, point2, Colors::Red);
		}
	}
}