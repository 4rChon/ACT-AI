#pragma once
#include "CoalitionHelper.h"

namespace CoalitionHelper
{
	namespace
	{
		static Coalitionset coalitionSet;
		static int nextID;
	}
	
	void initialiseHelper()
	{
		nextID = 0;
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
	
	Coalition* addCoalition(Composition composition, Task* task)
	{
		Coalition* coalition = new Coalition(composition, task);
		coalitionSet.insert(coalition);
		return coalition;
	}

	void removeCoalition(Coalition* coalition)
	{
		if (coalitionSet.count(coalition) > 0)
		{
			coalitionSet.erase(coalition);
			delete coalition;
		}
	}
};