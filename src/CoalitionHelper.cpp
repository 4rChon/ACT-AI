#pragma once
#include "..\include\CoalitionHelper.h"

namespace CoalitionHelper
{
	namespace
	{
		static Coalitionset coalitionSet;
		static int nextID;
		static int openCoalitions;
	}
	
	Coalition* getCoalition(int coalitionID)
	{
		for (auto &coalition : coalitionSet)
			if (coalition->getID() == coalitionID)
				return coalition;
		return nullptr;
	}

	const Coalitionset& getCoalitionset()
	{
		return coalitionSet;
	}

	int getNextID()
	{
		return ++nextID;
	}
	
	void addCoalition(Composition composition, Task * task)
	{
		Coalition* coalition = new Coalition(composition, task);
		coalitionSet.insert(coalition);
	}

	void removeCoalition(Coalition* coalition)
	{
		coalitionSet.erase(coalition);
	}
	//Coalitionset::iterator removeCoalition(Coalitionset::iterator coalition);
	//void activateCoalition(Coalition* coalition);
	//void disbandCoalition(Coalition* coalition);
};