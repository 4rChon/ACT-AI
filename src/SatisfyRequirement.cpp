#include "..\include\SatisfyRequirement.h"
#include "..\include\CreateUnit.h"

SatisfyRequirement::SatisfyRequirement(BWAPI::UnitType unitType)
{
	std::cout << taskName.c_str() << " : " << taskID << " : Construct\n";
	this->unitType = unitType;
}

void SatisfyRequirement::assign()
{
	//std::cout << taskName.c_str() << " : " << taskID << " : Assign\n";
	assigned = true;
	//std::cout << taskName.c_str() << " : " << taskID << " : Assign End\n";
}

void SatisfyRequirement::act()
{
	//create units to satisfy requirements
	//std::cout << taskName.c_str() << " : " << taskID << " : Acting\n";
	for (auto &requirement : unitType.requiredUnits())
	{
		std::cout << "required unit: " << requirement.first.c_str() << "\n";
		if (AgentHelper::getTypeCountMap()[requirement.first] < requirement.second)
		{
			if (BWAPI::Broodwar->self()->incompleteUnitCount(requirement.first) >= requirement.second)
				continue;
			CreateUnit* createUnit = new CreateUnit(requirement.first, requirement.second - AgentHelper::getTypeCountMap()[requirement.first]);
			std::cout << "Creating Requirement: " << requirement.first.c_str() << " : " << requirement.second << "\n";
			subTasks.insert(createUnit);
		}
	}
	acting = true;
	//std::cout << taskName.c_str() << " : " << taskID << " : Acting End\n";
}

void SatisfyRequirement::update()
{
	//std::cout << taskName.c_str() << " : " << taskID << " : Update\n";
	if (!assigned)
		assign();
	if (complete)
	{
		succeed();
		return;
	}
	if(!acting)
		act();

	if (acting)
	{
		for (auto &task : subTasks)
			if (!task->isComplete())
				return;
		complete = true;
		std::cout << "SatisfyRequirement: Complete\n";
	}
	//std::cout << taskName.c_str() << " : " << taskID << " : Update End\n";
}

