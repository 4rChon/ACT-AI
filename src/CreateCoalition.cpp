#include "..\include\CreateCoalition.h"
#include "..\include\CoalitionHelper.h"
#include "..\include\TaskHelper.h"
#include "..\include\CreateUnit.h"

CreateCoalition::CreateCoalition(Composition composition, Task* task)
{
	taskName = "CreateCoalition(Composition, Task*)";
	taskCoalition = CoalitionHelper::addCoalition(composition, task);
	task->setCoalition(taskCoalition);
	cost = composition.getCost();
}

CreateCoalition::~CreateCoalition()
{
	taskCoalition = nullptr;
}

// add coalition to open coalitions
void CreateCoalition::assign()
{
	//std::cout << taskName.c_str() << " : " << taskID << " : Assign\n";
	assigned = true;
	//std::cout << taskName.c_str() << " : " << taskID << " : Assign End\n";
}

// attempt to activate coalition
void CreateCoalition::act()
{
	//std::cout << taskName.c_str() << " : " << taskID << " : Acting\n";
	Composition differenceComposition = taskCoalition->getTargetComp() - taskCoalition->getCurrentComp();
	for (auto unitType : differenceComposition.getTypes())
	{
		if (differenceComposition[unitType] > 0)
		{
			std::cout << "I need " << differenceComposition[unitType] << " more " << unitType.c_str() << "\n";
			CreateUnit *createUnit = new CreateUnit(unitType, differenceComposition[unitType]);
			subTasks.insert(createUnit);
		}
	}
	acting = true;
	//std::cout << taskName.c_str() << " : " << taskID << " : Acting End\n";
}

void CreateCoalition::update()
{
	//std::cout << taskName.c_str() << " : " << taskID << " : Update\n";
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