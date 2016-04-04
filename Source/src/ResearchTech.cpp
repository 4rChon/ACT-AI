#include "ResearchTech.h"
#include "SatisfyTechRequirement.h"
#include "EconHelper.h"
#include "CreateCoalition.h"
#include "TaskHelper.h"
#include "CoalitionHelper.h"

ResearchTech::ResearchTech(BWAPI::TechType techType)
{
	taskName = "ResearchTech(" + techType.getName() + ")";
	satisfying = false;
	satisfied = true;
	this->techType = techType;	
	taskType = RES;
	//debug = true;
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
	if (!util::getSelf()->hasUnitTypeRequirement(techType.requiredUnit())
		&& !util::getSelf()->hasUnitTypeRequirement(techType.whatResearches()))
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
	if (!util::getSelf()->hasResearched(techType))
	{
		if (!util::getSelf()->isResearching(techType))
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
