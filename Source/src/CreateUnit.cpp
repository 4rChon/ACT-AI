#include "CreateUnit.h"
#include "CreateCoalition.h"
#include "EconHelper.h"
#include "SatisfyUnitRequirement.h"
#include "TaskHelper.h"
#include "CoalitionHelper.h"
#include "DesireHelper.h"
#include "EconHelper.h"
#include <string>

CreateUnit::CreateUnit(BWAPI::UnitType unitType, int unitCount)
	:unitType(unitType)
	,unitCount(unitCount)
	,satisfying(false)
	,satisfied(true)
	,requiresGas(false)
{
	taskName = "CreateUnit(" + unitType.getName() + " - " + std::to_string(unitCount) + ")";
	taskType = CRU;
}

void CreateUnit::createCoalition()
{
	printDebugInfo("Unit requirements satisfied");
	Composition producer;
	BWAPI::UnitType whatBuilds = unitType.whatBuilds().first;

	if (!whatBuilds.isWorker())
	{
		for (int i = 0; i < unitCount / 5; i++)
			producer.addType(whatBuilds, unitType.whatBuilds().second);
	}
	else
		producer.addType(whatBuilds, unitType.whatBuilds().second);
	CreateCoalition* createCoalition = new CreateCoalition(producer, this);
	addSubTask(createCoalition);
}

void CreateUnit::satisfyRequirements()
{
	if ((unitType.gasPrice()/((double)unitType.buildTime() / (24 * 60))) - EconHelper::getGasIncome() > 0)
	{		
		requiresGas = true;
		satisfied = false;
	}

	if(!BWAPI::Broodwar->canMake(unitType))
		satisfied = false;

	if (!satisfying && !satisfied)
	{
		printDebugInfo("Attempting to Satisfy Unit - Unit Requirement");
		SatisfyUnitRequirement* satisfyUnitRequirement = new SatisfyUnitRequirement(unitType);
		addSubTask(satisfyUnitRequirement);
		if (requiresGas)
		{
			CreateUnit* createGas = new CreateUnit(util::game::getSelf()->getRace().getRefinery());
			addSubTask(createGas);
		}
		satisfying = true;
	}
}

// assign a producer coalition
void CreateUnit::assign()
{
	if ((unitType.isWorker() || unitType.isResourceDepot()) && (util::game::getSelf()->allUnitCount(BWAPI::UnitTypes::Terran_SCV) == 0 && util::game::getSelf()->allUnitCount(BWAPI::UnitTypes::Terran_Command_Center) == 0))
		fail();

	printDebugInfo("Assign");
	if (satisfied && EconHelper::haveMoney(unitType) && EconHelper::haveSupply(unitType))
	{
		createCoalition();
		assigned = true;
		printDebugInfo("Assigned!");
	}
	printDebugInfo("Assign End");
}

// produce a unit
void CreateUnit::act()
{	
	printDebugInfo("Acting");	

	if (!EconHelper::haveMoney(unitType))
		return;

	printDebugInfo("UnitCount: " + std::to_string(unitCount));
	if (unitType == util::game::getSelf()->getRace().getRefinery())
	{
		for each (auto &agent in coalition->getAgentSet())
		{
			for each (auto &resourceDepot in AgentHelper::getResourceDepots())
				if (resourceDepot->addGeyser((Worker*)agent))
					break;
		}
		return;
	}

	//if the unitType is an addon, use the build addon command
	if (unitType.isAddon())
	{
		for each (auto &agent in coalition->getAgentSet())
		{
			if(agent->buildAddon(unitType))
				unitCount--;
		}
		return;
	}

	//if the unitType is a building, use the build command
	if (unitType.isBuilding())
	{
		for (auto &agent : coalition->getAgentSet())
			agent->build(unitType);
		return;
	}

	//if the unitType is a unit, use the train command
	else
	{
		for (auto &agent : coalition->getAgentSet())
		{
			if (agent->train(unitType))
				unitCount--;
		}
		return;
	}
		
	printDebugInfo("Acting End");
}

void CreateUnit::update()
{
	printDebugInfo("Update");
	if (complete)
		return;

	satisfied = true;
	satisfyRequirements();

	//if unitCount is <= 0, then succeed
	if (unitCount <= 0)
	{
		succeed();
		return;
	}

	//if the unitType is a refinery and all bases already have a refinery, then succeed
	if (unitType.isRefinery() && util::game::getSelf()->allUnitCount(util::game::getSelf()->getRace().getRefinery()) >= (int)AgentHelper::getResourceDepots().size())
	{
		succeed();
		return;
	}

	//if maxed, then succeed
	if (util::game::getSelf()->supplyTotal() >= 400)
	{
		succeed();
		return;
	}

	if (!assigned)
	{
		assign();
		return;
	}

	if (!coalition)
	{
		fail();
		return;
	}

	if (coalition->isActive() && unitCount > 0)
		act();

	printDebugInfo("Update End");
}