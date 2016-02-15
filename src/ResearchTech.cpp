#include "..\include\ResearchTech.h"
#include "..\include\SatisfyTechRequirement.h"
#include "..\include\EconHelper.h"
#include "..\include\CreateCoalition.h"
#include "..\include\TaskHelper.h"

ResearchTech::ResearchTech(BWAPI::TechType techType)
{
	taskName = "ResearchTech(" + techType.getName() + ")";
	this->techType = techType;	
	debug = true;
	TaskHelper::addTask(this, false);
}

void ResearchTech::createCoalition()
{

	printDebugInfo("Tech requirements satisfied");
	Composition producer;

	producer.addType(techType.whatResearches(), 1);
	CreateCoalition* createCoalition = new CreateCoalition(producer, this);
	subTasks.insert(createCoalition);
}

void ResearchTech::satisfyRequirements()
{	
	if (!BWAPI::Broodwar->self()->hasUnitTypeRequirement(techType.requiredUnit())
		&& !BWAPI::Broodwar->self()->hasUnitTypeRequirement(techType.whatResearches()))
	{					
		if (!satisfying)
		{
			printDebugInfo("Satisfying Tech Requirement");
			SatisfyTechRequirement* satisfyTechRequirement = new SatisfyTechRequirement(techType);
			subTasks.insert(satisfyTechRequirement);
			satisfying = true;
			return;
		}
	}
	else
		satisfied = true;
}

void ResearchTech::assign()
{
	satisfyRequirements();

	if (satisfied && EconHelper::haveMoney(techType))
	{
		createCoalition();
		assigned = true;
		printDebugInfo("Assigned!");
	}
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
