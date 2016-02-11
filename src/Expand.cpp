#include "..\include\Composition.h"
//#include "..\include\CreateCoalition.h"
#include "..\include\Expand.h"
#include "..\include\EconHelper.h"
#include "..\include\CreateCoalition.h"

Expand::Expand()
{
	taskName = "Expand()";
}

// assign an expanding coalition
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

// expandwith coalition
void Expand::act()
{
	std::cout << "Expand: Act\n";
	//EconHelper::addDebt(BWAPI::Broodwar->self()->getRace().getCenter().mineralPrice(), 0);
	acting = ((*coalition->getAgentSet().begin())->expand() && !(*coalition->getAgentSet().begin())->isMovingToBuild());		
	std::cout << "Expand: End\n";
}

void Expand::update()
{
	if (complete)
		return;
	if (!assigned)
		assign();
	if (coalition->isActive() && !acting)
		act();
}