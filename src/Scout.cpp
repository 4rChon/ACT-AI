#include "..\include\Scout.h"
#include "..\include\CreateCoalition.h"

Scout::Scout(MapHelper::Zone* target)
{
	taskName = "Scout()";
	this->target = target;
	debug = false;
}

void Scout::assign()
{
	printDebugInfo("Assign");
	Composition c;
	c.addType(BWAPI::Broodwar->self()->getRace().getWorker(), 1);
	CreateCoalition *createCoalition = new CreateCoalition(c, this);
	addSubTask(createCoalition);
	assigned = true;
	printDebugInfo("Assign End");
}

void Scout::act()
{
	printDebugInfo("Acting");
	acting = ((*coalition->getAgentSet().begin())->move(target->getRegion()->getCenter()));
	printDebugInfo("Acting End");
}

void Scout::update()
{
	printDebugInfo("Update");
	if (complete)
	{
		cleanSubTasks();
		return;
	}
	if (!assigned)
		assign();
	if (coalition->isActive())
		act();
	if (BWAPI::Broodwar->getFrameCount() - target->getLastVisited() < 5)
		succeed();
	printDebugInfo("Update End");
}

