#include "..\include\ResearchTech.h"
#include "..\include\SatisfyTechRequirement.h"
#include "..\include\EconHelper.h"
#include "..\include\CreateCoalition.h"
#include "..\include\TaskHelper.h"
#include "..\include\CoalitionHelper.h"

ResearchTech::ResearchTech(BWAPI::TechType techType)
{
	taskName = "ResearchTech(" + techType.getName() + ")";
	satisfying = false;
	satisfied = true;

	this->techType = techType;	
	debug = false;
}

void ResearchTech::createCoalition()
{

	printDebugInfo("Tech requirements satisfied");
	Composition producer;

	producer.addType(techType.whatResearches(), 1);
	CreateCoalition* createCoalition = new CreateCoalition(producer, this);
	addSubTask(createCoalition);
}

void ResearchTech::satisfyRequirements()
{	
	if (!BWAPI::Broodwar->self()->hasUnitTypeRequirement(techType.requiredUnit())
		&& !BWAPI::Broodwar->self()->hasUnitTypeRequirement(techType.whatResearches()))
	{					
		satisfied = false;
		if (!satisfying)
		{
			printDebugInfo("Satisfying Tech Requirement");
			SatisfyTechRequirement* satisfyTechRequirement = new SatisfyTechRequirement(techType);
			addSubTask(satisfyTechRequirement);
			satisfying = true;
		}
	}
}

void ResearchTech::assign()
{
	printDebugInfo("Assign");
	satisfied = true;
	satisfyRequirements();

	if (satisfied && EconHelper::haveMoney(techType))
	{
		createCoalition();
		assigned = true;
		printDebugInfo("Assigned!");
	}
	printDebugInfo("Assign End");
}

void ResearchTech::act()
{
	if (!BWAPI::Broodwar->self()->hasResearched(techType))
	{
		if (!BWAPI::Broodwar->self()->isResearching(techType))
		{
			if (!EconHelper::haveMoney(techType))
				return;
			(*coalition->getAgentSet().begin())->research(techType);
		}
	}
	else
		succeed();
}

void ResearchTech::update() //x2 redundant in create unit
{
	//printDebugInfo("Update");
	if (complete)
	{
		/*CoalitionHelper::removeCoalition(coalition);*/
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
	//printDebugInfo("Update End");
}
