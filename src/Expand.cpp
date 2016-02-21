#include "..\include\Composition.h"
#include "..\include\Expand.h"
#include "..\include\EconHelper.h"
#include "..\include\TaskHelper.h"
#include "..\include\CreateCoalition.h"
#include "..\include\CoalitionHelper.h"

Expand::Expand()
{
	taskName = "Expand()";
	builder = nullptr;
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
	complete = builder->expand();
	printDebugInfo("Acting End");
}

void Expand::update()
{
	printDebugInfo("Update");
	if (complete)
	{
		cleanSubTasks();
		return;
	}

	if (!assigned)
		assign();

	if (coalition->isActive() && !builder)
		builder = (*coalition->getAgentSet().begin());		

	if (builder && !complete)
		act();
	printDebugInfo("Update End");
}