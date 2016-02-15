#include "..\include\CreateUnit.h"
#include "..\include\CreateCoalition.h"
#include "..\include\EconHelper.h"
#include "..\include\SatisfyUnitRequirement.h"
#include "..\include\TaskHelper.h"

CreateUnit::CreateUnit(BWAPI::UnitType unitType, int unitCount)
{
	taskName = "CreateUnit(" + unitType.getName() + ")"; //
	this->unitType = unitType;
	this->unitCount = unitCount;
	satisfyingUnit = false;
	satisfyingTech = false;
	satisfied = true;
	debug = false;
	building = false;
	reserved = false;
	TaskHelper::addTask(this, false);
}

void CreateUnit::satisfyUnitRequirements()
{
	for (auto &required : unitType.requiredUnits())
	{
		if (!BWAPI::Broodwar->self()->hasUnitTypeRequirement(required.first))
			satisfied = false;
	}	

	if (!satisfyingUnit)
	{
		for (auto &required : unitType.requiredUnits())
		{
			printDebugInfo("Attempting to Satisfy Unit - Unit Requirement");
			SatisfyUnitRequirement* satisfyUnitRequirement = new SatisfyUnitRequirement(unitType);
			subTasks.insert(satisfyUnitRequirement);
			satisfyingUnit = true;
			return;
		}
	}
}

void CreateUnit::satisfyTechRequirements()
{		
	if (unitType.requiredTech() != BWAPI::TechTypes::None && !BWAPI::Broodwar->self()->hasResearched(unitType.requiredTech()))
	{
		satisfied = false;
		if (!satisfyingTech)
		{
			printDebugInfo("Attempting to Satisfy Unit - Tech Requirement"); //unit tech requirements
			SatisfyUnitRequirement* satisfyUnitRequirement = new SatisfyUnitRequirement(unitType);
			subTasks.insert(satisfyUnitRequirement);
			satisfyingTech = true;
		}
	}
		
}

// assign a producer coalition
void CreateUnit::assign()
{
	//printDebugInfo("Assign");
	satisfied = true;	
	satisfyUnitRequirements();
	if(satisfied)
		satisfyTechRequirements();
		
	if (satisfied)// && EconHelper::haveMoney(unitType))
	{
		createCoalition();
		assigned = true;
		//printDebugInfo("Assigned!");
	}
	//printDebugInfo("Assign End");
}

void CreateUnit::createCoalition()
{
	printDebugInfo("Unit requirements satisfied");
	Composition producer;
	auto whatBuilds = unitType.whatBuilds().first;
	if(whatBuilds == BWAPI::UnitTypes::Zerg_Larva)
		producer.addType(BWAPI::UnitTypes::Zerg_Hatchery, 1);
	else
		producer.addType(unitType.whatBuilds().first, unitType.whatBuilds().second);
	CreateCoalition* createCoalition = new CreateCoalition(producer, this);
	subTasks.insert(createCoalition);
}

// produce a unit

void CreateUnit::act()
{
	printDebugInfo("Acting");
	if (unitCount > 0)
	{
		if (unitType == BWAPI::UnitTypes::Zerg_Lurker)
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
				if (unitType.getRace() == BWAPI::Races::Zerg && unitType.isResourceDepot() && unitType != BWAPI::UnitTypes::Zerg_Hatchery)
				{
					std::cout << "MORPH ME WITH YOUR SPECIAL MORPH\n";
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
		return;

	if (!assigned)
	{
		assign();
		return;
	}

	if (coalition->isActive())
		act();
	printDebugInfo("Update End");
}