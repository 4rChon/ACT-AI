#include "..\include\CreateUnit.h"
#include "..\include\CreateCoalition.h"
#include "..\include\EconHelper.h"
#include "..\include\SatisfyUnitRequirement.h"
#include "..\include\TaskHelper.h"
#include "..\include\CoalitionHelper.h"
#include <string>

CreateUnit::CreateUnit(BWAPI::UnitType unitType, int unitCount)
{
	taskName = "CreateUnit(" + unitType.getName() + " : " + std::to_string(unitCount) + ")";
	
	this->unitType = unitType;
	this->unitCount = unitCount;
	satisfying = false;	
	satisfied = true;
	building = false;
	reserved = false;
	requiresGas = false;

	debug = false;
}

void CreateUnit::satisfyRequirements()
{
	if (unitType.gasPrice() > 0 && EconHelper::getGasIncome() == 0)
	{
		requiresGas = true;
		satisfied = false;		
	}

	for (auto &required : unitType.requiredUnits())
		if (!BWAPI::Broodwar->self()->hasUnitTypeRequirement(required.first))
			satisfied = false;

	if (unitType.requiredTech() != BWAPI::TechTypes::None && !BWAPI::Broodwar->self()->hasResearched(unitType.requiredTech()))
		satisfied = false;

	if (!satisfying && !satisfied)
	{
		printDebugInfo("Attempting to Satisfy Unit - Unit Requirement");
		SatisfyUnitRequirement* satisfyUnitRequirement = new SatisfyUnitRequirement(unitType);
		addSubTask(satisfyUnitRequirement);
		if (requiresGas)
		{
			CreateUnit* createGas = new CreateUnit(BWAPI::Broodwar->self()->getRace().getRefinery());
			addSubTask(createGas);
		}
		satisfying = true;
	}
}

void CreateUnit::createCoalition()
{
	printDebugInfo("Unit requirements satisfied");
	Composition producer;
	auto whatBuilds = unitType.whatBuilds().first;

	if (whatBuilds == BWAPI::UnitTypes::Zerg_Larva)
		producer.addType(BWAPI::UnitTypes::Zerg_Hatchery, 1);
	else
		producer.addType(unitType.whatBuilds().first, unitType.whatBuilds().second);
	CreateCoalition* createCoalition = new CreateCoalition(producer, this);
	subTasks.insert(createCoalition);
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
	if (unitCount > 0)
	{
		if (unitType == BWAPI::Broodwar->self()->getRace().getRefinery())
			for each (auto &resourceDepot in AgentHelper::getResourceDepots())
			{
				if (resourceDepot->addGeyser(*coalition->getAgentSet().begin()))
				{
					unitCount--;
					break;
				}
			}
		if (unitType == BWAPI::UnitTypes::Zerg_Lurker || unitType == BWAPI::UnitTypes::Zerg_Guardian)
		{
			if((*coalition->getAgentSet().begin())->morph(unitType))
				unitCount--;
		}
		if (unitType == BWAPI::UnitTypes::Protoss_Archon)
		{
			auto unit = (*++coalition->getUnitSet().begin());
			if((*coalition->getAgentSet().begin())->useAbility(BWAPI::TechTypes::Archon_Warp, unit))
				unitCount--;
		}

		if (unitType == BWAPI::UnitTypes::Protoss_Dark_Archon)
		{
			auto unit = (*++coalition->getUnitSet().begin());
			if ((*coalition->getAgentSet().begin())->useAbility(BWAPI::TechTypes::Dark_Archon_Meld, unit))
				unitCount--;
		}

		if (unitType.isAddon())
		{
			for (auto &agent : coalition->getAgentSet())
			{					
				if (agent->buildAddon(unitType))
					unitCount--;
			}
		}

		if (unitType.isBuilding())
		{
			
			for (auto &agent : coalition->getAgentSet())
			{
				if (unitType.getRace() == BWAPI::Races::Zerg 
					&& (unitType.isResourceDepot() && unitType != BWAPI::UnitTypes::Zerg_Hatchery) 
					|| unitType == BWAPI::UnitTypes::Zerg_Greater_Spire)
				{
					if (agent->morph(unitType))
						unitCount--;
				}
				else if (agent->build(unitType, nullptr))
					unitCount--;
			}
		}
		else
		{
			for (auto &agent : coalition->getAgentSet())
			{
				if (agent->train(unitType))
					unitCount--;
			}
		}
	}
	else
		succeed();
	printDebugInfo("Acting End");
}

void CreateUnit::update() //x2 redundant in res tech
{
	printDebugInfo("Update");
	if (complete)
	{
		cleanSubTasks();
		return;
	}

	if (!assigned)
	{
		assign();
		return;
	}

	if (coalition->isActive())
		act();
	printDebugInfo("Update End");
}