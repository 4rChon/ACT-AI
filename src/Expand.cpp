#include "..\include\Composition.h"
//#include "..\include\CreateCoalition.h"
#include "..\include\Expand.h"
#include "..\include\EconHelper.h"
#include "..\include\TaskHelper.h"
#include "..\include\CreateCoalition.h"

Expand::Expand()
{
	builder = nullptr;
	taskName = "Expand()";
}

Expand::~Expand()
{
	builder = nullptr;
}

void Expand::assign()
{
	//std::cout << taskName.c_str() << " : " << taskID << " : Assign\n";
	Composition c;
	c.addType(BWAPI::Broodwar->self()->getRace().getWorker(), 1);	
	CreateCoalition *createCoalition = new CreateCoalition(c, this);
	subTasks.insert(createCoalition);
	assigned = true;
	//std::cout << taskName.c_str() << " : " << taskID << " : Assign End\n";
}

void Expand::act()
{
	//std::cout << taskName.c_str() << " : " << taskID << " : Acting\n";
	acting = true;
	complete = builder->expand();
	//std::cout << taskName.c_str() << " : " << taskID << " : Acting End\n";
}

void Expand::update()
{
	//std::cout << taskName.c_str() << " : " << taskID << " : Update\n";
	if (!assigned)
		assign();

	if (coalition->isActive() && !builder)
		builder = (*coalition->getAgentSet().begin());		

	if (builder && !complete)
		act();
	//std::cout << taskName.c_str() << " : " << taskID << " : Update End\n";
}