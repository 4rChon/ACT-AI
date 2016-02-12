#include "..\include\CreateUnit.h"
#include "..\include\CreateCoalition.h"
#include "..\include\EconHelper.h"
#include "..\include\SatisfyRequirement.h"

CreateUnit::CreateUnit(BWAPI::UnitType unitType, int unitCount)
{
	taskName = "CreateUnit(" + unitType.getName() + ")";
	this->unitType = unitType;
	this->unitCount = unitCount;
	satisfyAttempt = false;
	building = false;
	reserved = false;
}

// assign a producer coalition
void CreateUnit::assign()
{
	//std::cout << taskName.c_str() << " : " << taskID << " : Assign\n";
	std::cout << "CreateUnit: Assign - " << unitType.whatBuilds().first.c_str() << "\n";
	Composition producer;
	producer.addType(unitType.whatBuilds().first, unitType.whatBuilds().second);
	CreateCoalition* createCoalition = new CreateCoalition(producer, this);
	subTasks.insert(createCoalition);
	assigned = true;
	//std::cout << taskName.c_str() << " : " << taskID << " : Assign End\n";
}

// produce a unit
void CreateUnit::act()
{
	//std::cout << taskName.c_str() << " : " << taskID << " : Acting\n";
	if (unitCount > 0)
	{		
		if (satisfyAttempt)
		{
			if (unitType == BWAPI::UnitTypes::Protoss_Archon)
			{
				if (coalition->getUnitSet().train(unitType))
					unitCount--;
			}

			if (unitType.isAddon())
			{
				for (auto &agent : coalition->getAgentSet())
				{
					if (agent->buildAddon(unitType))
						unitCount--;
				}
			}

			if (unitType.isBuilding())
			{
				for (auto &agent : coalition->getAgentSet())
				{
					if (agent->build(unitType, nullptr))
						unitCount--;
				}
			}
			else
			{
				for (auto &agent : coalition->getAgentSet())
				{
					if (agent->train(unitType))
						unitCount--;
				}
			}

		}
		else if (!satisfyAttempt)
		{
			for (auto &required : unitType.requiredUnits())
			{
				if (!BWAPI::Broodwar->self()->hasUnitTypeRequirement(required.first))
				{
					std::cout << "Attempting to Satisfy Requirement\n";
					SatisfyRequirement* satisfyRequirement = new SatisfyRequirement(unitType);
					subTasks.insert(satisfyRequirement);
					break;
				}
			}
			satisfyAttempt = true;
		}
	}
	else
		succeed();
	//std::cout << taskName.c_str() << " : " << taskID << " : Acting End\n";
}

void CreateUnit::update()
{
	//std::cout << taskName.c_str() << " : " << taskID << " : Update\n";
	if (complete)
		return;

	if (!assigned)
		assign();

	if (coalition->isActive())
		act();
	//std::cout << taskName.c_str() << " : " << taskID << " : Update End\n";
}