#include "..\include\CreateCoalition.h"
#include "..\include\CoalitionHelper.h"
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
	std::cout << "CreateCoalition: Assign\n";
	assigned = true;
	std::cout << "CreateCoalition: Assign End\n";
}

// attempt to activate coalition
void CreateCoalition::act()
{
	std::cout << "CreateCoalition: Acting\n";
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
	std::cout << "CreateCoalition: Acting End\n";
}

void CreateCoalition::update()
{
	if (complete)
		return;
	if (!assigned)
		assign();
	if (taskCoalition->isActive())
		succeed();
	if (assigned && !acting)
		act();
	//if(age > x) fail();
	//TODO: fail when taking too long
}

double CreateCoalition::getCost()
{
	cost = taskCoalition->getTargetComp().getCost() - taskCoalition->getCurrentComp().getCost();
	for (auto task : subTasks)
		cost += task->getCost();

	return cost;
}