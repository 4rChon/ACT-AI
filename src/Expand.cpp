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
	std::cout << "Expand: Assign\n";
	Composition c;
	c.addType(BWAPI::Broodwar->self()->getRace().getWorker(), 1);
	CreateCoalition *createCoalition = new CreateCoalition(c, this);
	addSubTask(createCoalition);
	assigned = true;
	std::cout << "Expand: Assign End\n";
}

void Expand::act()
{
	std::cout << "Expand: Act\n";
	acting = true;
	complete = builder->expand() && !builder->isMovingToBuild();
	std::cout << "Expand: Act End\n";
}

void Expand::update()
{
	if (!assigned)
		assign();
	if (coalition->isActive() && !builder)
	{
		std::cout << "Assigning Builder\n";
		builder = (*coalition->getAgentSet().begin());		
	}
	if (builder && !complete)
	{
		std::cout << "Acting\n";
		act();
	}
}