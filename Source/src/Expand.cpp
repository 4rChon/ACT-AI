#include "Composition.h"
#include "Expand.h"
#include "EconHelper.h"
#include "TaskHelper.h"
#include "CreateCoalition.h"
#include "CoalitionHelper.h"

Expand::Expand()
{
	taskName = "Expand()";
	builder = nullptr;
	taskType = EXP;
	//debug = true;
}

Expand::~Expand()
{
	builder = nullptr;
}

void Expand::createCoalition()
{
	Composition c;
	c.addType(BWAPI::Broodwar->self()->getRace().getWorker(), 1);
	CreateCoalition *createCoalition = new CreateCoalition(c, this);
	addSubTask(createCoalition);
}

void Expand::assign()
{
	printDebugInfo("Assign");
	createCoalition();
	assigned = true;
	printDebugInfo("Assign End");
}

void Expand::act()
{
	printDebugInfo("Acting");
	acting = true;
	builder->expand();
	printDebugInfo("Acting End");
}

void Expand::update()
{
	printDebugInfo("Update");
	if (complete)
		return;

	if (!assigned)
	{
		assign();
		return;
	}

	if (coalition->isActive() && !builder)
		builder = (*coalition->getAgentSet().begin());

	if (builder && !complete)
		act();

	printDebugInfo("Update End");
}