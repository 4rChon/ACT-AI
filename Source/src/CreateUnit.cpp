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
{
	taskName = "CreateUnit(" + unitType.getName() + " - " + std::to_string(unitCount) + ")";
		
	this->unitType = unitType;	
	this->unitCount = unitCount;
	satisfying = false;	
	satisfied = true;
	requiresGas = false;
	taskType = CRU;

	//debug = true;
}

void CreateUnit::createCoalition()
{
	printDebugInfo("Unit requirements satisfied");
	Composition producer;
	auto whatBuilds = unitType.whatBuilds().first;

	if (!whatBuilds.isWorker())
	{
		for (int i = 0; i < unitCount / 5; i++)
			producer.addType(whatBuilds, unitType.whatBuilds().second);
	}
	else
		producer.addType(whatBuilds, unitType.whatBuilds().second);
	CreateCoalition* createCoalition = new CreateCoalition(producer, this);
	subTasks.insert(createCoalition);
}


void CreateUnit::satisfyRequirements()
{
	if (unitType.gasPrice() > 0 && EconHelper::getGasIncome() == 0)
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

void CreateUnit::decrementUnitCount()
{
	printDebugInfo("Decrementing Unit Count");
	unitCount--;
}

// assign a producer coalition
void CreateUnit::assign()
{
	printDebugInfo("Assign");
	satisfied = true;	
	satisfyRequirements();
		
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

	//if the unitType is morphed from another unit, use a morph command
	if (unitType == BWAPI::UnitTypes::Zerg_Lurker || unitType == BWAPI::UnitTypes::Zerg_Guardian)
	{
		for each (auto &agent in coalition->getAgentSet())
			if (agent->morph(unitType))
				unitCount--;
		return;
	}

	//if the unitType is created by merging two high templar, use the archon warp ability
	if (unitType == BWAPI::UnitTypes::Protoss_Archon)
	{
		if(coalition->getAgentSet().size() % 2 == 0)
			for (auto agent = coalition->getAgentSet().begin(); agent != coalition->getAgentSet().end(); ++agent)
			{
				if ((*agent)->useAbility(BWAPI::TechTypes::Archon_Warp, (*++agent)->getUnit()))
					unitCount--;
			}
		return;
	}

	//if the unitType is created by merging two dark templar, use the dark archon meld ability
	if (unitType == BWAPI::UnitTypes::Protoss_Dark_Archon)
	{
		if (coalition->getAgentSet().size() % 2 == 0)
			for (auto &agent = coalition->getAgentSet().begin(); agent != coalition->getAgentSet().end(); ++agent)
			{
				if ((*agent)->useAbility(BWAPI::TechTypes::Dark_Archon_Meld, (*++agent)->getUnit()))
					unitCount--;
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
		for each (auto &agent in coalition->getAgentSet())
		{
			if (unitType.getRace() == BWAPI::Races::Zerg
				&& (unitType.isResourceDepot() && unitType != BWAPI::UnitTypes::Zerg_Hatchery)
				|| unitType == BWAPI::UnitTypes::Zerg_Greater_Spire)
			{
				if (agent->morph(unitType))
					unitCount--;
			}
			else
				agent->build(unitType);
		}
		return;
	}
	//if the unitType is a unit, use the train command
	else
	{
		for each (auto &agent in coalition->getAgentSet())
			if (agent->train(unitType))
				unitCount--;
		return;
	}
		
	printDebugInfo("Acting End");
}

void CreateUnit::update()
{
	printDebugInfo("Update");
	if (complete)
		return;

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