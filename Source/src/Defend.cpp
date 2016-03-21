#include "Defend.h"
#include "CreateCoalition.h"
#include "EconHelper.h"
#include <string>

Defend::Defend()
{
	taskName = "Defend()";
	taskType = DEF;
}

void Defend::createCoalition()
{
	Composition c;
	//c = getComposition(taskType);
	CreateCoalition *createCoalition = new CreateCoalition(c, this);
	addSubTask(createCoalition);
}

void Defend::assign()
{
	createCoalition();
	assigned = true;
}

void Defend::act()
{
	acting = true;
}

void Defend::update()
{
	if (complete)
		return;

	if (!assigned)
	{
		assign();
		return;
	}

	if (!acting)
		act();
}
