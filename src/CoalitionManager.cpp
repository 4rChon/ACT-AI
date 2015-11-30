#include "CoalitionManager.h"

CoalitionManager* CoalitionManager::instance = nullptr;

CoalitionManager::CoalitionManager()
{
	this->openCoalitions = 0;
}

CoalitionManager::~CoalitionManager()
{
	this->coalitionSet.clear();
	this->instance = nullptr;
}

CoalitionManager* CoalitionManager::getInstance()
{
	if (!instance)
		instance = new CoalitionManager();
	return instance;
}

//get coalition by id
Coalition* CoalitionManager::getCoalition(int coalitionID)
{
	for (auto &coalition : this->coalitionSet)
		if (coalition->getID() == coalitionID)
			return coalition;
	return nullptr;
}

//get set of all coalitions
Coalitionset& CoalitionManager::getCoalitionset()
{
	return this->coalitionSet;
}

//get open coalitions
int CoalitionManager::getOpenCount()
{
	this->openCoalitions = 0;
	for (auto &coalition : this->coalitionSet)
		if (!coalition->isActive())
			openCoalitions++;
	return openCoalitions;
}

//returns id of added coalition
int CoalitionManager::addCoalition(Composition composition, Task* task)
{
	Coalition* coalition = new Coalition(composition, task->getID(), task->getType());
	this->coalitionSet.insert(coalition);
	return coalition->getID();
}

//remove coalition by id
void CoalitionManager::removeCoalition(int id)
{
	this->removeCoalition(this->getCoalition(id));
}

//remove coalition by object
void CoalitionManager::removeCoalition(Coalition* coalition)
{
	this->coalitionSet.erase(coalition);
}

//remove coalition by iterator
Coalitionset::iterator CoalitionManager::removeCoalition(Coalitionset::iterator coalition)
{
	return this->coalitionSet.erase(coalition);
}

//void activateCoalition(Coalition* coalition)
//{
//	
//}
//
//void disbandCoalition(Coalition* coalition)
//{
//	
//}