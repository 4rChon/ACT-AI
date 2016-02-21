#include "..\include\Scout.h"
#include "..\include\CreateCoalition.h"
#include <string>

Scout::Scout(MapHelper::Zone* target)
{
	taskName = "Scout(" + std::to_string(target->getID()) + ")";
	this->target = target;
	debug = false;
}

void Scout::createCoalition()
{
	Composition c;
	c.addType(BWAPI::Broodwar->self()->getRace().getWorker(), 1);
	//c.addType(BWAPI::UnitTypes::Protoss_Probe, 1);
	CreateCoalition *createCoalition = new CreateCoalition(c, this);
	addSubTask(createCoalition);
}

void Scout::assign()
{
	printDebugInfo("Assign");
	createCoalition();
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

