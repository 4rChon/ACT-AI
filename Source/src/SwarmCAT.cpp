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

	Broodwar->enableFlag(Flag::UserInput);

	//Broodwar->enableFlag(Flag::CompleteMapInformation);

	Broodwar->setCommandOptimizationLevel(1);	

	util::initialiseUtil();
	CompositionHelper::initialiseHelper();
	
	if (Broodwar->isReplay())
	{
		Broodwar << "The following players are in this replay:" << std::endl;

		Playerset players = Broodwar->getPlayers();
		for (auto p : players)
			if (!p->isObserver())
				Broodwar << p->getName() << ", playing as " << p->getRace() << std::endl;

		ReplayHelper::initialiseHelper();
	}
	else
	{
		if (Broodwar->enemy())
			Broodwar << "The matchup is " << util::getSelf()->getRace() << " vs " << util::getEnemy()->getRace() << std::endl;

		BWTA::readMap();
		AnalyzeThread();
		
		CoalitionHelper::initialiseHelper();
		MapHelper::initialiseHelper();
		AgentHelper::initialiseHelper();
		EconHelper::initialiseHelper();
		DesireHelper::initialiseHelper();
		TaskHelper::initialiseHelper();
		ArmyHelper::initialiseHelper();

		//BWAPI::SetContainer<BWAPI::UnitType> unitTypes;
		/*unitTypes.insert(BWAPI::UnitTypes::Zerg_Zergling);
		unitTypes.insert(BWAPI::UnitTypes::Zerg_Lurker);
		unitTypes.insert(BWAPI::UnitTypes::Zerg_Hydralisk);*/
		/*CompositionHelper::filterByCounter(unitTypes);*/
		//CompositionHelper::getCounters(BWAPI::UnitTypes::Protoss_Carrier);

		/* just for testing */

		CreateUnit* createUnit = new CreateUnit(BWAPI::UnitTypes::Terran_Comsat_Station, 1);
		TaskHelper::addTask(createUnit, true);
	}
}

void SwarmCAT::onEnd(bool isWinner)
{
	CompositionHelper::saveCompositions();
	std::cout << " ---------------- MATCH END ----------------\n";
}

void SwarmCAT::onFrame()
{
	if (!Broodwar->isReplay())
	{
		//std::cout << "---FrameStart---\n";	
		drawDebugText();
		//Cancel unfinished structures if not being constructed by an scv.
		if (util::getSelf()->incompleteUnitCount(BWAPI::UnitTypes::Buildings))
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
			if ((*a)->isFree())
			{
				(*a)->pollCoalitions();
				AgentHelper::setLastServiced(++a);
				continue;
			}

			AgentHelper::setLastServiced(++a);

			auto diff = std::chrono::high_resolution_clock::now() - currentTime;

			//if (diff.count() > 0.005)
			//	return;
		}

		TaskHelper::updateRootTasks();
		EconHelper::updateEconomy();
		ArmyHelper::updateArmyMovement();
		DesireHelper::updateDesireMaps();

		if (Broodwar->getFrameCount() % (24 * 120) == 0)
			DesireHelper::updateSupplyDesire();
		//std::cout << "---FrameEnd---\n";
	}
	else
	{
		ReplayHelper::evolveFlaggedCompositions();
		//ReplayHelper::showNextComposition();
	}
}

void SwarmCAT::onSendText(std::string text)
{
	Broodwar->sendText("%s", text.c_str());
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
		//std::cout << "UnitDiscover\n";
		if (unit->getType().isBuilding() && unit->getPlayer() == util::getEnemy())
			ArmyHelper::addTargetPriority(unit);
	}
}

void SwarmCAT::onUnitEvade(BWAPI::Unit unit)
{
	//std::cout << "UnitEvade\n";
}

