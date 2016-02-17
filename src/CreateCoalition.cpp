#include "..\include\CreateCoalition.h"
#include "..\include\CoalitionHelper.h"
#include "..\include\TaskHelper.h"
#include "..\include\CreateUnit.h"

CreateCoalition::CreateCoalition(Composition composition, Task* task)
{
	taskName = "CreateCoalition(Composition, Task*)";
	taskCoalition = CoalitionHelper::addCoalition(composition, task);	
	cost = composition.getCost();	
}

CreateCoalition::~CreateCoalition()
{
	taskCoalition = nullptr;
}

// add coalition to open coalitions
void CreateCoalition::assign()
{
	task->setCoalition(taskCoalition);
	assigned = true;
}

// attempt to activate coalition
void CreateCoalition::act()
{
	printDebugInfo("Acting");
	Composition differenceComposition = taskCoalition->getTargetComp() - taskCoalition->getCurrentComp();
	for (auto unitType : differenceComposition.getTypes())
	{
		if (differenceComposition[unitType] > 0)
		{
			std::cout << "I need " << differenceComposition[unitType] << " more " << unitType.c_str() << "\n";
			CreateUnit *createUnit = new CreateUnit(unitType, differenceComposition[unitType]);
			addSubTask(createUnit);
		}
	}
	acting = true;
	printDebugInfo("Acting End");
}

void CreateCoalition::update()
{
	//std::cout << taskName.c_str() << " : " << taskID << " : Update\n";
	if (complete)
		return;

	if (!assigned)
		assign();

	if (taskCoalition->isActive())
	{
		succeed();
		return;
	}

	if (assigned && !acting)
		act();
	//if(age > x) fail();
	//TODO: fail when taking too long
	//std::cout << taskName.c_str() << " : " << taskID << " : Update End\n";
}

double CreateCoalition::getCost()
{
	cost = taskCoalition->getTargetComp().getCost() - taskCoalition->getCurrentComp().getCost();
	for (auto task : subTasks)
		cost += task->getCost();

	return cost;
}