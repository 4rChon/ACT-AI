#include "..\include\Attack.h"
#include "..\include\Scout.h"
#include "..\include\CreateCoalition.h"

Attack::Attack(MapHelper::Zone* target)
{
	taskName = "Attack(Zone*)";
	this->target = target;
	scouting = false;
}

// assign an attacking coalition
void Attack::assign()
{
	if (BWAPI::Broodwar->getFrameCount() - target->getLastVisited() > 5)
	{
		if (!scouting)
		{
			Scout* scout = new Scout(target);
			addSubTask(scout);
			scouting = true;
		}
		return;
	}

	std::cout << "Attack: Assign\n";
	Composition c;
	c.addType(BWAPI::UnitTypes::Terran_Marine, 5);
	CreateCoalition *createCoalition = new CreateCoalition(c, this);
	addSubTask(createCoalition);
	assigned = true;
}

// attack with coalition
void Attack::act()
{
	std::cout << "Attack: Act\n";
	coalition->getUnitSet().attack(target->getRegion()->getCenter());
	acting = true;
}

void Attack::update()
{
	if (complete)
	{
		cleanSubTasks();
		return;
	}

	if (!assigned)
		assign();

	if (assigned && coalition->isActive())
		act();

	if (BWAPI::Broodwar->getFrameCount() - target->getLastVisited() < 5 && target->getEnemyScore() == 0)
		succeed();
}