void SwarmCAT::onUnitShow(BWAPI::Unit unit)
{
	////std::cout << "UnitShow\n";
	if (!Broodwar->isReplay())
	{
		if (unit->getPlayer() == util::getEnemy())
		{
			ArmyHelper::addScoutedUnit(unit->getID(), unit->getType());
			if (unit->isCloaked())
				ArmyHelper::scan(unit->getPosition());
		}
	}
	
	//if (unit->getType() == BWAPI::UnitTypes::Zerg_Lurker || unit->getType().hasPermanentCloak())
	//	ArmyHelper::scan(unit->getPosition());
}

void SwarmCAT::onUnitHide(BWAPI::Unit unit)
{	
	//std::cout << "UnitHide\n";
}

void SwarmCAT::onUnitCreate(BWAPI::Unit unit)
{
	if (!Broodwar->isReplay())
	{
		//std::cout << unit->getID() << " : " << unit->getType().c_str() << " : Unit created!\n";
		if (unit->getPlayer() == util::getSelf() && !(unit->getType().supplyProvided() == 16 || unit->getType().isBuilding()))
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
		//std::cout << unit->getID() << " : " << unit->getType().c_str() << " : Unit destroyed!\n";
		if (unit->getPlayer() == util::getSelf() && !unit->isBeingConstructed())
		{
			AgentHelper::removeAgent(unit->getID());
			DesireHelper::updateSupplyDesire(unit->getType(), true);
		}

		if (unit->getPlayer() == util::getEnemy())
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
		//std::cout << unit->getID() << " : Unit morphed!\n";
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
	//std::cout << "UnitRenegade\n";
}

void SwarmCAT::onSaveGame(std::string gameName)
{
	Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void SwarmCAT::onUnitComplete(BWAPI::Unit unit)
{
	if (!Broodwar->isReplay())
	{
		//std::cout << unit->getID() << " : Unit complete!\n";
		if (unit->getPlayer() == util::getSelf())
		{
			if (unit->getType().isResourceDepot())
				DesireHelper::updateSupplyDesire(unit->getType());
			AgentHelper::createAgent(unit);
		}
	}
}

void SwarmCAT::drawDebugText()
{
	Broodwar->drawTextScreen(445, 20, "+ %.0f", EconHelper::getMineralIncome());
	Broodwar->drawTextScreen(510, 20, "+ %.0f", EconHelper::getGasIncome());
	Broodwar->drawTextScreen(445, 30, "%d", util::getSelf()->minerals() - EconHelper::getMinerals());
	Broodwar->drawTextScreen(510, 30, "%d", util::getSelf()->gas() - EconHelper::getGas());
	Broodwar->drawTextScreen(200, 0, "FPS: %d", Broodwar->getFPS());
	Broodwar->drawTextScreen(200, 10, "Average FPS: %f", Broodwar->getAverageFPS());
	//Broodwar->drawTextScreen(200, 30, "Candidate Base Count: %d", AgentHelper::getCandidateBases().size());
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
		Broodwar->drawTextScreen(5, 100 + (10 * i++), "%d : %.2f | %.2f", coalition->getID(), coalition->getCost(), coalition->getProfit());
		for (auto &unitType : coalition->getTargetComp().getUnitMap())
			Broodwar->drawTextScreen(10, 100 + (10 * i++), "%s : %d/%d", unitType.first.c_str(), coalition->getCurrentComp().getUnitMap()[unitType.first], unitType.second);
		if(coalition->isActive() && coalition->getTask()->getType() == ATT)
			Broodwar->drawCircleMap(coalition->getUnitSet().getPosition(), coalition->getUnitSet().size() * 5, BWAPI::Colors::Red);
	}

	auto scoutedUnits = ArmyHelper::getScoutedUnits();
	i = 1;
	for (auto type : scoutedUnits.getTypes())
		Broodwar->drawTextScreen(400, 250 + (10 * i++), "%s : %d", type.c_str(), scoutedUnits[type]);
	

	/*auto bestUnit = DesireHelper::getMostDesirableUnit();
	auto bestRaxUnit = DesireHelper::getMostDesirableUnit(BWAPI::UnitTypes::Terran_Barracks);
	auto bestFactoryUnit = DesireHelper::getMostDesirableUnit(BWAPI::UnitTypes::Terran_Factory);
	auto bestStarportUnit = DesireHelper::getMostDesirableUnit(BWAPI::UnitTypes::Terran_Starport);
	auto unitDesireMap = DesireHelper::getUnitDesireMap();*/
	/*Broodwar->drawTextScreen(325, 250, "Most Desirable Unit: %s - %.2f", bestUnit.c_str(), unitDesireMap[bestUnit]);
	Broodwar->drawTextScreen(325, 260, "Most Desirable Unit (Barracks): %s - %.2f", bestRaxUnit.c_str(), unitDesireMap[bestRaxUnit]);
	Broodwar->drawTextScreen(325, 270, "Most Desirable Unit (Factory): %s - %.2f", bestFactoryUnit.c_str(), unitDesireMap[bestFactoryUnit]);
	Broodwar->drawTextScreen(325, 280, "Most Desirable Unit (Starport): %s - %.2f", bestStarportUnit.c_str(), unitDesireMap[bestStarportUnit]);*/

	for (auto &a : AgentHelper::getAgentset())
	{
		auto u = a->getUnit();
		if (u->exists())
		{
			if (u->getType().isWorker())
			{
				if (((Worker*)a)->getMiningBase())

				{
					Broodwar->drawLineMap(u->getPosition(), ((Worker*)a)->getMiningBase()->getBaseLocation()->getPosition(), Colors::Red);					
					if(u->getOrderTargetPosition() != BWAPI::Position(0, 0))
						Broodwar->drawLineMap(u->getPosition(), u->getOrderTargetPosition(), Colors::Blue);
				}
			}
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
	
	Broodwar->drawTextScreen(10, 10, "Worker Count: %d", util::getSelf()->allUnitCount(util::getSelf()->getRace().getWorker()));
	Broodwar->drawTextScreen(10, 20, "Expand Desire: %.2f", DesireHelper::getExpandDesire());
	Broodwar->drawTextScreen(10, 30, "Supply Desire: %.2f", DesireHelper::getSupplyDesire());
	Broodwar->drawTextScreen(10, 40, "Unit Multiplier: %.2f", EconHelper::getUnitMultiplier());

	/*draw zones
	for (auto &zone : MapHelper::getRegionField())
	{
		Broodwar->drawTextMap(zone->getRegion()->getCenter(), "ZoneID : %d", zone->getID());
		Broodwar->drawTextMap(zone->getRegion()->getCenter().x, zone->getRegion()->getCenter().y + 10, "FriendScore : %d", zone->getFriendScore());
		Broodwar->drawTextMap(zone->getRegion()->getCenter().x, zone->getRegion()->getCenter().y + 20, "EnemyScore: %d", zone->getEnemyScore());
		Broodwar->drawTextMap(zone->getRegion()->getCenter().x, zone->getRegion()->getCenter().y + 30, "Time Since Last : %d", BWAPI::Broodwar->getFrameCount() - zone->getLastVisited());
	}*/
}

void SwarmCAT::drawTerrainData()
{
	//we will iterate through all the base locations, and draw their outlines.
	for (const auto& baseLocation : BWTA::getBaseLocations()) {
		TilePosition p = baseLocation->getTilePosition();

		//draw outline of center location
		Position leftTop(p.x * TILE_SIZE, p.y * TILE_SIZE);
		Position rightBottom(leftTop.x + 4 * TILE_SIZE, leftTop.y + 3 * TILE_SIZE);
		Broodwar->drawBoxMap(leftTop, rightBottom, Colors::Blue);

		//draw a circle at each mineral patch
		for (const auto& mineral : baseLocation->getStaticMinerals()) {
			Broodwar->drawCircleMap(mineral->getInitialPosition(), 30, Colors::Cyan);
		}

		//draw the outlines of Vespene geysers
		for (const auto& geyser : baseLocation->getGeysers()) {
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
	for (const auto& region : BWTA::getRegions()) {
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