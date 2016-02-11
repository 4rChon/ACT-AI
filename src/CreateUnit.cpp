#include "..\include\CreateUnit.h"
#include "..\include\CreateCoalition.h"
#include "..\include\EconHelper.h"
//#include "..\include\SatisfyRequirement.h"

CreateUnit::CreateUnit(BWAPI::UnitType unitType, int unitCount)
{
	taskName = "CreateUnit(unitType)";
	this->unitType = unitType;
	this->unitCount = unitCount;
	satisfied = false;
	building = false;
	reserved = false;
}

// assign a producer coalition
void CreateUnit::assign()
{
	std::cout << "CreateUnit: Assign - " << unitType.whatBuilds().first.c_str() << "\n";
	Composition producer;
	producer.addType(unitType.whatBuilds().first, unitType.whatBuilds().second);
	CreateCoalition* createCoalition = new CreateCoalition(producer, this);
	addSubTask(createCoalition);
	assigned = true;
}

// produce a unit
void CreateUnit::act()
{
	while (unitCount > 0)
	{
		if (BWAPI::Broodwar->self()->isUnitAvailable(unitType))
		{
			if (satisfied)
				return;

			//std::cout << unitType.c_str() << " is locked\n";
			/*SatisfyRequirement* satisfyRequirement = new SatisfyRequirement(unitType);
			this->addSubTask(satisfyRequirement);*/
			satisfied = true;
			return;
		}
		
			
		if (!EconHelper::haveMoney(unitType))
		{
			std::cout << "Not Enough Minerals\n";
			return;
		}

		if (!EconHelper::haveSupply(unitType))
		{
			std::cout << "You must construct additional pylons\n";
			return;
		}

		if (unitType.isAddon())
		{
			if (!coalition->getUnitSet().buildAddon(unitType))
				return;
			unitCount--;
		}

		if (unitType.isBuilding() && unitType.whatBuilds().first == BWAPI::Broodwar->self()->getRace().getWorker())
		{
			if (building)
				return;

			if (!reserved)
			{
				std::cout << "I have enough resources to build a " << unitType.c_str() << "\n";
				std::cout << "Reserving " << unitType.mineralPrice() << " Minerals and " << unitType.gasPrice() << " Gas to build a " << unitType.c_str() << "\n";
				EconHelper::addDebt(unitType.mineralPrice(), unitType.gasPrice());
				reserved = true;
			}

			for (auto builder : coalition->getUnitSet())
			{
				//std::cout << "Selecting Builder\n";
				BWAPI::TilePosition targetBuildLocation = BWAPI::Broodwar->getBuildLocation(unitType, builder->getTilePosition());

				if (targetBuildLocation)
				{
					//std::cout << "I found a suitable location to build a " << this->unitType.c_str() << "\n";

					if (!builder->move(BWAPI::Position(targetBuildLocation.x * BWAPI::TILEPOSITION_SCALE, targetBuildLocation.y * BWAPI::TILEPOSITION_SCALE)))
						return;

					if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg)
					{
						if (!builder->morph(true)) return;
						unitCount--;
					}
					else
					{
						building = builder->build(unitType, targetBuildLocation);

						if (!building)
							return;

						//std::cout << "Moving to build a " << this->unitType.c_str() << "\n";							
						int min = unitType.mineralPrice();
						int gas = unitType.gasPrice();
						auto uType = unitType;

						BWAPI::Broodwar->registerEvent([builder, min, gas, uType](BWAPI::Game*)
						{
							//std::cout << "Releasing " << min << " Minerals and " << gas << " Gas after placing a " << uType.c_str() << "\n";
							EconHelper::subtractDebt(min, gas);
						},
							[builder](BWAPI::Game*) {return builder->getOrder() == BWAPI::Orders::ConstructingBuilding; },
							1);

						unitCount--;
					}
				}
			}
		}
		else
		{
			if (coalition->getUnitSet().size() == 0)
				return;

			for (auto producer : coalition->getUnitSet())
			{
				if (producer->getTrainingQueue().size() < 1)
				{
					if (coalition->getUnitSet().train(unitType))
						unitCount--;
					else
						return;
				}
				else
					return;
			}
		}
	}
	acting = true;
}

void CreateUnit::update()
{
	if (complete)
		return;
	if (!assigned)
		assign();
	if (coalition->isActive() && !acting)
		act();	
	if (acting)
		succeed();
}