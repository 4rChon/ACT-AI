#include "..\include\Producer.h"
#include "..\include\EconHelper.h"

Producer::Producer()
{
}

Producer::Producer(BWAPI::Unit unit)
{
	this->unit = unit;
	unitID = unit->getID();
}

Producer::~Producer()
{
	std::cout << "\t~Producer\n";
}

void Producer::act()
{	
	if (unit->getType() == BWAPI::UnitTypes::Terran_Barracks)
		train(BWAPI::UnitTypes::Terran_Marine);
	else
		train(*unit->getType().buildsWhat().begin());
	/*if (!train(unit->getPlayer()->getRace().getWorker()))
	unit->train(unit->getPlayer()->getRace().getSupplyProvider());*/
}

bool Producer::train(BWAPI::UnitType unitType)
{
	std::cout << "Building train " << unitType.c_str() << "\n";
	for (auto t : unit->getType().buildsWhat())
		std::cout << "\tunit build types: " << t.c_str() << "\n";
	if (EconHelper::haveMoney(unitType) && EconHelper::haveSupply(unitType))
	{
		std::cout << unit->getType() << " : Attempting to train a " << unitType.c_str() << "\n";
		return this->unit->train(unitType);
	}
	return false;
}